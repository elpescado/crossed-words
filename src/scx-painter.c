/*
 * Scrabble
 * --------
 *
 * Copyright (C) 2009 Przemysław Sitek
 * 
 */

#include <gtk/gtk.h>

#include "common.h"
#include "scx-painter.h"


G_DEFINE_TYPE (ScxPainter, scx_painter, G_TYPE_OBJECT)

struct _ScxPainterPrivate
{
	/* Private members go here */
	GtkWidget *widget;
	GdkGC     *tile_gc;

	gboolean disposed;
};


#define SCX_PAINTER_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	SCX_TYPE_PAINTER, ScxPainterPrivate))


extern const int TILE_SIZE;
extern const int TILE_SPACING;


ScxPainter*
scx_painter_new (GtkWidget *widget)
{
	ScxPainter *self = g_object_new (SCX_TYPE_PAINTER, NULL);
	ScxPainterPrivate *priv = self->priv;

	priv->widget = widget;
	return self;
}


static void
scx_painter_init (ScxPainter *self)
{
	self->priv = SCX_PAINTER_GET_PRIVATE (self);
	ScxPainterPrivate *priv = self->priv;

	priv->disposed = FALSE;
}

static void
set_color (GdkGC *gc, const gchar *color_name)
{
	GdkColor color;
	
	gdk_color_parse (color_name, &color);
	gdk_gc_set_rgb_fg_color (gc, &color);
}


/**
 * Draws a tile
 **/
void
scx_painter_draw_tile (ScxPainter  *self,
                       Letter      *l,
                       gint         x,
                       gint         y,
                       gint         style_id)
{
	ScxPainterPrivate *priv = self->priv;

	const gchar *border = style_id == 0 ? "#8f5902" : "#4e9a06";
	const gchar *bg     = style_id == 0 ? "#e9b96e" : "#8ae234";

	priv->tile_gc = gdk_gc_new (gtk_widget_get_window (GTK_WIDGET (priv->widget)));

	PangoLayout *layout = gtk_widget_create_pango_layout (priv->widget, "");
	PangoFontDescription *font = pango_font_description_from_string ("Euromode 32px");
	pango_layout_set_font_description (layout, font);
	pango_layout_set_width (layout, (TILE_SIZE - 2) * PANGO_SCALE);
	pango_layout_set_alignment (layout, PANGO_ALIGN_CENTER);

	PangoLayout *layout2 = gtk_widget_create_pango_layout (priv->widget, "");
	pango_layout_set_width (layout2, (TILE_SIZE - 2) * PANGO_SCALE);
	pango_layout_set_alignment (layout2, PANGO_ALIGN_RIGHT);

	set_color (priv->tile_gc, border);
	gdk_draw_rectangle (gtk_widget_get_window (priv->widget),
						priv->tile_gc,
						FALSE, x, y, TILE_SIZE, TILE_SIZE);

	
	set_color (priv->tile_gc, bg);
	gdk_draw_rectangle (gtk_widget_get_window (priv->widget),
						priv->tile_gc,
						TRUE, x+1, y+1, TILE_SIZE-1, TILE_SIZE-1);

	set_color (priv->tile_gc, "#000000");
	pango_layout_set_text (layout, l->label, -1);
	gdk_draw_layout (gtk_widget_get_window (priv->widget),
	                 priv->tile_gc, x+1, 4 + y+1,
					 layout);

	gchar points_text[4];
	snprintf (points_text, 4, "%d", l->value);
	pango_layout_set_text (layout2, points_text, -1);
	gdk_draw_layout (gtk_widget_get_window (priv->widget),
	                 priv->tile_gc, x+1, 32 + y+1,
					 layout2);

	gdk_draw_line (gtk_widget_get_window (priv->widget),
	               priv->tile_gc, x + 1, y + TILE_SIZE + 1, x + TILE_SIZE + 1, y + TILE_SIZE + 1);
	gdk_draw_line (gtk_widget_get_window (priv->widget),
	               priv->tile_gc, x + TILE_SIZE + 1, y + 1, x + TILE_SIZE + 1, y + TILE_SIZE + 1);

	g_object_unref (layout);
	g_object_unref (layout2);
}



static void
scx_painter_dispose (GObject *object)
{
	ScxPainter *self = (ScxPainter*) object;
	ScxPainterPrivate *priv = self->priv;


	/* Make sure dispose is called only once */
	if (priv->disposed) {
		return;
	}
	priv->disposed = TRUE;


	/* Chain up to the parent class */
	G_OBJECT_CLASS (scx_painter_parent_class)->dispose (object);
}


static void
scx_painter_finalize (GObject *object)
{
	G_OBJECT_CLASS (scx_painter_parent_class)->finalize (object);
}

	
static void
scx_painter_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
	ScxPainter* self = SCX_PAINTER (object);
	G_UNUSED (self);

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
scx_painter_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
	ScxPainter* self = SCX_PAINTER (object);
	G_UNUSED (self);

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
scx_painter_class_init (ScxPainterClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->get_property = scx_painter_get_property;
	gobject_class->set_property = scx_painter_set_property;
	gobject_class->dispose = scx_painter_dispose;
	gobject_class->finalize = scx_painter_finalize;

	g_type_class_add_private (klass, sizeof (ScxPainterPrivate));
}
