/*
 * Scrabble
 * --------
 *
 * Copyright (C) 2009 Przemysław Sitek
 * 
 */

#ifndef __SC_GAME_H__
#define __SC_GAME_H__

/* Includes here */
#include <gtk/gtkwidget.h>
#include <gtk/gtkwindow.h>

#include "alphabet.h"
#include "sc-player.h"
#include "sc-board.h"
#include "sc-rack-model.h"
#include "sc-bag.h"
#include "sc-dawg.h"

G_BEGIN_DECLS

#define SC_TYPE_GAME \
	(sc_game_get_type ())
#define SC_GAME(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST ((obj), SC_TYPE_GAME, ScGame))
#define SC_GAME_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST ((klass), SC_TYPE_GAME, ScGameClass))
#define SC_IS_GAME(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), SC_TYPE_GAME)
#define SC_IS_GAME_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), SC_TYPE_GAME))
#define SC_GAME_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), SC_TYPE_GAME, ScGameClass))


typedef struct _ScGame		ScGame;
typedef struct _ScGamePrivate		ScGamePrivate;
typedef struct _ScGameClass		ScGameClass;

struct _ScGame{
	GObject		parent;
	ScGamePrivate	*priv;
};

struct _ScGameClass
{
	GObjectClass		parent;

	/* Signals */
};


GType
sc_game_get_type				(void);


ScGame*
sc_game_new				(GMainContext *ctx);


void
sc_game_set_player (ScGame *self, gint num, ScPlayer *player);


ScBoard *
sc_game_get_board (ScGame *self);


void
sc_game_set_board (ScGame *self, ScBoard *board);


Alphabet *
sc_game_get_alphabet (ScGame *self);


void
sc_game_start (ScGame *game);


void
sc_game_end (ScGame *self);


gboolean
sc_game_check_end (ScGame *self);


void
sc_game_fill_rack (ScGame *game, ScRackModel *rack, ScBag *bag);


gboolean
sc_game_init_move (ScGame *game, ScMove *move, gint x, gint y, ScOrientation, const gchar *str);


ScRackModel *
sc_game_get_players_rack (ScGame *self, ScPlayer *player);


gint
sc_game_get_players_score (ScGame *self, ScPlayer *player);


gint
sc_game_get_players_bingos (ScGame *self, ScPlayer *player);


gint
sc_game_get_players_time (ScGame *self, ScPlayer *player);


gint
sc_game_get_n_players (ScGame *self);


ScPlayer *
sc_game_get_player (ScGame *game, gint n);


ScPlayer *
sc_game_get_current_player (ScGame *game);



ScDawg *
sc_game_get_dictionary (ScGame *game);


gint
sc_game_get_remaining_tiles (ScGame *game);


void
sc_game_set_time (ScGame *game,
                  gint    time);


gint
sc_game_get_time (ScGame *game);


G_END_DECLS

#endif
