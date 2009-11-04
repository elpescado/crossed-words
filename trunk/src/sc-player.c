/*
 * Scrabble
 * --------
 *
 * Copyright (C) 2009 Przemysław Sitek
 * 
 */

#include <gtk/gtk.h>

#include "common.h"
#include "sc-player.h"

#include "marshal.h"


G_DEFINE_TYPE (ScPlayer, sc_player, G_TYPE_OBJECT)

struct _ScPlayerPrivate
{
	/* Private members go here */

	gboolean disposed;
};


#define SC_PLAYER_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	SC_TYPE_PLAYER, ScPlayerPrivate))


enum {
	YOUR_TURN,
	MOVE_DONE,

	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL];



ScPlayer*
sc_player_new (void)
{
	ScPlayer *self = g_object_new (SC_TYPE_PLAYER, NULL);
	return self;
}


static void
sc_player_init (ScPlayer *self)
{
	self->priv = SC_PLAYER_GET_PRIVATE (self);
	ScPlayerPrivate *priv = self->priv;

	priv->disposed = FALSE;
}


gboolean
sc_player_do_move (ScPlayer *player, ScMove *move)
{
	return SC_PLAYER_GET_CLASS(player)->do_move (player, move);
}

static gboolean
_sc_player_do_move (ScPlayer *player, ScMove *move)
{
	gboolean ret_val;
	g_signal_emit (player, signals[MOVE_DONE], 0, move, &ret_val);
	g_print ("_do_move = %d\n", (int) ret_val);
	return ret_val;
}


static void
sc_player_dispose (GObject *object)
{
	ScPlayer *self = (ScPlayer*) object;
	ScPlayerPrivate *priv = self->priv;


	/* Make sure dispose is called only once */
	if (priv->disposed) {
		return;
	}
	priv->disposed = TRUE;


	/* Chain up to the parent class */
	G_OBJECT_CLASS (sc_player_parent_class)->dispose (object);
}


static void
sc_player_finalize (GObject *object)
{
	G_OBJECT_CLASS (sc_player_parent_class)->finalize (object);
}

	
static void
sc_player_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
	ScPlayer* self = SC_PLAYER (object);
	G_UNUSED (self);

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
sc_player_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
	ScPlayer* self = SC_PLAYER (object);
	G_UNUSED (self);

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
sc_player_class_init (ScPlayerClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->get_property = sc_player_get_property;
	gobject_class->set_property = sc_player_set_property;
	gobject_class->dispose = sc_player_dispose;
	gobject_class->finalize = sc_player_finalize;

	klass->do_move = _sc_player_do_move;

	g_type_class_add_private (klass, sizeof (ScPlayerPrivate));

	/**
	 * ScPlayer::your-turn:
	 * 
	 * Emitted when player has to make a move
	 **/
	signals[YOUR_TURN] = g_signal_newv ("your-turn",
			G_TYPE_FROM_CLASS (klass),
			(GSignalFlags)(G_SIGNAL_RUN_LAST),
			0,
			NULL,
			NULL,
			g_cclosure_marshal_VOID__VOID,
			G_TYPE_NONE, 0,
			NULL);

	/**
	 * ScPlayer::your-turn:
	 * 
	 * Emitted when player has to make a move
	 **/
	signals[MOVE_DONE] = g_signal_new ("move-done",
			G_TYPE_FROM_CLASS (klass),
			(GSignalFlags)(G_SIGNAL_RUN_LAST),
			0,
			NULL,
			NULL,
			g_cclosure_user_marshal_BOOLEAN__POINTER,
			G_TYPE_BOOLEAN, 1,
			G_TYPE_POINTER);
		
}

