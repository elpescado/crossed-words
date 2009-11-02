/*
 * Scrabble
 * --------
 *
 * Copyright (C) 2009 Przemysław Sitek
 * 
 */

#ifndef __SC_RACK_H__
#define __SC_RACK_H__

/* Includes here */
#include <gtk/gtkwidget.h>
#include <gtk/gtkwindow.h>

#include "alphabet.h"
#include "letter.h"

G_BEGIN_DECLS

#define SC_TYPE_RACK \
	(sc_rack_get_type ())
#define SC_RACK(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST ((obj), SC_TYPE_RACK, ScRack))
#define SC_RACK_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST ((klass), SC_TYPE_RACK, ScRackClass))
#define SC_IS_RACK(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), SC_TYPE_RACK)
#define SC_IS_RACK_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), SC_TYPE_RACK))
#define SC_RACK_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), SC_TYPE_RACK, ScRackClass))


typedef struct _ScRack		ScRack;
typedef struct _ScRackPrivate		ScRackPrivate;
typedef struct _ScRackClass		ScRackClass;

struct _ScRack{
	GObject		parent;
	ScRackPrivate	*priv;
};

struct _ScRackClass
{
	GObjectClass		parent;

	/* Signals */
};



GType
sc_rack_get_type				(void);

ScRack*
sc_rack_new				(Alphabet *al);


void
sc_rack_get_tiles               (ScRack *rack,
                                 LID    *tiles,
                                 gint   *n_tiles);


gboolean
sc_rack_has_tiles (ScRack *self,
                   LID    *tiles,
                   gint    n_tiles);


gboolean
sc_rack_add_tile                (ScRack *rack,
                                 LID     tile);


gint
sc_rack_count_tiles             (ScRack *rack);


Alphabet *
sc_rack_get_alphabet            (ScRack *rack);


void
sc_rack_remove_tile (ScRack *rack,
                     LID     tile);


void
sc_rack_remove_tiles (ScRack *rack,
                      LID    *tiles,
                      gint    n_tiles);

G_END_DECLS

#endif
