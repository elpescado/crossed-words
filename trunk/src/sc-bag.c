/*
 * Scrabble
 * --------
 *
 * Copyright (C) 2009 Przemysław Sitek
 * 
 */

#include <stdlib.h>

#include <gtk/gtk.h>

#include "common.h"
#include "sc-bag.h"


G_DEFINE_TYPE (ScBag, sc_bag, G_TYPE_OBJECT)

//const int SC_BAG_N_TILES = 100;
#define SC_BAG_N_TILES 100

struct _ScBagPrivate
{
	/* Private members go here */
	LID      tiles[SC_BAG_N_TILES];
	gint     n_tiles;

	gboolean disposed;
};


#define SC_BAG_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	SC_TYPE_BAG, ScBagPrivate))


ScBag*
sc_bag_new (void)
{
	ScBag *self = g_object_new (SC_TYPE_BAG, NULL);
	return self;
}


static void
sc_bag_init (ScBag *self)
{
	self->priv = SC_BAG_GET_PRIVATE (self);
	ScBagPrivate *priv = self->priv;

	priv->disposed = FALSE;
}


/**
 * Take a tile from the bag tile
 **/
LID
sc_bag_pop (ScBag *self)
{
	ScBagPrivate *priv = self->priv;

	if (priv->n_tiles > 0) {
		return priv->tiles[--priv->n_tiles];
	} else {
		g_printerr ("Bag is empty\n");
		return 0;
	}
}


/**
 * Count tiles inside bag
 **/
gint
sc_bag_n_tiles (ScBag *self)
{
	ScBagPrivate *priv = self->priv;

	return priv->n_tiles;
}


/**
 * Shuffle tiles inside bag
 **/
static void
sc_bag_shuffle (ScBag *self)
{
	ScBagPrivate *priv = self->priv;

	int i;
	for (i = 0; i < priv->n_tiles; i++) {
		int k = (int)(((double)rand() / (double)RAND_MAX) * priv->n_tiles);

		if (i != k) {
			LID tmp        = priv->tiles[i];
			priv->tiles[i] = priv->tiles[k];
			priv->tiles[k] = tmp;
		}
	}
}


/**
 * Load a bag with tiles from alphabet
 **/
void
sc_bag_load (ScBag *self, Alphabet *al)
{
	ScBagPrivate *priv = self->priv;

	/* Insert blanks */
	//priv->tiles[0] = priv->tiles[1] = BLANK;
	//int n = 2;

	int n = 0;
	
	int i;
	for (i = 0; i < al->n_letters; i++) {
		Letter *l = &(al->letters[i]);

		int j;
		for (j = 0; j < l->count; j++) {
			priv->tiles[n++] = l->index;
		}
	}

	if (n != 100) {
		g_printerr ("Expected 100 tiles, got %d\n", n);
	}

	priv->n_tiles = n;
	
	sc_bag_shuffle (self);
}


static void
sc_bag_dispose (GObject *object)
{
	ScBag *self = (ScBag*) object;
	ScBagPrivate *priv = self->priv;


	/* Make sure dispose is called only once */
	if (priv->disposed) {
		return;
	}
	priv->disposed = TRUE;


	/* Chain up to the parent class */
	G_OBJECT_CLASS (sc_bag_parent_class)->dispose (object);
}


static void
sc_bag_finalize (GObject *object)
{
	G_OBJECT_CLASS (sc_bag_parent_class)->finalize (object);
}

	
static void
sc_bag_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
	ScBag* self = SC_BAG (object);
	G_UNUSED (self);

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
sc_bag_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
	ScBag* self = SC_BAG (object);
	G_UNUSED (self);

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
sc_bag_class_init (ScBagClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->get_property = sc_bag_get_property;
	gobject_class->set_property = sc_bag_set_property;
	gobject_class->dispose = sc_bag_dispose;
	gobject_class->finalize = sc_bag_finalize;

	g_type_class_add_private (klass, sizeof (ScBagPrivate));
}

