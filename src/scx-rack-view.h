/*
 * Scrabble
 * --------
 *
 * Copyright (C) 2009 Przemysław Sitek
 * 
 */

#ifndef __SCX_RACK_VIEW_H__
#define __SCX_RACK_VIEW_H__

/* Includes here */
#include <gtk/gtkwidget.h>
#include <gtk/gtkdrawingarea.h>

#include "sc-rack.h"

G_BEGIN_DECLS

#define SCX_TYPE_RACK_VIEW \
	(scx_rack_view_get_type ())
#define SCX_RACK_VIEW(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST ((obj), SCX_TYPE_RACK_VIEW, ScxRackView))
#define SCX_RACK_VIEW_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST ((klass), SCX_TYPE_RACK_VIEW, ScxRackViewClass))
#define SCX_IS_RACK_VIEW(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), SCX_TYPE_RACK_VIEW)
#define SCX_IS_RACK_VIEW_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), SCX_TYPE_RACK_VIEW))
#define SCX_RACK_VIEW_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), SCX_TYPE_RACK_VIEW, ScxRackViewClass))


typedef struct _ScxRackView		ScxRackView;
typedef struct _ScxRackViewPrivate		ScxRackViewPrivate;
typedef struct _ScxRackViewClass		ScxRackViewClass;

struct _ScxRackView{
	GtkDrawingArea       parent;
	ScxRackViewPrivate	*priv;
};

struct _ScxRackViewClass
{
	GtkDrawingAreaClass		parent;

	/* Signals */
};



GType
scx_rack_view_get_type				(void);

GtkWidget*
scx_rack_view_new				(void);


ScRack *
scx_rack_view_get_rack (ScxRackView *self);


void
scx_rack_view_set_rack (ScxRackView *self, ScRack *rack);


G_END_DECLS

#endif
