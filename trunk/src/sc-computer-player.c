/*
 * crossed-words
 * -------------
 *
 * Copyright (C) 2009 Przemysław Sitek
 * 
 */

#include <gtk/gtk.h>
#include "sc-computer-player.h"


G_DEFINE_TYPE (ScComputerPlayer, sc_computer_player, SC_TYPE_PLAYER)

struct _ScComputerPlayerPrivate
{
	/* Private members go here */

	gboolean disposed;
};


#define SC_COMPUTER_PLAYER_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	SC_TYPE_COMPUTER_PLAYER, ScComputerPlayerPrivate))


ScComputerPlayer*
sc_computer_player_new (void)
{
	ScComputerPlayer *self = g_object_new (SC_TYPE_COMPUTER_PLAYER, NULL);
	return self;
}


static void
sc_computer_player_init (ScComputerPlayer *self)
{
	self->priv = SC_COMPUTER_PLAYER_GET_PRIVATE (self);
	ScComputerPlayerPrivate *priv = self->priv;

	priv->disposed = FALSE;
}


static void
sc_computer_player_dispose (GObject *object)
{
	ScComputerPlayer *self = (ScComputerPlayer*) object;
	ScComputerPlayerPrivate *priv = self->priv;


	/* Make sure dispose is called only once */
	if (priv->disposed) {
		return;
	}
	priv->disposed = TRUE;


	/* Chain up to the parent class */
	G_OBJECT_CLASS (sc_computer_player_parent_class)->dispose (object);
}


static void
sc_computer_player_finalize (GObject *object)
{
	G_OBJECT_CLASS (sc_computer_player_parent_class)->finalize (object);
}

	
static void
sc_computer_player_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
	ScComputerPlayer* self = SC_COMPUTER_PLAYER (object);
	ScComputerPlayerPrivate* priv = self->priv;

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
sc_computer_player_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
	ScComputerPlayer* self = SC_COMPUTER_PLAYER (object);
	ScComputerPlayerPrivate* priv = self->priv;

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
sc_computer_player_class_init (ScComputerPlayerClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->get_property = sc_computer_player_get_property;
	gobject_class->set_property = sc_computer_player_set_property;
	gobject_class->dispose = sc_computer_player_dispose;
	gobject_class->finalize = sc_computer_player_finalize;

	g_type_class_add_private (klass, sizeof (ScComputerPlayerPrivate));
}

