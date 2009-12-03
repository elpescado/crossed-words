/*
 * crossed-words
 * -------------
 *
 * Copyright (C) 2009 Przemysław Sitek
 * 
 */

#ifndef __SC_COMPUTER_PLAYER_H__
#define __SC_COMPUTER_PLAYER_H__

/* Includes here */
#include <gtk/gtkwidget.h>
#include <gtk/gtkwindow.h>

#include "sc-player.h"
#include "sc-move.h"


G_BEGIN_DECLS

#define SC_TYPE_COMPUTER_PLAYER \
	(sc_computer_player_get_type ())
#define SC_COMPUTER_PLAYER(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST ((obj), SC_TYPE_COMPUTER_PLAYER, ScComputerPlayer))
#define SC_COMPUTER_PLAYER_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST ((klass), SC_TYPE_COMPUTER_PLAYER, ScComputerPlayerClass))
#define SC_IS_COMPUTER_PLAYER(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), SC_TYPE_COMPUTER_PLAYER))
#define SC_IS_COMPUTER_PLAYER_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), SC_TYPE_COMPUTER_PLAYER))
#define SC_COMPUTER_PLAYER_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), SC_TYPE_COMPUTER_PLAYER, ScComputerPlayerClass))


typedef struct _ScComputerPlayer		ScComputerPlayer;
typedef struct _ScComputerPlayerPrivate		ScComputerPlayerPrivate;
typedef struct _ScComputerPlayerClass		ScComputerPlayerClass;

struct _ScComputerPlayer{
	ScPlayer		parent;
	ScComputerPlayerPrivate	*priv;
};

struct _ScComputerPlayerClass
{
	ScPlayerClass		parent;

	/* Signals */

	/* Virtual methods */
	ScMove * (*analyze_moves) (struct _ScComputerPlayer *self);
};

typedef struct {
	ScMove move;
	gint   rating;
} _MoveProposal;



GType
sc_computer_player_get_type				(void);

ScComputerPlayer*
sc_computer_player_new				(void);


void
sc_computer_player_save_move (ScComputerPlayer *self, ScMove *move, gint rating);


void
sc_computer_player_clear_moves (ScComputerPlayer *self);


GList *
sc_computer_player_get_stored_moves (ScComputerPlayer *self);


void
sc_computer_player_enable_exchange (ScComputerPlayer *self,
                                    gboolean          enabled);

gboolean
sc_computer_player_exchange_enabled (ScComputerPlayer *self);


G_END_DECLS

#endif
