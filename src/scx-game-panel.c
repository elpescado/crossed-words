/*
 * Crossed Words
 * -------------
 *
 * Copyright (C) 2009 Przemysław Sitek
 * 
 */

#include <gtk/gtk.h>

#include "sc-game.h"
#include "sc-player.h"

#include "scx-game-panel.h"
#include "scx-player-view.h"


G_DEFINE_TYPE (ScxGamePanel, scx_game_panel, GTK_TYPE_VBOX)

struct _ScxGamePanelPrivate
{
	/* Private members go here */
	GtkWidget *views[2];

	gboolean disposed;
};


#define SCX_GAME_PANEL_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	SCX_TYPE_GAME_PANEL, ScxGamePanelPrivate))


GtkWidget*
scx_game_panel_new (void)
{
	ScxGamePanel *self = g_object_new (SCX_TYPE_GAME_PANEL, NULL);
	return GTK_WIDGET (self);
}


static void
scx_game_panel_init_gui (ScxGamePanel *self)
{
	ScxGamePanelPrivate *priv = self->priv;

	GtkBox *pbox = GTK_BOX (self);
	gint i;

	for (i = 0; i < 2; i++) {
		GtkWidget *view = scx_player_view_new ();
		gtk_box_pack_start (pbox, view, FALSE, FALSE, 0);
		gtk_widget_show (view);
		priv->views[i] = view;
	}
}


static void
scx_game_panel_init (ScxGamePanel *self)
{
	self->priv = SCX_GAME_PANEL_GET_PRIVATE (self);
	ScxGamePanelPrivate *priv = self->priv;

	priv->disposed = FALSE;

	scx_game_panel_init_gui (self);
}


void
scx_game_panel_update (ScxGamePanel *self, ScGame *game)
{
	ScxGamePanelPrivate *priv = self->priv;
	gint i;
	gint n = sc_game_get_n_players (game);

	for (i = 0; i < n; i++) {
		scx_player_view_set_player (SCX_PLAYER_VIEW (priv->views[i]), game,
					sc_game_get_player (game, i));
	}
}


static void
scx_game_panel_dispose (GObject *object)
{
	ScxGamePanel *self = (ScxGamePanel*) object;
	ScxGamePanelPrivate *priv = self->priv;


	/* Make sure dispose is called only once */
	if (priv->disposed) {
		return;
	}
	priv->disposed = TRUE;


	/* Chain up to the parent class */
	G_OBJECT_CLASS (scx_game_panel_parent_class)->dispose (object);
}


static void
scx_game_panel_finalize (GObject *object)
{
	G_OBJECT_CLASS (scx_game_panel_parent_class)->finalize (object);
}

	
static void
scx_game_panel_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
	//ScxGamePanel* self = SCX_GAME_PANEL (object);
	//ScxGamePanelPrivate* priv = self->priv;

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
scx_game_panel_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
	//ScxGamePanel* self = SCX_GAME_PANEL (object);
	//ScxGamePanelPrivate* priv = self->priv;

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
scx_game_panel_class_init (ScxGamePanelClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->get_property = scx_game_panel_get_property;
	gobject_class->set_property = scx_game_panel_set_property;
	gobject_class->dispose = scx_game_panel_dispose;
	gobject_class->finalize = scx_game_panel_finalize;

	g_type_class_add_private (klass, sizeof (ScxGamePanelPrivate));
}

