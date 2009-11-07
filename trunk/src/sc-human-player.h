/*
 * Scrabble
 * --------
 *
 * Copyright (C) 2009 Przemysław Sitek
 * 
 */

#ifndef __SC_HUMAN_PLAYER_H__
#define __SC_HUMAN_PLAYER_H__

/* Includes here */
#include <gtk/gtkwidget.h>
#include <gtk/gtkwindow.h>

#include "sc-player.h"


G_BEGIN_DECLS

#define SC_TYPE_HUMAN_PLAYER \
	(sc_human_player_get_type ())
#define SC_HUMAN_PLAYER(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST ((obj), SC_TYPE_HUMAN_PLAYER, ScHumanPlayer))
#define SC_HUMAN_PLAYER_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST ((klass), SC_TYPE_HUMAN_PLAYER, ScHumanPlayerClass))
#define SC_IS_HUMAN_PLAYER(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), SC_TYPE_HUMAN_PLAYER))
#define SC_IS_HUMAN_PLAYER_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), SC_TYPE_HUMAN_PLAYER))
#define SC_HUMAN_PLAYER_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), SC_TYPE_HUMAN_PLAYER, ScHumanPlayerClass))


typedef struct _ScHumanPlayer		ScHumanPlayer;
typedef struct _ScHumanPlayerPrivate		ScHumanPlayerPrivate;
typedef struct _ScHumanPlayerClass		ScHumanPlayerClass;

struct _ScHumanPlayer{
	ScPlayer		parent;
	ScHumanPlayerPrivate	*priv;
};

struct _ScHumanPlayerClass
{
	ScPlayerClass		parent;

	/* Signals */
};



GType
sc_human_player_get_type				(void);

ScHumanPlayer*
sc_human_player_new				(void);


G_END_DECLS

#endif
