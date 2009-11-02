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
#include "scx-alphabet-panel.h"


G_DEFINE_TYPE (ScxAlphabetPanel, scx_alphabet_panel, GTK_TYPE_VBOX)

struct _ScxAlphabetPanelPrivate
{
	/* Private members go here */
	GtkWidget    *view;
	GtkTreeModel *model;

	gboolean disposed;
};


#define SCX_ALPHABET_PANEL_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	SCX_TYPE_ALPHABET_PANEL, ScxAlphabetPanelPrivate))


GtkWidget*
scx_alphabet_panel_new (void)
{
	ScxAlphabetPanel *self = g_object_new (SCX_TYPE_ALPHABET_PANEL, NULL);
	return self;
}


static void
scx_alphabet_panel_init_gui (ScxAlphabetPanel *self)
{
	ScxAlphabetPanelPrivate *priv = self->priv;

	priv->model = gtk_list_store_new (3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

	GtkWidget *sw = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw), GTK_SHADOW_IN);
	gtk_box_pack_start (GTK_BOX (self), sw, TRUE, TRUE, 0);
	gtk_widget_show (sw);

	priv->view = gtk_tree_view_new ();
	gtk_tree_view_set_model (GTK_TREE_VIEW (priv->view), priv->model);
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (priv->view), TRUE);

	GtkTreeViewColumn *col;

	col = gtk_tree_view_column_new_with_attributes (_("Letter"), gtk_cell_renderer_text_new (), "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (priv->view), col);

	col = gtk_tree_view_column_new_with_attributes (_("Points"), gtk_cell_renderer_text_new (), "text", 1, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (priv->view), col);

	col = gtk_tree_view_column_new_with_attributes (_("Count"), gtk_cell_renderer_text_new (), "text", 2, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (priv->view), col);
	
	gtk_container_add (GTK_CONTAINER (sw), priv->view);
	gtk_widget_show (priv->view);
}


static void
scx_alphabet_panel_init (ScxAlphabetPanel *self)
{
	self->priv = SCX_ALPHABET_PANEL_GET_PRIVATE (self);
	ScxAlphabetPanelPrivate *priv = self->priv;

	priv->disposed = FALSE;

	scx_alphabet_panel_init_gui (self);
}


void
scx_alphabet_panel_set_alphabet (ScxAlphabetPanel *self, Alphabet *al)
{
	ScxAlphabetPanelPrivate *priv = self->priv;

	gtk_list_store_clear (GTK_LIST_STORE (priv->model));
	
	int i;
	for (i = 0; i < al->n_letters; i++) {
		Letter *l = &(al->letters[i]);
		GtkTreeIter iter;

		char value_buf[4]; snprintf (value_buf, 4, "%d", l->value);
		char count_buf[4]; snprintf (count_buf, 4, "%d", l->count);

		gtk_list_store_append (GTK_LIST_STORE (priv->model), &iter);
		gtk_list_store_set (GTK_LIST_STORE (priv->model), &iter,
				0, l->label,
				1, value_buf,
				2, count_buf,
				-1);
	}
}


static void
scx_alphabet_panel_dispose (GObject *object)
{
	ScxAlphabetPanel *self = (ScxAlphabetPanel*) object;
	ScxAlphabetPanelPrivate *priv = self->priv;


	/* Make sure dispose is called only once */
	if (priv->disposed) {
		return;
	}
	priv->disposed = TRUE;


	/* Chain up to the parent class */
	G_OBJECT_CLASS (scx_alphabet_panel_parent_class)->dispose (object);
}


static void
scx_alphabet_panel_finalize (GObject *object)
{
	G_OBJECT_CLASS (scx_alphabet_panel_parent_class)->finalize (object);
}

	
static void
scx_alphabet_panel_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
	ScxAlphabetPanel* self = SCX_ALPHABET_PANEL (object);
	G_UNUSED (self);

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
scx_alphabet_panel_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
	ScxAlphabetPanel* self = SCX_ALPHABET_PANEL (object);
	G_UNUSED (self);

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
scx_alphabet_panel_class_init (ScxAlphabetPanelClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->get_property = scx_alphabet_panel_get_property;
	gobject_class->set_property = scx_alphabet_panel_set_property;
	gobject_class->dispose = scx_alphabet_panel_dispose;
	gobject_class->finalize = scx_alphabet_panel_finalize;

	g_type_class_add_private (klass, sizeof (ScxAlphabetPanelPrivate));
}

