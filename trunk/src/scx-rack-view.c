/*
 * Scrabble
 * --------
 *
 * Copyright (C) 2009 Przemysław Sitek
 * 
 */

#include <gtk/gtk.h>

#include "common.h"
#include "scx-rack-view.h"
#include "scx-painter.h"


G_DEFINE_TYPE (ScxRackView, scx_rack_view, GTK_TYPE_DRAWING_AREA);

struct _ScxRackViewPrivate
{
	/* Private members go here */
	ScRackModel *rack;
	ScxPainter  *painter;

	gboolean disposed;
};


#define SCX_RACK_VIEW_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	SCX_TYPE_RACK_VIEW, ScxRackViewPrivate))

extern const int TILE_SIZE;
extern const int TILE_SPACING;

const int PADDING = 6;


GtkWidget*
scx_rack_view_new (void)
{
	ScxRackView *self = g_object_new (SCX_TYPE_RACK_VIEW, NULL);

	gint min_width = 2*PADDING + 7*(TILE_SIZE+TILE_SPACING) - TILE_SPACING;
	gint min_height = 2*PADDING + TILE_SIZE;

	gtk_widget_set_size_request (GTK_WIDGET (self), min_width, min_height);

	return GTK_WIDGET(self);
}


static void
scx_rack_view_init (ScxRackView *self)
{
	self->priv = SCX_RACK_VIEW_GET_PRIVATE (self);
	ScxRackViewPrivate *priv = self->priv;

	priv->disposed = FALSE;
}


ScRackModel *
scx_rack_view_get_rack (ScxRackView *self)
{
	ScxRackViewPrivate *priv = self->priv;

	return priv->rack ? g_object_ref (priv->rack) : NULL;
}


void
scx_rack_view_set_rack (ScxRackView *self, ScRackModel *rack)
{
	ScxRackViewPrivate *priv = self->priv;

	if (priv->rack)
		g_object_unref (priv->rack);

	priv->rack = rack ? g_object_ref (rack) : NULL;
	gtk_widget_queue_draw (GTK_WIDGET (self));
}

static gboolean
scx_rack_view_expose_event (GtkWidget          *widget,
                            GdkEventExpose     *event)
{
	ScxRackView        *self = SCX_RACK_VIEW (widget);
	ScxRackViewPrivate *priv = self->priv;

	if (priv->rack == NULL)
		return TRUE;

	if (priv->painter == NULL)
		priv->painter = scx_painter_new (GTK_WIDGET (self));

	int i;
	int n_tiles;
	LID tiles[7];
	sc_rack_model_get_tiles (priv->rack, tiles, &n_tiles);
	Alphabet *al = sc_rack_model_get_alphabet (priv->rack);

	for (i = 0; i < n_tiles; i++) {
		Letter *l = alphabet_lookup_letter (al, tiles[i]);
		scx_painter_draw_tile (priv->painter, l, PADDING + i * (TILE_SIZE + TILE_SPACING), PADDING, 0);
	}

	return TRUE;
}


static void
scx_rack_view_dispose (GObject *object)
{
	ScxRackView *self = (ScxRackView*) object;
	ScxRackViewPrivate *priv = self->priv;


	/* Make sure dispose is called only once */
	if (priv->disposed) {
		return;
	}
	priv->disposed = TRUE;


	/* Chain up to the parent class */
	G_OBJECT_CLASS (scx_rack_view_parent_class)->dispose (object);
}


static void
scx_rack_view_finalize (GObject *object)
{
	G_OBJECT_CLASS (scx_rack_view_parent_class)->finalize (object);
}

	
static void
scx_rack_view_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
	ScxRackView* self = SCX_RACK_VIEW (object);
	G_UNUSED (self);

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
scx_rack_view_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
	ScxRackView* self = SCX_RACK_VIEW (object);
	G_UNUSED (self);

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
scx_rack_view_class_init (ScxRackViewClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

	gobject_class->get_property = scx_rack_view_get_property;
	gobject_class->set_property = scx_rack_view_set_property;
	gobject_class->dispose = scx_rack_view_dispose;
	gobject_class->finalize = scx_rack_view_finalize;

	widget_class->expose_event = scx_rack_view_expose_event;

	g_type_class_add_private (klass, sizeof (ScxRackViewPrivate));
}

