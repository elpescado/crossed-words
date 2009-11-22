/*
 * Scrabble
 * --------
 *
 * Copyright (C) 2009 Przemysław Sitek
 * 
 */

#ifndef __SC_PLAYER_H__
#define __SC_PLAYER_H__

/* Includes here */
#include <gtk/gtkwidget.h>
#include <gtk/gtkwindow.h>

#include "sc-move.h"
#include "sc-board.h"
#include "sc-rack.h"

G_BEGIN_DECLS

#define SC_TYPE_PLAYER \
	(sc_player_get_type ())
#define SC_PLAYER(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST ((obj), SC_TYPE_PLAYER, ScPlayer))
#define SC_PLAYER_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST ((klass), SC_TYPE_PLAYER, ScPlayerClass))
#define SC_IS_PLAYER(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), SC_TYPE_PLAYER)
#define SC_IS_PLAYER_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), SC_TYPE_PLAYER))
#define SC_PLAYER_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), SC_TYPE_PLAYER, ScPlayerClass))


typedef struct _ScPlayer		ScPlayer;
typedef struct _ScPlayerPrivate		ScPlayerPrivate;
typedef struct _ScPlayerClass		ScPlayerClass;

struct _ScPlayer{
	GObject		parent;
	gpointer   *game;
	ScPlayerPrivate	*priv;
};

struct _ScPlayerClass
{
	GObjectClass		parent;

	/* Virtual methods */
	gboolean (*do_move) (ScPlayer *, ScMove *move);

	/* Signals */
};


GType
sc_player_get_type				(void);


ScPlayer*
sc_player_new				(void);


gboolean
sc_player_do_move (ScPlayer *plaver, ScMove *move);


/*
ScGame *
sc_player_get_game (ScPlayer *player);
*/

ScBoard *
sc_player_get_board (ScPlayer *player);

void
sc_player_get_rack (ScPlayer *player, ScRack *rack);

G_END_DECLS

#endif
