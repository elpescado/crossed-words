/*
 * Scrabble
 * --------
 *
 * Copyright (C) 2009 Przemysław Sitek
 * 
 */

#include <gtk/gtk.h>

#include "common.h"
#include "sc-dumb-player.h"


G_DEFINE_TYPE (ScDumbPlayer, sc_dumb_player, SC_TYPE_PLAYER)

struct _ScDumbPlayerPrivate
{
	/* Private members go here */

	gboolean disposed;
};


#define SC_DUMB_PLAYER_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	SC_TYPE_DUMB_PLAYER, ScDumbPlayerPrivate))


ScDumbPlayer*
sc_dumb_player_new (void)
{
	ScDumbPlayer *self = g_object_new (SC_TYPE_DUMB_PLAYER, NULL);
	return self;
}


static void
sc_dumb_player_your_turn (ScDumbPlayer *self)
{
	ScMove move;
	move.type = SC_MOVE_TYPE_PASS;
	sc_player_do_move (SC_PLAYER (self), &move);
}


static void
sc_dumb_player_init (ScDumbPlayer *self)
{
	self->priv = SC_DUMB_PLAYER_GET_PRIVATE (self);
	ScDumbPlayerPrivate *priv = self->priv;

	g_signal_connect (self, "your-turn",
	                  G_CALLBACK (sc_dumb_player_your_turn), self);

	priv->disposed = FALSE;
}


static void
sc_dumb_player_dispose (GObject *object)
{
	ScDumbPlayer *self = (ScDumbPlayer*) object;
	ScDumbPlayerPrivate *priv = self->priv;


	/* Make sure dispose is called only once */
	if (priv->disposed) {
		return;
	}
	priv->disposed = TRUE;


	/* Chain up to the parent class */
	G_OBJECT_CLASS (sc_dumb_player_parent_class)->dispose (object);
}


static void
sc_dumb_player_finalize (GObject *object)
{
	G_OBJECT_CLASS (sc_dumb_player_parent_class)->finalize (object);
}

	
static void
sc_dumb_player_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
	ScDumbPlayer* self = SC_DUMB_PLAYER (object);
	G_UNUSED (self);

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
sc_dumb_player_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
	ScDumbPlayer* self = SC_DUMB_PLAYER (object);
	G_UNUSED (self);

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
sc_dumb_player_class_init (ScDumbPlayerClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->get_property = sc_dumb_player_get_property;
	gobject_class->set_property = sc_dumb_player_set_property;
	gobject_class->dispose = sc_dumb_player_dispose;
	gobject_class->finalize = sc_dumb_player_finalize;

	g_type_class_add_private (klass, sizeof (ScDumbPlayerPrivate));
}

