/*
 * Scrabble
 * --------
 *
 * Copyright (C) 2009 Przemysław Sitek
 * 
 */
#include <string.h>

#include <gtk/gtk.h>

#include "common.h"
#include "letter.h"
#include "sc-rack-model.h"


G_DEFINE_TYPE (ScRackModel, sc_rack_model, G_TYPE_OBJECT)

struct _ScRackModelPrivate
{
	/* Private members go here */
	LID      tiles[7];
	gint     n_tiles;

	Alphabet *al;

	gboolean disposed;
};


#define SC_RACK_MODEL_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	SC_TYPE_RACK, ScRackModelPrivate))


ScRackModel*
sc_rack_model_new (Alphabet *al)
{
	ScRackModel *self = g_object_new (SC_TYPE_RACK, NULL);
	ScRackModelPrivate *priv = self->priv;
	priv->al = al;

	return self;
}


static void
sc_rack_model_init (ScRackModel *self)
{
	self->priv = SC_RACK_MODEL_GET_PRIVATE (self);
	ScRackModelPrivate *priv = self->priv;

	priv->disposed = FALSE;
}


void
sc_rack_model_get_tiles               (ScRackModel *rack,
                                 LID    *tiles,
                                 gint   *n_tiles)
{
	ScRackModelPrivate *priv = rack->priv;

	if (n_tiles)
		*n_tiles = priv->n_tiles;

	memcpy (tiles, priv->tiles, priv->n_tiles * sizeof(LID));
}


gboolean
sc_rack_model_has_tiles (ScRackModel *self,
                   LID    *tiles,
                   gint    n_tiles)
{
	ScRackModelPrivate *priv = self->priv;
	Alphabet *al = priv->al;
	gboolean ok = TRUE;
	int i;

	gint *tile_counts = g_new0 (gint, al->n_letters+2);

	for (i = 0; i < n_tiles; i++) {
		//g_print (" -> You need %d\n", tiles[i]);
		tile_counts[tiles[i]]++;
	}

	for (i = 0; i < priv->n_tiles; i++) {
		LID lid = priv->tiles[i];
		if (tile_counts[lid] > 0) {
			//g_print (" -> You have %d\n", priv->tiles[i]);
			tile_counts[lid]--;
		}
	}

	for (i = 0; i < al->n_letters; i++) {
		if (tile_counts[i] != 0) {
			Letter *l = alphabet_lookup_letter (priv->al, i);
			g_print ("You don't have %d %s\n", tile_counts[i], l->label);
			ok = FALSE;
		}
	}

	g_free (tile_counts);

	return ok;
}


gboolean
sc_rack_model_add_tile                (ScRackModel *rack,
                                 LID     tile)
{
	ScRackModelPrivate *priv = rack->priv;

	if (priv->n_tiles >= 7) {
		return FALSE;
	} else {
		priv->tiles[priv->n_tiles++] = tile;
		return TRUE;
	}
}


void
sc_rack_model_remove_tile (ScRackModel *rack,
                     LID     tile)
{
	ScRackModelPrivate *priv = rack->priv;
	int found_i = -1;
	int i;

	for (i = 0; i < priv->n_tiles; i++) {
		if (priv->tiles[i] == tile) {
			found_i = i;
			break;
		}
	}

	if (found_i > -1) {
		priv->n_tiles--;
		int tmp                    = priv->tiles[priv->n_tiles];
		priv->tiles[priv->n_tiles] = priv->tiles[found_i];
		priv->tiles[found_i]       = tmp;
	}
}


/**
 * Remove tiles from player's rack
 **/
void
sc_rack_model_remove_tiles (ScRackModel *rack,
                      LID    *tiles,
                      gint    n_tiles)
{
	int i;
	for (i = 0; i < n_tiles; i++) {
		sc_rack_model_remove_tile (rack, tiles[i]);
	}
}


gint
sc_rack_model_count_tiles             (ScRackModel *rack)
{
	ScRackModelPrivate *priv = rack->priv;

	return priv->n_tiles;
}


Alphabet *
sc_rack_model_get_alphabet            (ScRackModel *rack)
{
	ScRackModelPrivate *priv = rack->priv;
	
	return priv->al;
}


static void
sc_rack_model_dispose (GObject *object)
{
	ScRackModel *self = (ScRackModel*) object;
	ScRackModelPrivate *priv = self->priv;


	/* Make sure dispose is called only once */
	if (priv->disposed) {
		return;
	}
	priv->disposed = TRUE;


	/* Chain up to the parent class */
	G_OBJECT_CLASS (sc_rack_model_parent_class)->dispose (object);
}


static void
sc_rack_model_finalize (GObject *object)
{
	G_OBJECT_CLASS (sc_rack_model_parent_class)->finalize (object);
}

	
static void
sc_rack_model_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
	ScRackModel* self = SC_RACK_MODEL (object);
	G_UNUSED (self);

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
sc_rack_model_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
	ScRackModel* self = SC_RACK_MODEL (object);
	G_UNUSED (self);

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
sc_rack_model_class_init (ScRackModelClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->get_property = sc_rack_model_get_property;
	gobject_class->set_property = sc_rack_model_set_property;
	gobject_class->dispose = sc_rack_model_dispose;
	gobject_class->finalize = sc_rack_model_finalize;

	g_type_class_add_private (klass, sizeof (ScRackModelPrivate));
}

