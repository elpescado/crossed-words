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

#include "scx-player-selector.h"


G_DEFINE_TYPE (ScxPlayerSelector, scx_player_selector, GTK_TYPE_TABLE)

struct _ScxPlayerSelectorPrivate
{
	/* Private members go here */
	GtkWidget *type_combo;
	GtkWidget *name_entry;
	GtkListStore *types;

	gboolean disposed;
};


#define SCX_PLAYER_SELECTOR_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	SCX_TYPE_PLAYER_SELECTOR, ScxPlayerSelectorPrivate))


GtkWidget*
scx_player_selector_new (void)
{
	ScxPlayerSelector *self = g_object_new (SCX_TYPE_PLAYER_SELECTOR,
			"column-spacing", 6,
			"row-spacing",    6,
			NULL);
	return GTK_WIDGET (self);
}


static void
scx_player_selector_init_gui (ScxPlayerSelector *self)
{
	ScxPlayerSelectorPrivate *priv = self->priv;
	GtkTable *table = GTK_TABLE (self);

	GtkWidget *type_label = gtk_label_new (_("Type:"));
	gtk_table_attach (table, type_label, 0, 1, 0, 1, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
	gtk_widget_show (type_label);

	priv->type_combo = gtk_combo_box_new_with_model (GTK_TREE_MODEL (priv->types));
	gtk_table_attach (table, priv->type_combo, 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
	gtk_widget_show (priv->type_combo);

	GtkCellRenderer *cell = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (priv->type_combo), cell, TRUE);
	gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (priv->type_combo), cell, "text", 1, NULL);

	GtkWidget *name_label = gtk_label_new (_("Name:"));
	gtk_table_attach (table, name_label, 0, 1, 1, 2, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
	gtk_widget_show (name_label);

	priv->name_entry = gtk_entry_new ();
	gtk_table_attach (table, priv->name_entry, 1, 2, 1, 2, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
	gtk_widget_show (priv->name_entry);

	
}


static void
scx_player_selector_init (ScxPlayerSelector *self)
{
	self->priv = SCX_PLAYER_SELECTOR_GET_PRIVATE (self);
	ScxPlayerSelectorPrivate *priv = self->priv;

	priv->disposed = FALSE;

	priv->types = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_STRING);
	scx_player_selector_init_gui (self);
}



void
scx_player_selector_set_types (ScxPlayerSelector  *self,
                               const char        **types)
{
	ScxPlayerSelectorPrivate *priv = self->priv;

	while (*types) {
		GtkTreeIter iter;
		gtk_list_store_append (priv->types, &iter);
		gtk_list_store_set (priv->types, &iter, 0, *types, 1, *(types+1), -1);

		types+=2;
	}
}


gchar *
scx_player_selector_get_type_name (ScxPlayerSelector *self)
{
	ScxPlayerSelectorPrivate *priv = self->priv;

	GtkTreeIter iter;
	if (gtk_combo_box_get_active_iter (GTK_COMBO_BOX (priv->type_combo), &iter)) {
		gchar *name;

		gtk_tree_model_get (GTK_TREE_MODEL (priv->types), &iter, 0, &name, -1);
		return name;
	} else {
		return NULL;
	}
}


const gchar *
scx_player_selector_get_name (ScxPlayerSelector *self)
{
	ScxPlayerSelectorPrivate *priv = self->priv;
	return gtk_entry_get_text (GTK_ENTRY (priv->name_entry));
}


static void
scx_player_selector_dispose (GObject *object)
{
	ScxPlayerSelector *self = (ScxPlayerSelector*) object;
	ScxPlayerSelectorPrivate *priv = self->priv;


	/* Make sure dispose is called only once */
	if (priv->disposed) {
		return;
	}
	priv->disposed = TRUE;


	/* Chain up to the parent class */
	G_OBJECT_CLASS (scx_player_selector_parent_class)->dispose (object);
}


static void
scx_player_selector_finalize (GObject *object)
{
	G_OBJECT_CLASS (scx_player_selector_parent_class)->finalize (object);
}

	
static void
scx_player_selector_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
	//ScxPlayerSelector* self = SCX_PLAYER_SELECTOR (object);
	//ScxPlayerSelectorPrivate* priv = self->priv;

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
scx_player_selector_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
	//ScxPlayerSelector* self = SCX_PLAYER_SELECTOR (object);
	//ScxPlayerSelectorPrivate* priv = self->priv;

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
scx_player_selector_class_init (ScxPlayerSelectorClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->get_property = scx_player_selector_get_property;
	gobject_class->set_property = scx_player_selector_set_property;
	gobject_class->dispose = scx_player_selector_dispose;
	gobject_class->finalize = scx_player_selector_finalize;

	g_type_class_add_private (klass, sizeof (ScxPlayerSelectorPrivate));
}

