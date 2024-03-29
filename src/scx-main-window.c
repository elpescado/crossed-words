/*
 * Scrabble
 * --------
 *
 * Copyright (C) 2009 Przemysław Sitek
 *
 */

#include <gtk/gtk.h>
#include <glib/gi18n.h>

#include "common.h"
#include "sc-game.h"
#include "sc-dumb-player.h"
#include "sc-computer-player.h"
#include "sc-human-player.h"
#include "sc-noob-player.h"
#include "sc-pro-player.h"
#include "sc-sim-player.h"

#include "scx-alphabet-panel.h"
#include "scx-console.h"
#include "scx-board-view.h"
#include "scx-main-window.h"
#include "scx-move-entry.h"
#include "scx-new-game-dialog.h"
#include "scx-rack-view.h"
#include "scx-game-panel.h"

#include "version.h"


G_DEFINE_TYPE (ScxMainWindow, scx_main_window, GTK_TYPE_WINDOW)

struct _ScxMainWindowPrivate
{
	/* Private members go here */

	/* Actions */
	GtkActionGroup  *action_group;
	GtkUIManager    *ui;

	/* Widgets */
	GtkWidget *vbox;
	GtkWidget *menubar;
	GtkWidget *splitter;
	GtkWidget *notebook;
	GtkWidget *board_view;
	GtkWidget *bottom_hbox;
	GtkWidget *rack_view;
	GtkWidget *move_entry;
	GtkWidget *statusbar;
	GtkWidget *score_label;

	/* Panel */
	GtkWidget *game_panel;
	GtkWidget *alphabet_panel;

	ScGame    *game;

	ScPlayer  *current_player;

	ScPlayerFactory *factory;

	gint       zoom_level;

	gboolean disposed;
};



gint tile_sizes [] = {16, 24, 32, 48, 64, 96};
const gint n_tile_sizes = sizeof(tile_sizes)/sizeof(tile_sizes[0]);
const gint default_zoom_level = 2;


#define SCX_MAIN_WINDOW_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	SCX_TYPE_MAIN_WINDOW, ScxMainWindowPrivate))

static void
scx_main_window_players_turn_cb (ScPlayer      *player,
                                 ScxMainWindow *self);

static void
scx_main_window_update_move (ScxMainWindow *self);

static void
scx_main_window_move_activated (ScxMoveEntry  *entry,
                                ScMoveType     move_type,
                                ScxMainWindow *self);

static void
scx_main_window_game_begin (ScxMainWindow *self,
                            ScGame        *game);

static void
scx_main_window_game_end (ScxMainWindow *self,
                            ScGame        *game);


static void
scx_main_window_game_tick (ScxMainWindow *self,
                           ScGame        *game);

GtkWidget*
scx_main_window_new (void)
{
	ScxMainWindow *self = g_object_new (SCX_TYPE_MAIN_WINDOW, NULL);
	return GTK_WIDGET (self);
}


/**
 * Start a new game
 **/
static void
_action_game_new  (GtkAction     *action,
                   ScxMainWindow *self)
{
	ScxMainWindowPrivate *priv = self->priv;

	GtkWidget *win = scx_new_game_dialog_new ();
	scx_new_game_dialog_set_factory (SCX_NEW_GAME_DIALOG (win), priv->factory);
	gint response = gtk_dialog_run (GTK_DIALOG (win));
	gtk_widget_hide (win);

	if (response != GTK_RESPONSE_ACCEPT)
		return;

	priv->game = sc_game_new (NULL);
	sc_game_set_time (priv->game, 60 * scx_new_game_dialog_get_time (SCX_NEW_GAME_DIALOG (win)));

	scx_board_view_set_board (SCX_BOARD_VIEW (priv->board_view),
	                          sc_game_get_board (priv->game));

	gint i;
	for (i = 0; i < 2; i++) {
		ScPlayer *player = scx_new_game_dialog_create_player (SCX_NEW_GAME_DIALOG (win), i);
		sc_game_set_player (priv->game, i, player);
		player->game = (void*)priv->game;
		g_signal_connect (player, "your-turn", G_CALLBACK (scx_main_window_players_turn_cb), self);
	}

	gtk_widget_destroy (win);

	g_signal_connect_swapped (priv->game, "begin", G_CALLBACK (scx_main_window_game_begin), self);
	g_signal_connect_swapped (priv->game, "end", G_CALLBACK (scx_main_window_game_end), self);
	g_signal_connect_swapped (priv->game, "tick", G_CALLBACK (scx_main_window_game_tick), self);

	scx_board_view_enable_selection (SCX_BOARD_VIEW (priv->board_view));
	scx_board_view_set_selection (SCX_BOARD_VIEW (priv->board_view), 3, 7);

	scx_alphabet_panel_set_alphabet (SCX_ALPHABET_PANEL (priv->alphabet_panel),
	                                 sc_game_get_alphabet (priv->game));

	sc_game_start (priv->game);
}


static void
_action_help_about (GtkAction     *action,
                    ScxMainWindow *self)
{
	const gchar *authors[] = {"Przemysław Sitek", NULL};

#ifndef G_OS_WIN32
	GdkPixbuf *icon = gdk_pixbuf_new_from_file (PIXMAP_PATH "crossed-words.png", NULL);
#endif

	gtk_show_about_dialog (GTK_WINDOW (self),
			"program-name", _("Crossed Words"),
			"version", VERSION,
			"comments", _("Word game playing program"),
			"website", "http://code.google.com/p/crossed/words/",
			"authors",  authors,
			"copyright", "(c) 2009 Przemysław Sitek",
			"license", "GNU LGPL 2.1",
			"translator-credits", _("translator-credits"),
#ifndef G_OS_WIN32
			"logo", icon,
#endif
			NULL);
}



static void
set_zoom (ScxMainWindow *self,
          gint zoom_level)
{
	ScxMainWindowPrivate *priv = self->priv;
	if (zoom_level < 0)
		zoom_level = 0;
	if (zoom_level >= n_tile_sizes)
		zoom_level = n_tile_sizes - 1;

	priv->zoom_level = zoom_level;

	scx_board_view_set_tile_size (SCX_BOARD_VIEW (priv->board_view), tile_sizes[priv->zoom_level]);	
}


static void
_action_zoom_in (GtkAction     *action,
                 ScxMainWindow *self)
{
	ScxMainWindowPrivate *priv = self->priv;
	set_zoom (self, priv->zoom_level+1);
}


static void
_action_zoom_out (GtkAction     *action,
                  ScxMainWindow *self)
{
	ScxMainWindowPrivate *priv = self->priv;
	set_zoom (self, priv->zoom_level-1);
}





static const gchar* ui_markup =
"<ui>"
	"<menubar>"
		"<menu action='Game'>"
			"<menuitem action='New' />"
			"<separator />"
			"<menuitem action='Quit' />"
		"</menu>"
		"<menu action='View'>"
			"<menuitem action='ZoomIn' />"
			"<menuitem action='ZoomOut' />"
		"</menu>"
		"<menu action='Help'>"
			"<menuitem action='About' />"
		"</menu>"
	"</menubar>"
	"<toolbar>"
		"<toolitem action='New' />"
		"<separator />"
		"<toolitem action='ZoomIn' />"
		"<toolitem action='ZoomOut' />"
	"</toolbar>"
"</ui>";


static const GtkActionEntry actions[] = {
	{"Game", NULL, N_("Game")},
		{"New", GTK_STOCK_NEW, NULL, "<Ctrl>n", N_("New game"), G_CALLBACK (_action_game_new)},
		{"Quit", GTK_STOCK_QUIT, NULL, "<Ctrl>q", NULL, G_CALLBACK (gtk_main_quit)},
	{"View", NULL, N_("View")},
		{"ZoomIn", GTK_STOCK_ZOOM_IN, NULL, "<Ctrl>plus", NULL, G_CALLBACK (_action_zoom_in)},
		{"ZoomOut", GTK_STOCK_ZOOM_OUT, NULL, "<Ctrl>minus", NULL, G_CALLBACK (_action_zoom_out)},
	{"Help", NULL, N_("Help")},
		{"About", GTK_STOCK_ABOUT, NULL, NULL, NULL, G_CALLBACK (_action_help_about)}
};

static const guint n_actions = G_N_ELEMENTS (actions);


static void
scx_main_window_init_actions (ScxMainWindow *self)
{
	ScxMainWindowPrivate *priv = self->priv;

	GError *error = NULL;

	priv->action_group = gtk_action_group_new ("sc");
	//gtk_action_group_set_translation_domain (browser->action_group, GETTEXT_PACKAGE);
	gtk_action_group_add_actions (priv->action_group, actions, n_actions, self);

	priv->ui = gtk_ui_manager_new ();
	gtk_ui_manager_insert_action_group (priv->ui, priv->action_group, 0);
	gtk_window_add_accel_group (GTK_WINDOW (self), gtk_ui_manager_get_accel_group (priv->ui));

	if (! gtk_ui_manager_add_ui_from_string (priv->ui, ui_markup, -1, &error)) {
		g_printerr (" *** ERROR??? %s\n", error->message);
		g_error_free (error);
	}
}


static void
scx_main_window_add_sidebar_panel (ScxMainWindow *self, GtkWidget *panel, const gchar *label_text)
{
	ScxMainWindowPrivate *priv = self->priv;

	GtkWidget *label = gtk_label_new (label_text);
	gtk_label_set_angle (GTK_LABEL (label), 90.0);
	gtk_notebook_append_page (GTK_NOTEBOOK (priv->notebook), panel, label);
	gtk_widget_show (label);
}


static void
scx_main_window_init_sidebar (ScxMainWindow *self)
{
	ScxMainWindowPrivate *priv = self->priv;
	GtkWidget *sample;

	sample = scx_game_panel_new ();
	scx_main_window_add_sidebar_panel (self, sample, _("Game"));
	gtk_widget_show (sample);
	priv->game_panel = sample;

	sample = scx_alphabet_panel_new ();
	scx_main_window_add_sidebar_panel (self, sample, _("Alphabet"));
	gtk_widget_show (sample);
	priv->alphabet_panel = sample;

	sample = gtk_label_new ("Crossed Words");
	scx_main_window_add_sidebar_panel (self, sample, _("History"));
	gtk_widget_show (sample);

	sample = scx_console_new ();
	scx_main_window_add_sidebar_panel (self, sample, _("Console"));
	gtk_widget_show (sample);

	gtk_notebook_set_current_page (GTK_NOTEBOOK (priv->notebook), 0);
}



static void
scx_main_window_init_gui (ScxMainWindow *self)
{
	ScxMainWindowPrivate *priv = self->priv;

	gtk_window_set_title (GTK_WINDOW (self), _("Crossed Words"));
	gtk_window_set_default_size (GTK_WINDOW (self), 1024, 768);

	/* Main vbox */
	priv->vbox = gtk_vbox_new (FALSE, 0);
	gtk_container_add (GTK_CONTAINER (self), priv->vbox);
	gtk_widget_show (priv->vbox);

	/* Main menu placeholder */
	GtkWidget *menubar = gtk_ui_manager_get_widget (priv->ui, "/menubar");
	gtk_box_pack_start (GTK_BOX (priv->vbox), menubar, FALSE, FALSE, 0);	// FIXME: Warning here
	gtk_widget_show (menubar);

	/* Toolbar placeholder */
	GtkWidget *toolbar = gtk_ui_manager_get_widget (priv->ui, "/toolbar");
	gtk_box_pack_start (GTK_BOX (priv->vbox), toolbar, FALSE, FALSE, 0);
	gtk_widget_show (toolbar);


	/* splitter */
	priv->splitter = gtk_hpaned_new ();
	gtk_paned_set_position (GTK_PANED (priv->splitter), 240);
	gtk_box_pack_start (GTK_BOX (priv->vbox), priv->splitter, TRUE, TRUE, 0);
	gtk_widget_show (priv->splitter);

	/* Left pane */
	priv->notebook = gtk_notebook_new ();
	gtk_notebook_set_tab_pos (GTK_NOTEBOOK (priv->notebook), GTK_POS_LEFT);
	gtk_paned_add1 (GTK_PANED (priv->splitter), priv->notebook);
	gtk_widget_show (priv->notebook);

	/* Right pane */
	GtkWidget *vbox2 = gtk_vbox_new (FALSE, 6);
	gtk_paned_add2 (GTK_PANED (priv->splitter), vbox2);
	gtk_widget_show (vbox2);

	/* Board view */
	GtkWidget *sw = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start (GTK_BOX (vbox2), sw, TRUE, TRUE, 0);
	gtk_widget_show (sw);

	priv->board_view = scx_board_view_new ();
	g_signal_connect_swapped (priv->board_view, "changed",
	                          G_CALLBACK (scx_main_window_update_move), self);
	gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (sw), priv->board_view);
	gtk_widget_show (priv->board_view);

	/* Bottom box */
	priv->bottom_hbox = gtk_hbox_new (FALSE, 6);
	gtk_box_pack_start (GTK_BOX (vbox2), priv->bottom_hbox, FALSE, FALSE, 0);
	gtk_widget_show (priv->bottom_hbox);

	/* Rack view */
	priv->rack_view = scx_rack_view_new ();
	gtk_box_pack_start (GTK_BOX (priv->bottom_hbox), priv->rack_view, TRUE, FALSE, 0);
	gtk_widget_show (priv->rack_view);

	/* Score label */
	priv->score_label = gtk_label_new (" 0 ");
	PangoFontDescription *font = pango_font_description_from_string ("bold 32");
	gtk_widget_modify_font (priv->score_label, font);
	gtk_box_pack_start (GTK_BOX (priv->bottom_hbox), priv->score_label, FALSE, FALSE, 0);
	gtk_widget_show (priv->score_label);

	/* Move entry */
	priv->move_entry = scx_move_entry_new ();
	g_signal_connect_swapped (priv->move_entry, "changed",
	                          G_CALLBACK (scx_main_window_update_move), self);
	g_signal_connect (priv->move_entry, "activated",
	                  G_CALLBACK (scx_main_window_move_activated), self);
	gtk_box_pack_start (GTK_BOX (priv->bottom_hbox), priv->move_entry, TRUE, TRUE, 0);
	gtk_widget_show (priv->move_entry);

	priv->statusbar = gtk_statusbar_new ();
	gtk_box_pack_start (GTK_BOX (priv->vbox), priv->statusbar, FALSE, FALSE, 0);
	gtk_widget_show (priv->statusbar);

	scx_main_window_init_sidebar (self);
}


static void
scx_main_window_init (ScxMainWindow *self)
{
	self->priv = SCX_MAIN_WINDOW_GET_PRIVATE (self);
	ScxMainWindowPrivate *priv = self->priv;

	priv->disposed = FALSE;

	priv->factory = sc_player_factory_new ();
	sc_player_factory_add_type (priv->factory, (ScPlayerConstructor)sc_human_player_new,    "human", "Human");
	sc_player_factory_add_type (priv->factory, (ScPlayerConstructor)sc_dumb_player_new,     "dumb", "Dumb player");
	sc_player_factory_add_type (priv->factory, (ScPlayerConstructor)sc_noob_player_new,     "noob", "Noob computer");
	sc_player_factory_add_type (priv->factory, (ScPlayerConstructor)sc_computer_player_new, "computer", "Computer player");
	sc_player_factory_add_type (priv->factory, (ScPlayerConstructor)sc_sim_player_new, "sim", "Simulation");
	//sc_player_factory_add_type (priv->factory, (ScPlayerConstructor)sc_pro_player_new,      "pro", "Pro computer");

	priv->zoom_level = default_zoom_level;

	scx_main_window_init_actions (self);
	scx_main_window_init_gui (self);
	set_zoom (self, default_zoom_level);
}



static void
scx_main_window_game_begin (ScxMainWindow *self,
                            ScGame        *game)
{
	/*
	GtkWidget *win = gtk_message_dialog_new (GTK_WINDOW (self),
	                                         0,
	                                         GTK_MESSAGE_INFO,
	                                         GTK_BUTTONS_OK,
	                                         "Game started");
	gtk_dialog_run (GTK_DIALOG (win));
	gtk_widget_destroy (win);
	*/
}


static void
scx_main_window_game_end (ScxMainWindow *self,
                          ScGame        *game)
{
	ScxMainWindowPrivate *priv = self->priv;

	scx_game_panel_update (SCX_GAME_PANEL (priv->game_panel), priv->game);
	GtkWidget *win = gtk_message_dialog_new (GTK_WINDOW (self),
	                                         0,
	                                         GTK_MESSAGE_INFO,
	                                         GTK_BUTTONS_OK,
	                                         "Game over");
	gtk_dialog_run (GTK_DIALOG (win));
	gtk_widget_destroy (win);
}

static void
scx_main_window_game_tick (ScxMainWindow *self,
                           ScGame        *game)
{
	ScxMainWindowPrivate *priv = self->priv;
	scx_game_panel_update (SCX_GAME_PANEL (priv->game_panel), priv->game);
}


static void
scx_main_window_players_turn_cb (ScPlayer      *player,
                                 ScxMainWindow *self)
{
	ScxMainWindowPrivate *priv = self->priv;

	scx_game_panel_update (SCX_GAME_PANEL (priv->game_panel), priv->game);
	gtk_widget_queue_draw (priv->board_view); // FIXME: use proper signals etc.

	if (! SC_IS_HUMAN_PLAYER(player)) {
		gtk_widget_set_sensitive (priv->bottom_hbox, FALSE);
		return;
	}


	GtkWidget *msg = gtk_message_dialog_new (GTK_WINDOW (self),
	                                         0/*GTK_DIALOG_NO_SEPARATOR*/,
											 GTK_MESSAGE_INFO,
											 GTK_BUTTONS_OK,
	                                         _("Your turn!!!"));
	gtk_dialog_run (GTK_DIALOG (msg));
	gtk_widget_destroy (msg);

	priv->current_player = player;
	scx_board_view_set_move (SCX_BOARD_VIEW (priv->board_view), NULL);

	ScRackModel *rack = sc_game_get_players_rack (priv->game, player);
	scx_rack_view_set_rack (SCX_RACK_VIEW(priv->rack_view), rack);
	g_object_unref (rack);

	scx_move_entry_set_text (SCX_MOVE_ENTRY (priv->move_entry), "");


	gtk_widget_set_sensitive (priv->bottom_hbox, TRUE);
}


static void
scx_main_window_update_move (ScxMainWindow *self)
{
	ScxMainWindowPrivate *priv = self->priv;
	ScMove move;
	gint x, y;

	if (priv->game == NULL)
		return;

	const gchar *word = scx_move_entry_get_text (SCX_MOVE_ENTRY (priv->move_entry));
	if (*word == '\0') {
		scx_move_entry_set_validation_status (SCX_MOVE_ENTRY (priv->move_entry), SCX_NONE);
		return;
	}
	ScOrientation o = scx_move_entry_get_orientation (SCX_MOVE_ENTRY (priv->move_entry));
	scx_board_view_get_selection (SCX_BOARD_VIEW (priv->board_view), &x, &y);

	if (sc_game_init_move (priv->game, &move, x, y, o, word)) {
		scx_board_view_set_move (SCX_BOARD_VIEW (priv->board_view), &move);

		gchar rating_str[12];
		gint move_rating = sc_board_rate_move (sc_game_get_board (priv->game), &move);
		snprintf (rating_str, 12, " %d ", move_rating);
		gtk_label_set_text (GTK_LABEL (priv->score_label), rating_str);

		scx_move_entry_set_validation_status (SCX_MOVE_ENTRY (priv->move_entry),
				sc_game_validate_move (priv->game, sc_game_get_current_player (priv->game), &move) ? SCX_VALID : SCX_INVALID);
				//sc_board_validate_move (sc_game_get_board (priv->game), &move) ? SCX_VALID : SCX_INVALID);
	}
}


static void
scx_main_window_move_activated (ScxMoveEntry  *entry,
                                ScMoveType     move_type,
                                ScxMainWindow *self)
{
	ScxMainWindowPrivate *priv = self->priv;

	if (priv->game == NULL)
		return;

//	if (move_type == SC_MOVE_TYPE_MOVE) { // this is now handled by ScGame itself
		g_print ("Move was done!!!\n");

		ScMove move;
		gint x, y;

		const gchar *word = scx_move_entry_get_text (SCX_MOVE_ENTRY (priv->move_entry));
		ScOrientation o = scx_move_entry_get_orientation (SCX_MOVE_ENTRY (priv->move_entry));
		scx_board_view_get_selection (SCX_BOARD_VIEW (priv->board_view), &x, &y);

		if (sc_game_init_move (priv->game, &move, x, y, o, word)) {
			move.type = move_type;

			sc_player_do_move (priv->current_player, &move);
		}

//		scx_board_view_set_move (SCX_BOARD_VIEW (priv->board_view), &move);
//	}
}


static void
scx_main_window_dispose (GObject *object)
{
	ScxMainWindow *self = (ScxMainWindow*) object;
	ScxMainWindowPrivate *priv = self->priv;


	/* Make sure dispose is called only once */
	if (priv->disposed) {
		return;
	}
	priv->disposed = TRUE;


	/* Chain up to the parent class */
	G_OBJECT_CLASS (scx_main_window_parent_class)->dispose (object);
}


static void
scx_main_window_finalize (GObject *object)
{
	G_OBJECT_CLASS (scx_main_window_parent_class)->finalize (object);
}


static void
scx_main_window_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
	ScxMainWindow* self = SCX_MAIN_WINDOW (object);
	G_UNUSED (self);

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
scx_main_window_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
	ScxMainWindow* self = SCX_MAIN_WINDOW (object);
	G_UNUSED (self);

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
scx_main_window_class_init (ScxMainWindowClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->get_property = scx_main_window_get_property;
	gobject_class->set_property = scx_main_window_set_property;
	gobject_class->dispose = scx_main_window_dispose;
	gobject_class->finalize = scx_main_window_finalize;

	g_type_class_add_private (klass, sizeof (ScxMainWindowPrivate));
}

