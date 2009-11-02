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
#include "scx-move-entry.h"

#define PIXMAP_PATH "../pixmaps/"


G_DEFINE_TYPE (ScxMoveEntry, scx_move_entry, GTK_TYPE_VBOX)

struct _ScxMoveEntryPrivate
{
	/* Private members go here */
	GtkWidget *entry;

	GtkWidget *horizontal;
	GtkWidget *vertical;

	GtkWidget *pass_button;
	GtkWidget *exchange_button;
	GtkWidget *ok_button;

	gboolean disposed;
};


enum {
	CHANGED,
	ACTIVATED,

	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL];


#define SCX_MOVE_ENTRY_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	SCX_TYPE_MOVE_ENTRY, ScxMoveEntryPrivate))


/* Callbacks */
static void
scx_move_entry_orientation_toggled (GtkButton    *button,
                                    ScxMoveEntry *self);

static void
scx_move_entry_text_changed (GtkEditable  *editable,
                             ScxMoveEntry *self);

static void
scx_move_entry_button_clicked (GtkButton    *button,
                               ScxMoveEntry *self);




GtkWidget*
scx_move_entry_new (void)
{
	ScxMoveEntry *self = g_object_new (SCX_TYPE_MOVE_ENTRY, NULL);
	return self;
}


static void
scx_move_entry_init_gui (ScxMoveEntry *self)
{
	ScxMoveEntryPrivate *priv = self->priv;

	gtk_box_set_spacing (GTK_BOX (self), 6);

	priv->entry = gtk_entry_new ();
	g_signal_connect (priv->entry, "changed", G_CALLBACK (scx_move_entry_text_changed), self);
	gtk_box_pack_start (GTK_BOX (self), priv->entry, FALSE, FALSE, 0);
	gtk_widget_show (priv->entry);

	GtkWidget *hbox = gtk_hbox_new (FALSE, 6);
	gtk_box_pack_start (GTK_BOX (self), hbox, FALSE, FALSE, 0);
	
	priv->horizontal = gtk_toggle_button_new ();
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (priv->horizontal), TRUE);
	gtk_box_pack_start (GTK_BOX (hbox), priv->horizontal, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER (priv->horizontal),
	                   gtk_image_new_from_file (PIXMAP_PATH "horizontal.png"));
	g_signal_connect (priv->horizontal, "toggled",
	                  G_CALLBACK (scx_move_entry_orientation_toggled), self);
	gtk_widget_show_all (priv->horizontal);

	priv->vertical = gtk_toggle_button_new ();
	gtk_box_pack_start (GTK_BOX (hbox), priv->vertical, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER (priv->vertical),
	                   gtk_image_new_from_file (PIXMAP_PATH "vertical.png"));
	g_signal_connect (priv->vertical, "toggled",
	                  G_CALLBACK (scx_move_entry_orientation_toggled), self);
	gtk_widget_show_all (priv->vertical);

	GtkWidget *bbox = gtk_hbutton_box_new ();
	gtk_button_box_set_layout (GTK_BUTTON_BOX (bbox), GTK_BUTTONBOX_END);
	gtk_box_set_spacing (GTK_BOX (bbox), 6);
	gtk_box_pack_end (GTK_BOX (hbox), bbox, TRUE, TRUE, 0);

	priv->pass_button = gtk_button_new_with_label (_("Pass"));
	g_signal_connect (priv->pass_button, "clicked",
	                  G_CALLBACK (scx_move_entry_button_clicked), self);
	gtk_container_add (GTK_CONTAINER (bbox), priv->pass_button);
	gtk_widget_show (priv->pass_button);

	priv->exchange_button = gtk_button_new_with_label (_("Exchange"));
	g_signal_connect (priv->exchange_button, "clicked",
	                  G_CALLBACK (scx_move_entry_button_clicked), self);
	gtk_container_add (GTK_CONTAINER (bbox), priv->exchange_button);
	gtk_widget_show (priv->exchange_button);

	priv->ok_button = gtk_button_new_with_label (_("Submit"));
	g_signal_connect (priv->ok_button, "clicked",
	                  G_CALLBACK (scx_move_entry_button_clicked), self);
	gtk_container_add (GTK_CONTAINER (bbox), priv->ok_button);
	gtk_widget_show (priv->ok_button);

	gtk_widget_show (bbox);

	gtk_widget_show (hbox);
}


static void
scx_move_entry_orientation_toggled (GtkButton    *button,
                                    ScxMoveEntry *self)
{
	ScxMoveEntryPrivate *priv = self->priv;

	GtkWidget *other = ((void*)button == (void*)(priv->horizontal))
		? priv->vertical
		: priv->horizontal;

	gboolean this_active = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button));
	gboolean other_active = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (other));

	if (this_active == other_active) {
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (other), !this_active);
		g_signal_emit (self, signals[CHANGED], 0);
	}
}


static void
scx_move_entry_text_changed (GtkEditable  *editable,
                             ScxMoveEntry *self)
{
	g_signal_emit (self, signals[CHANGED], 0);
}


static void
scx_move_entry_button_clicked (GtkButton    *button,
                               ScxMoveEntry *self)
{
	ScxMoveEntryPrivate *priv = self->priv;

	ScMoveType type;

	if (button == priv->ok_button) {
		type = SC_MOVE_TYPE_MOVE;
		g_print ("OK\n");
	} else if (button == priv->exchange_button) {
		type = SC_MOVE_TYPE_EXCHANGE;
		g_print ("Exchange\n");
	} else if (button == priv->pass_button) {
		type = SC_MOVE_TYPE_PASS;
		g_print ("Pass\n");
	}

	g_signal_emit (self, signals[ACTIVATED], 0, type);
}


static void
scx_move_entry_init (ScxMoveEntry *self)
{
	self->priv = SCX_MOVE_ENTRY_GET_PRIVATE (self);
	ScxMoveEntryPrivate *priv = self->priv;

	priv->disposed = FALSE;

	scx_move_entry_init_gui (self);
}


ScOrientation
scx_move_entry_get_orientation (ScxMoveEntry *self)
{
	ScxMoveEntryPrivate *priv = self->priv;

	return gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (priv->vertical))
		? SC_VERTICAL
		: SC_HORIZONTAL;
}


const gchar *
scx_move_entry_get_text (ScxMoveEntry *self)
{
	ScxMoveEntryPrivate *priv = self->priv;

	return gtk_entry_get_text (GTK_ENTRY (priv->entry));
}


static void
scx_move_entry_dispose (GObject *object)
{
	ScxMoveEntry *self = (ScxMoveEntry*) object;
	ScxMoveEntryPrivate *priv = self->priv;


	/* Make sure dispose is called only once */
	if (priv->disposed) {
		return;
	}
	priv->disposed = TRUE;


	/* Chain up to the parent class */
	G_OBJECT_CLASS (scx_move_entry_parent_class)->dispose (object);
}


static void
scx_move_entry_finalize (GObject *object)
{
	G_OBJECT_CLASS (scx_move_entry_parent_class)->finalize (object);
}

	
static void
scx_move_entry_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
	ScxMoveEntry* self = SCX_MOVE_ENTRY (object);
	G_UNUSED (self);

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
scx_move_entry_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
	ScxMoveEntry* self = SCX_MOVE_ENTRY (object);
	G_UNUSED (self);

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
scx_move_entry_class_init (ScxMoveEntryClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->get_property = scx_move_entry_get_property;
	gobject_class->set_property = scx_move_entry_set_property;
	gobject_class->dispose = scx_move_entry_dispose;
	gobject_class->finalize = scx_move_entry_finalize;

	g_type_class_add_private (klass, sizeof (ScxMoveEntryPrivate));

	signals[CHANGED] = g_signal_new ("changed",
			G_TYPE_FROM_CLASS (klass),
			(GSignalFlags)(G_SIGNAL_RUN_LAST),
			NULL,
			NULL,
			NULL,
			g_cclosure_marshal_VOID__VOID,
			G_TYPE_NONE, 0,
			NULL);

	signals[ACTIVATED] = g_signal_new ("activated",
			G_TYPE_FROM_CLASS (klass),
			(GSignalFlags)(G_SIGNAL_RUN_LAST),
			NULL,
			NULL,
			NULL,
			g_cclosure_marshal_VOID__INT,
			G_TYPE_NONE, 1,
			G_TYPE_INT);
		
}

