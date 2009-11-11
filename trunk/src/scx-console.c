/*
 * Scrabble
 * --------
 *
 * Copyright (C) 2009 Przemysław Sitek
 * 
 */

#include <gtk/gtk.h>
#include "common.h"
#include "scx-console.h"


G_DEFINE_TYPE (ScxConsole, scx_console, GTK_TYPE_VBOX)

struct _ScxConsolePrivate
{
	/* Private members go here */
	GtkWidget     *textview;
	GtkTextBuffer *buffer;

	gboolean disposed;
};


#define SCX_CONSOLE_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	SCX_TYPE_CONSOLE, ScxConsolePrivate))

static GList *_all_consoles = NULL;

static void
_console_print (const gchar *msg)
{
	GList *node;
	for (node = _all_consoles; node; node = node->next) {
		scx_console_print (SCX_CONSOLE (node->data), msg);
	}
}



GtkWidget*
scx_console_new (void)
{
	ScxConsole *self = g_object_new (SCX_TYPE_CONSOLE, NULL);
	return GTK_WIDGET(self);
}


static void
scx_console_init_gui (ScxConsole *self)
{
	ScxConsolePrivate *priv = self->priv;

	GtkWidget *sw = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw), GTK_SHADOW_IN);
	gtk_box_pack_start (GTK_BOX (self), sw, TRUE, TRUE, 0);
	gtk_widget_show (sw);

	priv->textview = gtk_text_view_new_with_buffer (priv->buffer);
	gtk_text_view_set_editable (GTK_TEXT_VIEW (priv->textview), FALSE);
	gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (priv->textview), FALSE);
	gtk_container_add (GTK_CONTAINER(sw), priv->textview);
	gtk_widget_show (priv->textview);
}


static void
scx_console_init (ScxConsole *self)
{
	self->priv = SCX_CONSOLE_GET_PRIVATE (self);
	ScxConsolePrivate *priv = self->priv;

	priv->disposed = FALSE;

	priv->buffer = gtk_text_buffer_new (NULL);
	scx_console_init_gui (self);

	_all_consoles = g_list_append (_all_consoles, self);
}


void
scx_console_print (ScxConsole *self, const gchar *msg)
{
	ScxConsolePrivate *priv = self->priv;
	GtkTextIter iter;

	gtk_text_buffer_get_end_iter (priv->buffer, &iter);
	gtk_text_buffer_insert (priv->buffer, &iter, msg, -1);
}


static void
scx_console_dispose (GObject *object)
{
	ScxConsole *self = (ScxConsole*) object;
	ScxConsolePrivate *priv = self->priv;


	/* Make sure dispose is called only once */
	if (priv->disposed) {
		return;
	}
	priv->disposed = TRUE;


	/* Chain up to the parent class */
	G_OBJECT_CLASS (scx_console_parent_class)->dispose (object);
}


static void
scx_console_finalize (GObject *object)
{
	G_OBJECT_CLASS (scx_console_parent_class)->finalize (object);
	_all_consoles = g_list_remove (_all_consoles, object);
}

	
static void
scx_console_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
	ScxConsole* self = SCX_CONSOLE (object);
	G_UNUSED(self);

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
scx_console_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
	ScxConsole* self = SCX_CONSOLE (object);
	G_UNUSED(self);

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
scx_console_class_init (ScxConsoleClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->get_property = scx_console_get_property;
	gobject_class->set_property = scx_console_set_property;
	gobject_class->dispose = scx_console_dispose;
	gobject_class->finalize = scx_console_finalize;

	g_type_class_add_private (klass, sizeof (ScxConsolePrivate));

	g_set_print_handler (_console_print);
}

