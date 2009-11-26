/*
 * Crossed Words
 * -------------
 *
 * Copyright (C) 2009 Przemysław Sitek
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <gtk/gtk.h>
#include <glib/gi18n.h>

#include "scx-new-game-dialog.h"
#include "scx-player-selector.h"


G_DEFINE_TYPE (ScxNewGameDialog, scx_new_game_dialog, GTK_TYPE_DIALOG)

struct _ScxNewGameDialogPrivate
{
	/* Private members go here */
	ScPlayerFactory *factory;

	GtkWidget *player_entries[2];



	gboolean disposed;
};


#define SCX_NEW_GAME_DIALOG_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	SCX_TYPE_NEW_GAME_DIALOG, ScxNewGameDialogPrivate))


GtkWidget*
scx_new_game_dialog_new (void)
{
	ScxNewGameDialog *self = g_object_new (SCX_TYPE_NEW_GAME_DIALOG, NULL);
	return GTK_WIDGET (self);
}


static void
scx_new_game_dialog_init_gui (ScxNewGameDialog *self)
{
	ScxNewGameDialogPrivate *priv = self->priv;
	GtkWidget *content_area = gtk_dialog_get_content_area (GTK_DIALOG (self));

	gtk_window_set_title (GTK_WINDOW (self), _("New game"));

	GtkWidget *vbox = gtk_vbox_new (FALSE, 6);
	gtk_container_set_border_width (GTK_CONTAINER (vbox), 12);
	gtk_container_add (GTK_CONTAINER (content_area), vbox);
	gtk_widget_show (vbox);

	gint i;
	for (i = 0; i < 2; i++) {
		GtkWidget *sep = gtk_hseparator_new ();
		gtk_box_pack_start (GTK_BOX (vbox), sep, FALSE, FALSE, 0);
		gtk_widget_show (sep);

		GtkWidget *entry = scx_player_selector_new ();
		gtk_box_pack_start (GTK_BOX (vbox), entry, FALSE, FALSE, 0);
		gtk_widget_show (entry);

		priv->player_entries[i] = entry;
	}

	gtk_dialog_add_button (GTK_DIALOG (self), GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);
	gtk_dialog_add_button (GTK_DIALOG (self), GTK_STOCK_OK, GTK_RESPONSE_ACCEPT);
}


static void
scx_new_game_dialog_init (ScxNewGameDialog *self)
{
	self->priv = SCX_NEW_GAME_DIALOG_GET_PRIVATE (self);
	ScxNewGameDialogPrivate *priv = self->priv;

	priv->disposed = FALSE;

	scx_new_game_dialog_init_gui (self);
}


void
scx_new_game_dialog_set_factory (ScxNewGameDialog *self,
                                 ScPlayerFactory  *factory)
{
	ScxNewGameDialogPrivate *priv = self->priv;

	if (priv->factory)
		g_object_unref (priv->factory);

	if (factory) {
		priv->factory = g_object_ref (factory);

		gint i;
		gchar **types = sc_player_factory_get_types (priv->factory);
		for (i = 0; i < 2; i++) {
			scx_player_selector_set_types (SCX_PLAYER_SELECTOR (priv->player_entries[i]), types);
		}
		g_free (types);
	}
}


ScPlayer *
scx_new_game_dialog_create_player (ScxNewGameDialog *self,
                                   gint i)
{
	ScxNewGameDialogPrivate *priv = self->priv;

	gchar *type_name = scx_player_selector_get_type_name (SCX_PLAYER_SELECTOR (priv->player_entries[i]));
	if (type_name == NULL)
		return NULL;

	ScPlayer *player = sc_player_factory_create (priv->factory, type_name);

	const gchar *player_name = scx_player_selector_get_name (SCX_PLAYER_SELECTOR (priv->player_entries[i]));
	sc_player_set_name (player, player_name);

	return player;
}


static void
scx_new_game_dialog_dispose (GObject *object)
{
	ScxNewGameDialog *self = (ScxNewGameDialog*) object;
	ScxNewGameDialogPrivate *priv = self->priv;


	/* Make sure dispose is called only once */
	if (priv->disposed) {
		return;
	}
	scx_new_game_dialog_set_factory (self, NULL);

	priv->disposed = TRUE;


	/* Chain up to the parent class */
	G_OBJECT_CLASS (scx_new_game_dialog_parent_class)->dispose (object);
}


static void
scx_new_game_dialog_finalize (GObject *object)
{
	G_OBJECT_CLASS (scx_new_game_dialog_parent_class)->finalize (object);
}

	
static void
scx_new_game_dialog_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
	//ScxNewGameDialog* self = SCX_NEW_GAME_DIALOG (object);
	//ScxNewGameDialogPrivate* priv = self->priv;

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
scx_new_game_dialog_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
	//ScxNewGameDialog* self = SCX_NEW_GAME_DIALOG (object);
	//ScxNewGameDialogPrivate* priv = self->priv;

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
scx_new_game_dialog_class_init (ScxNewGameDialogClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->get_property = scx_new_game_dialog_get_property;
	gobject_class->set_property = scx_new_game_dialog_set_property;
	gobject_class->dispose = scx_new_game_dialog_dispose;
	gobject_class->finalize = scx_new_game_dialog_finalize;

	g_type_class_add_private (klass, sizeof (ScxNewGameDialogPrivate));
}

