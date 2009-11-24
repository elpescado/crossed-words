/*
 * Crossed Words
 * -------------
 *
 * Copyright (C) 2009 Przemysław Sitek
 * 
 */
#include <stdio.h>

#include <gtk/gtk.h>
#include <glib/gi18n.h>

#include "scx-player-view.h"


G_DEFINE_TYPE (ScxPlayerView, scx_player_view, GTK_TYPE_TABLE)

struct _ScxPlayerViewPrivate
{
	/* Private members go here */
	GtkWidget *title_label;
	GtkWidget *score_label;

	gboolean disposed;
};


#define SCX_PLAYER_VIEW_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	SCX_TYPE_PLAYER_VIEW, ScxPlayerViewPrivate))


GtkWidget*
scx_player_view_new (void)
{
	ScxPlayerView *self = g_object_new (SCX_TYPE_PLAYER_VIEW, NULL);
	return GTK_WIDGET (self);
}


static void
scx_player_view_init_gui (ScxPlayerView *self)
{
	GtkTable *table = GTK_TABLE (self);
	ScxPlayerViewPrivate *priv = self->priv;

	priv->title_label = gtk_label_new ("");
	PangoFontDescription *font = pango_font_description_from_string ("bold");
	gtk_widget_modify_font (priv->title_label, font);
	gtk_table_attach (table, priv->title_label, 0, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
	gtk_widget_show (priv->title_label);

	GtkWidget *label1 = gtk_label_new (_("Score:"));
	gtk_table_attach (table, label1, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
	gtk_widget_show (label1);

	priv->score_label = gtk_label_new ("");
	gtk_table_attach (table, priv->score_label, 1, 2, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
	gtk_widget_show (priv->score_label);

}


static void
scx_player_view_init (ScxPlayerView *self)
{
	self->priv = SCX_PLAYER_VIEW_GET_PRIVATE (self);
	ScxPlayerViewPrivate *priv = self->priv;

	priv->disposed = FALSE;

	scx_player_view_init_gui (self);
}


void
scx_player_view_set_player (ScxPlayerView *self,
                            ScGame        *game,
                            ScPlayer      *player)
{
	ScxPlayerViewPrivate *priv = self->priv;
	gint score = sc_game_get_players_score (game, player);

	gchar buf[32];
	snprintf (buf, 32, "%d", score);
	gtk_label_set_text (GTK_LABEL (priv->score_label), buf);

	gtk_label_set_text (GTK_LABEL (priv->title_label),
			sc_player_get_name (player));
}


static void
scx_player_view_dispose (GObject *object)
{
	ScxPlayerView *self = (ScxPlayerView*) object;
	ScxPlayerViewPrivate *priv = self->priv;


	/* Make sure dispose is called only once */
	if (priv->disposed) {
		return;
	}
	priv->disposed = TRUE;


	/* Chain up to the parent class */
	G_OBJECT_CLASS (scx_player_view_parent_class)->dispose (object);
}


static void
scx_player_view_finalize (GObject *object)
{
	G_OBJECT_CLASS (scx_player_view_parent_class)->finalize (object);
}

	
static void
scx_player_view_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
	//ScxPlayerView* self = SCX_PLAYER_VIEW (object);
	//ScxPlayerViewPrivate* priv = self->priv;

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
scx_player_view_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
	//ScxPlayerView* self = SCX_PLAYER_VIEW (object);
	//ScxPlayerViewPrivate* priv = self->priv;

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
scx_player_view_class_init (ScxPlayerViewClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->get_property = scx_player_view_get_property;
	gobject_class->set_property = scx_player_view_set_property;
	gobject_class->dispose = scx_player_view_dispose;
	gobject_class->finalize = scx_player_view_finalize;

	g_type_class_add_private (klass, sizeof (ScxPlayerViewPrivate));
}

