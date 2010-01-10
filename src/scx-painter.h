/*
 * Scrabble
 * --------
 *
 * Copyright (C) 2009 Przemysław Sitek
 * 
 */

#ifndef __SCX_PAINTER_H__
#define __SCX_PAINTER_H__

/* Includes here */
#include <gtk/gtkwidget.h>
#include <gtk/gtkwindow.h>

#include "letter.h"


G_BEGIN_DECLS

#define SCX_TYPE_PAINTER \
	(scx_painter_get_type ())
#define SCX_PAINTER(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST ((obj), SCX_TYPE_PAINTER, ScxPainter))
#define SCX_PAINTER_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST ((klass), SCX_TYPE_PAINTER, ScxPainterClass))
#define SCX_IS_PAINTER(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), SCX_TYPE_PAINTER)
#define SCX_IS_PAINTER_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), SCX_TYPE_PAINTER))
#define SCX_PAINTER_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), SCX_TYPE_PAINTER, ScxPainterClass))


typedef struct _ScxPainter		ScxPainter;
typedef struct _ScxPainterPrivate		ScxPainterPrivate;
typedef struct _ScxPainterClass		ScxPainterClass;

struct _ScxPainter{
	GObject		parent;
	ScxPainterPrivate	*priv;
};

struct _ScxPainterClass
{
	GObjectClass		parent;

	/* Signals */
};



GType
scx_painter_get_type				(void);

ScxPainter*
scx_painter_new				(GtkWidget *widget);


gint
scx_painter_get_tile_size (ScxPainter *self);


void
scx_painter_set_tile_size (ScxPainter *self,
                           gint        tile_size);


gint
scx_painter_get_tile_spacing (ScxPainter *self);


void
scx_painter_set_tile_spacing (ScxPainter *self,
                              gint        tile_spacing);


void
scx_painter_draw_tile (ScxPainter  *self,
                       Letter      *l,
                       gint         x,
                       gint         y,
                       gint         style_id,
                       gboolean     blank);




G_END_DECLS

#endif
