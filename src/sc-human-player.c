/*
 * Scrabble
 * --------
 *
 * Copyright (C) 2009 Przemysław Sitek
 * 
 */

#include <gtk/gtk.h>

#include "common.h"
#include "sc-human-player.h"


G_DEFINE_TYPE (ScHumanPlayer, sc_human_player, SC_TYPE_PLAYER)

struct _ScHumanPlayerPrivate
{
	/* Private members go here */

	gboolean disposed;
};


#define SC_HUMAN_PLAYER_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	SC_TYPE_HUMAN_PLAYER, ScHumanPlayerPrivate))


ScHumanPlayer*
sc_human_player_new (void)
{
	ScHumanPlayer *self = g_object_new (SC_TYPE_HUMAN_PLAYER, NULL);
	return self;
}


static void
sc_human_player_init (ScHumanPlayer *self)
{
	self->priv = SC_HUMAN_PLAYER_GET_PRIVATE (self);
	ScHumanPlayerPrivate *priv = self->priv;

	priv->disposed = FALSE;
}


static void
sc_human_player_dispose (GObject *object)
{
	ScHumanPlayer *self = (ScHumanPlayer*) object;
	ScHumanPlayerPrivate *priv = self->priv;


	/* Make sure dispose is called only once */
	if (priv->disposed) {
		return;
	}
	priv->disposed = TRUE;


	/* Chain up to the parent class */
	G_OBJECT_CLASS (sc_human_player_parent_class)->dispose (object);
}


static void
sc_human_player_finalize (GObject *object)
{
	G_OBJECT_CLASS (sc_human_player_parent_class)->finalize (object);
}

	
static void
sc_human_player_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
	ScHumanPlayer* self = SC_HUMAN_PLAYER (object);
	G_UNUSED (self);

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
sc_human_player_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
	ScHumanPlayer* self = SC_HUMAN_PLAYER (object);
	G_UNUSED (self);

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
sc_human_player_class_init (ScHumanPlayerClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->get_property = sc_human_player_get_property;
	gobject_class->set_property = sc_human_player_set_property;
	gobject_class->dispose = sc_human_player_dispose;
	gobject_class->finalize = sc_human_player_finalize;

	g_type_class_add_private (klass, sizeof (ScHumanPlayerPrivate));
}

