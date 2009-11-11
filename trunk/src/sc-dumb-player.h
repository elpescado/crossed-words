/*
 * Scrabble
 * --------
 *
 * Copyright (C) 2009 Przemysław Sitek
 * 
 */

#ifndef __SC_DUMB_PLAYER_H__
#define __SC_DUMB_PLAYER_H__

/* Includes here */
#include "sc-player.h"

G_BEGIN_DECLS

#define SC_TYPE_DUMB_PLAYER \
	(sc_dumb_player_get_type ())
#define SC_DUMB_PLAYER(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST ((obj), SC_TYPE_DUMB_PLAYER, ScDumbPlayer))
#define SC_DUMB_PLAYER_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST ((klass), SC_TYPE_DUMB_PLAYER, ScDumbPlayerClass))
#define SC_IS_DUMB_PLAYER(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), SC_TYPE_DUMB_PLAYER)
#define SC_IS_DUMB_PLAYER_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), SC_TYPE_DUMB_PLAYER))
#define SC_DUMB_PLAYER_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), SC_TYPE_DUMB_PLAYER, ScDumbPlayerClass))


typedef struct _ScDumbPlayer		ScDumbPlayer;
typedef struct _ScDumbPlayerPrivate		ScDumbPlayerPrivate;
typedef struct _ScDumbPlayerClass		ScDumbPlayerClass;

struct _ScDumbPlayer{
	ScPlayer		parent;
	ScDumbPlayerPrivate	*priv;
};

struct _ScDumbPlayerClass
{
	ScPlayerClass		parent;

	/* Signals */
};



GType
sc_dumb_player_get_type				(void);

ScDumbPlayer*
sc_dumb_player_new				(void);


G_END_DECLS

#endif
