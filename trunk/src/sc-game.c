/*
 * Scrabble
 * --------
 *
 * Copyright (C) 2009 Przemysław Sitek
 * 
 */

#include <gtk/gtk.h>

#include "common.h"
#include "sc-bag.h"
#include "sc-board.h"
#include "sc-game.h"
#include "sc-dawg.h"

#define BINGO_BONUS 50

G_DEFINE_TYPE (ScGame, sc_game, G_TYPE_OBJECT)

typedef struct _ScPlayerCtx ScPlayerCtx;

gint n_bingos = 0;


struct _ScGamePrivate
{
	/* Private members go here */
	gboolean     running;
	gint         current_player;
	Alphabet    *al;
	ScBag       *bag;
	ScBoard     *board;
	gint         n_players;
	ScPlayerCtx *players[2];
	gint         pass_counter;

	ScDawg      *dictionary;

	gboolean disposed;
};


struct _ScPlayerCtx {
	ScPlayer    *player;
	gint         points;
	ScRackModel *rack;
};



enum {
	BEGIN,
	END,

	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL];



static void
sc_game_request_move (ScGame *self);

static ScPlayerCtx *
sc_game_get_ctx_by_player (ScGame *self, ScPlayer *player);


static void
sc_game_load_dictionary (ScGame *self);


static void
sc_game_unload_dictionary (ScGame *self);




#define SC_GAME_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	SC_TYPE_GAME, ScGamePrivate))


ScGame*
sc_game_new (void)
{
	ScGame *self = g_object_new (SC_TYPE_GAME, NULL);
	return self;
}


static void
sc_game_init (ScGame *self)
{
	self->priv = SC_GAME_GET_PRIVATE (self);
	ScGamePrivate *priv = self->priv;

	priv->disposed = FALSE;

	priv->al = alphabet_new ();
	alphabet_load (priv->al, "lang/pl/alphabet.txt");

	priv->board = sc_board_new (priv->al);

	priv->n_players = 2;

	sc_game_load_dictionary (self);

	priv->bag = sc_bag_new ();
}


static gboolean
sc_game_timeout_cb (ScGame *self)
{
	sc_game_request_move (self);
	return FALSE;
}


static gboolean
sc_game_do_move (ScPlayer *player, ScMove *move, ScGame *game)
{
	ScGamePrivate *priv = game->priv;
	ScPlayerCtx * ctx = sc_game_get_ctx_by_player (game, player);
	g_print ("Move done (");
	{
		int i;
		for (i = 0; i < move->n_letters; i++) {
			Letter *l = alphabet_lookup_letter (priv->al, move->letters[i]);
			g_print ("%s", l->label);
		}
	}
	g_print (")\n");

	/* Can move be placed on board? */
	if (! sc_board_validate_move (priv->board, move))
		return FALSE;

	/* Does player have needed tiles? */
	LID needed_tiles[BOARD_SIZE] = {0};
	gint n_needed_tiles = 0;
	sc_board_get_needed_tiles (priv->board, move, needed_tiles, &n_needed_tiles);
	if (! sc_rack_model_has_tiles (ctx->rack, needed_tiles, n_needed_tiles)) {
		g_print ("You don't have needed tiles\n");
		return FALSE;
	}

	/* Rate move */
	int points = sc_board_rate_move (priv->board, move);
	g_print ("Rating: %d points\n", points);

	if (sc_board_place_move (priv->board, move)) {
		g_print ("move ok\n");

		ctx->points += points;
		/* sc_board_rate_move handles this */
		if (n_needed_tiles == 7) {
			n_bingos++;
//			ctx->points += BINGO_BONUS;
		}

		/* Update players rack */
		sc_rack_model_remove_tiles (ctx->rack, needed_tiles, n_needed_tiles);
		sc_game_fill_rack (game, ctx->rack, priv->bag);


	}	

	return TRUE;
}

static gboolean
sc_game_do_exchange (ScPlayer *player, ScMove *move, ScGame *game)
{
	ScGamePrivate *priv = game->priv;
	ScPlayerCtx * ctx = sc_game_get_ctx_by_player (game, player);
	sc_rack_model_remove_tiles (ctx->rack, move->letters, move->n_letters);

	int i;
	for (i = 0; i < move->n_letters; i++) {
		sc_bag_push (priv->bag, move->letters[i]);
	}
	sc_bag_shuffle (priv->bag);

	sc_game_fill_rack (game, ctx->rack, priv->bag);

	return TRUE;
}

static gboolean
sc_game_move_perform (ScPlayer *player, ScMove *move, ScGame *game)
{
	ScGamePrivate *priv = game->priv;
	switch (move->type) {
		case SC_MOVE_TYPE_MOVE:     priv->pass_counter=0; return sc_game_do_move (player, move, game);
		case SC_MOVE_TYPE_EXCHANGE: priv->pass_counter=0; return sc_game_do_exchange (player, move, game);
		case SC_MOVE_TYPE_PASS:     priv->pass_counter++; return TRUE;
		default: return FALSE;
	}
}
static gboolean
sc_game_move_done (ScPlayer *player, ScMove *move, ScGame *game)
{
	ScGamePrivate *priv = game->priv;
	if (sc_game_move_perform (player, move, game)) {

		/* End game? */
		if (sc_game_check_end (game)) {
			sc_game_end (game);
		} else {
			/* next player */
			priv->current_player = (priv->current_player+1) % priv->n_players;
			g_timeout_add (1, (GSourceFunc)sc_game_timeout_cb, game);
		}

		return TRUE;
	} else {
		return FALSE;
	}
}

void
sc_game_set_player (ScGame *self, gint num, ScPlayer *player)
{
	ScGamePrivate *priv = self->priv;

	ScPlayerCtx *ctx = g_new0 (ScPlayerCtx, 1);
	ctx->player = g_object_ref (player);
	ctx->rack   = sc_rack_model_new (priv->al);

	if (priv->players[num]) {
		g_object_unref (priv->players[num]->player);
		g_free (priv->players[num]);
	}

	
	priv->players[num] = ctx;
	g_signal_connect (player, "move-done", G_CALLBACK(sc_game_move_done), self);
}


ScBoard *
sc_game_get_board (ScGame *self)
{
	ScGamePrivate *priv = self->priv;

	return g_object_ref (priv->board);
}


void
sc_game_set_board (ScGame *self, ScBoard *board)
{
	ScGamePrivate *priv = self->priv;

	if (priv->board)
		g_object_unref (priv->board);

	priv->board = g_object_ref (board);
}


/**
 * Get a PlayerCtx associated with a particular player
 **/
static ScPlayerCtx *
sc_game_get_ctx_by_player (ScGame *self, ScPlayer *player)
{
	ScGamePrivate *priv = self->priv;
	
	int i;
	for (i = 0; i < priv->n_players; i++)
		if (priv->players[i]->player == player)
			return priv->players[i];

	return NULL;
}


/**
 * Request move from current player
 **/
static void
sc_game_request_move (ScGame *self)
{
	ScGamePrivate *priv = self->priv;

	g_signal_emit_by_name (priv->players[priv->current_player]->player, "your-turn");
}


/**
 * Fill user's rack
 **/
void
sc_game_fill_rack (ScGame *game, ScRackModel *rack, ScBag *bag)
{
//	ScGamePrivate *priv = self->priv;

	while (sc_rack_model_count_tiles (rack) < 7 && sc_bag_n_tiles (bag) > 0)
		sc_rack_model_add_tile (rack, sc_bag_pop (bag));
}


gboolean
sc_game_check_end (ScGame *self)
{
	ScGamePrivate *priv = self->priv;

	if (priv->pass_counter == 2*priv->n_players)
		return TRUE;

	if (sc_bag_n_tiles (priv->bag) != 0)
		return FALSE;

	int i;
	for (i = 0; i < priv->n_players; i++) {
		ScRackModel *rack = priv->players[i]->rack;
		if (sc_rack_model_count_tiles (rack) == 0) {
			return TRUE;
		}
	}

	return FALSE;
}


/**
 * Starts a game
 **/
void
sc_game_start (ScGame *self)
{
	ScGamePrivate *priv = self->priv;

	if (priv->running)
		return;

	if (priv->players[0] == NULL || priv->players[1] == NULL)
		return;

	if (priv->board == NULL)
		return;

	priv->pass_counter = 0;
	sc_board_clear (priv->board);
	sc_bag_load (priv->bag, priv->al);

	priv->running = TRUE;
	priv->current_player = 0;

	int i;
	for (i = 0; i < 2; i++) {
		ScPlayerCtx *ctx = priv->players[i];

		sc_game_fill_rack (self, ctx->rack, priv->bag);
	}

	g_signal_emit (self, signals[BEGIN], 0);

	sc_game_request_move (self);
}


/**
 * End a game
 **/
void
sc_game_end (ScGame *self)
{
	ScGamePrivate *priv = self->priv;

	if (!priv->running)
		return;

	g_print ("End game\n");

	int i;
	gint t = 0;
	for (i = 0; i < priv->n_players; i++) {
		ScRackModel *rack = priv->players[i]->rack;
		LID tiles[15];
		gint n_tiles;
		int j;

		sc_rack_model_get_tiles (rack, tiles, &n_tiles);
		for (j = 0; j < n_tiles; j++) {
			Letter *l = alphabet_lookup_letter (priv->al, tiles[j]);
			if (l) { // unused blanks have no Letter*
				priv->players[i]->points -= l->value;
				t += l->value;
			}
		}
	}

	for (i = 0; i < priv->n_players; i++) {
		ScRackModel *rack = priv->players[i]->rack;
		if (sc_rack_model_count_tiles (rack)) {
			priv->players[i]->points += t;
		}
	}


	priv->running = FALSE;

	g_signal_emit (self, signals[END], 0);
}



gboolean
sc_game_init_move (ScGame *game, ScMove *move, gint x, gint y, ScOrientation o, const gchar *str)
{
	ScGamePrivate *priv = game->priv;

	move->x = x;
	move->y = y;
	move->orientation = o;
	//move->n_letters = g_utf8_strlen (str, -1);
	gboolean ok = alphabet_translate (sc_board_get_alphabet (priv->board), str, move->letters, &(move->n_letters));
}


ScRackModel *
sc_game_get_players_rack (ScGame *self, ScPlayer *player)
{
	ScPlayerCtx *ctx = sc_game_get_ctx_by_player (self, player);
	if (ctx == NULL)
		return NULL;

	return g_object_ref (ctx->rack);
}


gint
sc_game_get_players_score (ScGame *self, ScPlayer *player)
{
	ScPlayerCtx *ctx = sc_game_get_ctx_by_player (self, player);
	return ctx->points;
}


gint
sc_game_get_n_players (ScGame *self)
{
	ScGamePrivate *priv = self->priv;
	return priv->n_players;
}


ScPlayer *
sc_game_get_player (ScGame *self, gint n)
{
	ScGamePrivate *priv = self->priv;
	return priv->players[n]->player;
}


Alphabet *
sc_game_get_alphabet (ScGame *self)
{
	ScGamePrivate *priv = self->priv;

	return sc_board_get_alphabet (priv->board);
}


static void
sc_game_load_dictionary (ScGame *self)
{
	ScGamePrivate *priv = self->priv;

	if (priv->dictionary)
		sc_game_unload_dictionary (self);

	priv->dictionary = sc_dawg_load ("gaddag.dag");
}


static void
sc_game_unload_dictionary (ScGame *self)
{
	ScGamePrivate *priv = self->priv;
	if (priv->dictionary) {
		sc_dawg_unref (priv->dictionary);
	}
}


ScDawg *
sc_game_get_dictionary (ScGame *game)
{
	ScGamePrivate *priv = game->priv;
	return sc_dawg_ref (priv->dictionary);
}


gint
sc_game_get_remaining_tiles (ScGame *game)
{
	ScGamePrivate *priv = game->priv;
	return sc_bag_n_tiles (priv->bag);
}


static void
sc_game_dispose (GObject *object)
{
	ScGame *self = (ScGame*) object;
	ScGamePrivate *priv = self->priv;


	/* Make sure dispose is called only once */
	if (priv->disposed) {
		return;
	}
	//g_printerr ("sc_game_dispose()\n");

	sc_game_unload_dictionary (self);

	priv->disposed = TRUE;

	/* Chain up to the parent class */
	G_OBJECT_CLASS (sc_game_parent_class)->dispose (object);
}


static void
sc_game_finalize (GObject *object)
{
	//g_printerr ("sc_game_finalize()\n");
	G_OBJECT_CLASS (sc_game_parent_class)->finalize (object);
}

	
static void
sc_game_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
	ScGame* self = SC_GAME (object);
	G_UNUSED (self);

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
sc_game_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
	ScGame* self = SC_GAME (object);
	G_UNUSED (self);

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
sc_game_class_init (ScGameClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->get_property = sc_game_get_property;
	gobject_class->set_property = sc_game_set_property;
	gobject_class->dispose = sc_game_dispose;
	gobject_class->finalize = sc_game_finalize;

	g_type_class_add_private (klass, sizeof (ScGamePrivate));

	signals[BEGIN] = g_signal_new ("begin",
			G_TYPE_FROM_CLASS (klass),
			(GSignalFlags)(G_SIGNAL_RUN_LAST),
			0,
			NULL,
			NULL,
			g_cclosure_marshal_VOID__VOID,
			G_TYPE_NONE, 0,
			NULL);

	signals[END] = g_signal_new ("end",
			G_TYPE_FROM_CLASS (klass),
			(GSignalFlags)(G_SIGNAL_RUN_LAST),
			0,
			NULL,
			NULL,
			g_cclosure_marshal_VOID__VOID,
			G_TYPE_NONE, 0,
			NULL);
}

