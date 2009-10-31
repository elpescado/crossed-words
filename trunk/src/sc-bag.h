/*
 * Scrabble
 * --------
 *
 * Copyright (C) 2009 Przemysław Sitek
 * 
 */

#ifndef __SC_BAG_H__
#define __SC_BAG_H__

/* Includes here */
#include <gtk/gtkwidget.h>
#include <gtk/gtkwindow.h>

#include "alphabet.h"


G_BEGIN_DECLS

#define SC_TYPE_BAG \
	(sc_bag_get_type ())
#define SC_BAG(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST ((obj), SC_TYPE_BAG, ScBag))
#define SC_BAG_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST ((klass), SC_TYPE_BAG, ScBagClass))
#define SC_IS_BAG(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), SC_TYPE_BAG)
#define SC_IS_BAG_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), SC_TYPE_BAG))
#define SC_BAG_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), SC_TYPE_BAG, ScBagClass))


typedef struct _ScBag		ScBag;
typedef struct _ScBagPrivate		ScBagPrivate;
typedef struct _ScBagClass		ScBagClass;

struct _ScBag{
	GObject		parent;
	ScBagPrivate	*priv;
};

struct _ScBagClass
{
	GObjectClass		parent;

	/* Signals */
};



GType
sc_bag_get_type				(void);

ScBag*
sc_bag_new				(void);


LID
sc_bag_pop (ScBag *self);


gint
sc_bag_n_tiles (ScBag *self);


void
sc_bag_load (ScBag *bag, Alphabet *al);



G_END_DECLS

#endif
