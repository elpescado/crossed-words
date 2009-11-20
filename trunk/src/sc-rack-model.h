/*
 * Scrabble
 * --------
 *
 * Copyright (C) 2009 Przemysław Sitek
 * 
 */

#ifndef __SC_RACK_MODEL_H__
#define __SC_RACK_MODEL_H__

/* Includes here */
#include <gtk/gtkwidget.h>
#include <gtk/gtkwindow.h>

#include "alphabet.h"
#include "letter.h"

G_BEGIN_DECLS

#define SC_TYPE_RACK \
	(sc_rack_model_get_type ())
#define SC_RACK_MODEL(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST ((obj), SC_TYPE_RACK, ScRackModel))
#define SC_RACK_MODEL_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST ((klass), SC_TYPE_RACK, ScRackModelClass))
#define SC_IS_RACK(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), SC_TYPE_RACK)
#define SC_IS_RACK_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), SC_TYPE_RACK))
#define SC_RACK_MODEL_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), SC_TYPE_RACK, ScRackModelClass))


typedef struct _ScRackModel		ScRackModel;
typedef struct _ScRackModelPrivate		ScRackModelPrivate;
typedef struct _ScRackModelClass		ScRackModelClass;

struct _ScRackModel{
	GObject		parent;
	ScRackModelPrivate	*priv;
};

struct _ScRackModelClass
{
	GObjectClass		parent;

	/* Signals */
};



GType
sc_rack_model_get_type				(void);

ScRackModel*
sc_rack_model_new				(Alphabet *al);


void
sc_rack_model_get_tiles               (ScRackModel *rack,
                                 LID    *tiles,
                                 gint   *n_tiles);


gboolean
sc_rack_model_has_tiles (ScRackModel *self,
                   LID    *tiles,
                   gint    n_tiles);


gboolean
sc_rack_model_add_tile                (ScRackModel *rack,
                                 LID     tile);


gint
sc_rack_model_count_tiles             (ScRackModel *rack);


Alphabet *
sc_rack_model_get_alphabet            (ScRackModel *rack);


void
sc_rack_model_remove_tile (ScRackModel *rack,
                     LID     tile);


void
sc_rack_model_remove_tiles (ScRackModel *rack,
                      LID    *tiles,
                      gint    n_tiles);

G_END_DECLS

#endif
