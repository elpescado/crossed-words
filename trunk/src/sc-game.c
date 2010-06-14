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
#include "util.h"

#define BINGO_BONUS 50
#define DEFAULT_TIME 120

G_DEFINE_TYPE (ScGame, sc_game, G_TYPE_OBJECT)

typedef struct _ScPlayerCtx ScPlayerCtx;

gint n_bingos = 0;


struct _ScGamePrivate
{
	/* Private members go here */
	GMainContext *loop_ctx;        /**< Main loop context                   */

	gboolean     running;          /**< Whether or not game has ended       */
	gint         current_player;   /**< Player number whose turn is current */
	Alphabet    *al;               /**< Alphabet used in game               */
	ScBag       *bag;              /**< Bag with tiles                      */
	ScBoard     *board;            /**< Game board                          */
	gint         n_players;        /**< Number of players                   */
	ScPlayerCtx *players[2];       /**< Array of players                    */
	gint         pass_counter;     /**< Number of consecutive passes        */
	gint         time;             /**< Time per player                     */
	guint        timer_id;         /**< Clock timer id                      */

	ScDawg      *dictionary;       /**< Dictionary                          */

	GThread     *owner_thread;     /**< Thread that owns this game          */

	gboolean disposed;
};


struct _ScPlayerCtx {
	ScPlayer    *player;           /**< Player controller                   */
	gint         points;           /**< Number of points scored             */
	gint         bingos;           /**< Number of bingos                    */
	gint         time;             /**< Remaining time                      */
	ScRackModel *rack;             /**< Rack                                */
};


#define CHECK_THREAD(self) do { if (((ScGame*)self)->priv->owner_thread != g_thread_self ()) { g_printerr ("Bad thread\n"); abort (); } } while (0)


enum {
	BEGIN,
	END,
	TICK,

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
sc_game_new (GMainContext *ctx)
{
	ScGame *self = g_object_new (SC_TYPE_GAME, NULL);
	ScGamePrivate *priv = self->priv;
	if (!ctx)
		ctx = g_main_context_default ();
	priv->loop_ctx = g_main_context_ref (ctx);
	return self;
}


GMainContext *
sc_game_get_ctx (ScGame *self)
{
	ScGamePrivate *priv = self->priv;
	return priv->loop_ctx;
}


static void
sc_game_init (ScGame *self)
{
	/* FIXME: Quick hack */
	static Alphabet *_al = NULL;
	if (_al == NULL) {
		_al = alphabet_new ();
		alphabet_load (_al, "lang/pl/alphabet.txt");
	}

	self->priv = SC_GAME_GET_PRIVATE (self);
	ScGamePrivate *priv = self->priv;

	priv->disposed = FALSE;

	priv->al = _al;

	priv->board = sc_board_new (priv->al);
	sc_board_clear (priv->board);

	priv->n_players = 2;

	priv->time = DEFAULT_TIME;

	sc_game_load_dictionary (self);

	priv->bag = sc_bag_new ();

	priv->owner_thread = g_thread_self ();
}


static gboolean
sc_game_timeout_cb (ScGame *self)
{
	sc_game_request_move (self);
	return FALSE;
}


gboolean
sc_game_validate_move (ScGame *game, ScPlayer *player, ScMove *move)
{
	ScGamePrivate *priv = game->priv;
	ScPlayerCtx * ctx = sc_game_get_ctx_by_player (game, player);

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

	if (n_needed_tiles == 0)
		return FALSE;



	return TRUE;
}


static gboolean
sc_game_do_move (ScPlayer *player, ScMove *move, ScGame *game)
{
	ScGamePrivate *priv = game->priv;
	ScPlayerCtx * ctx = sc_game_get_ctx_by_player (game, player);

	foo_assert (ctx != NULL);

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

	/* You have to use at least one tile */
	if (n_needed_tiles == 0)
		return FALSE;


	/* Rate move */
	int points = sc_board_rate_move (priv->board, move);
	g_print ("Rating: %d points\n", points);

	if (sc_board_place_move (priv->board, move)) {
		g_print ("move ok\n");

		ctx->points += points;
		/* sc_board_rate_move handles this */
		if (n_needed_tiles == 7) {
			n_bingos++;
			ctx->bingos++;
//			ctx->points += BINGO_BONUS;
		}

		/* Update players rack */
		sc_rack_model_remove_tiles (ctx->rack, needed_tiles, n_needed_tiles);
		sc_game_fill_rack (game, ctx->rack, priv->bag);


	}	

	priv->pass_counter=0;
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

	priv->pass_counter=0;
	return TRUE;
}

static gboolean
sc_game_move_perform (ScPlayer *player, ScMove *move, ScGame *game)
{
	ScGamePrivate *priv = game->priv;
	switch (move->type) {
		case SC_MOVE_TYPE_MOVE:     /*priv->pass_counter=0;*/ return sc_game_do_move (player, move, game);
		case SC_MOVE_TYPE_EXCHANGE: /*priv->pass_counter=0;*/ return sc_game_do_exchange (player, move, game);
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
//			g_timeout_add (1, (GSourceFunc)sc_game_timeout_cb, game);

			GSource *source = g_timeout_source_new (1);
			g_source_set_callback (source, (GSourceFunc)sc_game_timeout_cb, game, NULL);
			g_source_attach (source, priv->loop_ctx);
			g_source_unref (source);
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

	if (priv->players[num]) {
		g_object_unref (priv->players[num]->player);
		g_free (priv->players[num]);
	}

	if (player) {	
		ScPlayerCtx *ctx = g_new0 (ScPlayerCtx, 1);
		ctx->player = g_object_ref (player);
		ctx->rack   = sc_rack_model_new (priv->al);

		priv->players[num] = ctx;
		g_signal_connect (player, "move-done", G_CALLBACK(sc_game_move_done), self);
	} else {
		priv->players[num] = NULL;
	}
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
	CHECK_THREAD (self);
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
	CHECK_THREAD (self);
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


static gboolean
sc_game_tick (ScGame *self)
{
	/* Seems to be fixed */
	CHECK_THREAD (self);
	g_object_ref (self);
	ScGamePrivate *priv = self->priv;

	if (((GObject*)self)->ref_count == 0) {
		g_printerr ("Something very weird... ref_count = 0 (thread = %p)...\n",
				    g_thread_self ());
		abort ();
	}

	struct _ScPlayerCtx *ctx = priv->players[priv->current_player];
	foo_assert (ctx != NULL);

	if (--ctx->time == 0) {
		g_printerr ("Time over!\n");
		sc_game_end (self);
	} else {
		g_signal_emit (self, signals[TICK], 0);
	}

	g_object_unref (self);
	return TRUE;
}


/**
 * Starts a game
 **/
void
sc_game_start (ScGame *self)
{
	CHECK_THREAD (self);
	ScGamePrivate *priv = self->priv;

	if (priv->running)
		return;

	if (priv->players[0] == NULL || priv->players[1] == NULL)
		return;

	if (priv->board == NULL)
		return;

	priv->pass_counter = 0;
	//sc_board_clear (priv->board);
	sc_bag_load (priv->bag, priv->al);

	priv->running = TRUE;
	priv->current_player = 0;

	int i;
	for (i = 0; i < 2; i++) {
		ScPlayerCtx *ctx = priv->players[i];

		sc_game_fill_rack (self, ctx->rack, priv->bag);
		ctx->time = priv->time;
	}

	g_signal_emit (self, signals[BEGIN], 0);
//	priv->timer_id = g_timeout_add (1000, (GSourceFunc)sc_game_tick, self);


	GSource *source = g_timeout_source_new (1000);
	g_source_set_callback (source, (GSourceFunc)sc_game_tick, self, NULL);
	foo_assert (priv->timer_id == 0);
	priv->timer_id = g_source_attach (source, priv->loop_ctx);
	g_source_unref (source);

	sc_game_request_move (self);
}


/**
 * End a game
 **/
void
sc_game_end (ScGame *self)
{
	CHECK_THREAD (self);
	ScGamePrivate *priv = self->priv;

	if (!priv->running)
		return;

	if (priv->timer_id != 0) {
		GSource *source = g_main_context_find_source_by_id (priv->loop_ctx, priv->timer_id);
		g_source_destroy (source);
//		g_source_remove (priv->timer_id);
		priv->timer_id = 0;
	}

	g_print ("End game\n");

	int i;
	gint t = 0;
	for (i = 0; i < priv->n_players; i++) {
		ScRackModel *rack = priv->players[i]->rack;
		LID tiles[15];
		gint n_tiles;
		int j;

		gint penalty = 0;
		sc_rack_model_get_tiles (rack, tiles, &n_tiles);
		for (j = 0; j < n_tiles; j++) {
			Letter *l = alphabet_lookup_letter (priv->al, tiles[j]);
			if (l) { // unused blanks have no Letter*
				//priv->players[i]->points -= l->value;
				//t += l->value;
				penalty += l->value;
			}
		}
		g_print ("Player %d: penalty %d, curr %d\n", i, penalty, priv->players[i]->points);
		priv->players[i]->points -= penalty;
		t += penalty;
	}

	for (i = 0; i < priv->n_players; i++) {
		ScRackModel *rack = priv->players[i]->rack;
		if (sc_rack_model_count_tiles (rack) == 0) {
			priv->players[i]->points += t;
			g_print ("Player %d: bonus %d\n", i, t);
		}
	}


	priv->running = FALSE;

	g_signal_emit (self, signals[END], 0);
}

gboolean
sc_game_is_running (ScGame *self)
{
	ScGamePrivate *priv = self->priv;
	return priv->running;
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
	return ok;
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
sc_game_get_players_bingos (ScGame *self, ScPlayer *player)
{
	ScPlayerCtx *ctx = sc_game_get_ctx_by_player (self, player);
	return ctx->bingos;
}

gint
sc_game_get_players_time (ScGame *self, ScPlayer *player)
{
	ScPlayerCtx *ctx = sc_game_get_ctx_by_player (self, player);
	return ctx->time;
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


ScPlayer *
sc_game_get_current_player (ScGame *self)
{
	ScGamePrivate *priv = self->priv;
	return priv->players[priv->current_player]->player;
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
	/* FIXME: Quick hack */
	static ScDawg *_dict = NULL;
	if (_dict == NULL) {
		_dict = sc_dawg_load ("gaddag.dag");
	}
	ScGamePrivate *priv = self->priv;

	if (priv->dictionary)
		sc_game_unload_dictionary (self);

	priv->dictionary = sc_dawg_ref (_dict);
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


void
sc_game_set_time (ScGame *game,
                  gint    time)
{
	ScGamePrivate *priv = game->priv;
	g_return_if_fail (!priv->running);
	priv->time = time;
}


gint
sc_game_get_time (ScGame *game)
{
	ScGamePrivate *priv = game->priv;
	return priv->time;
}


ScGameState *
sc_game_save_state (ScGame *game, ScPlayer *player, ScMove *move)
{
	ScGamePrivate *priv = game->priv;
	ScGameState *state = g_new0 (ScGameState, 1);
	int points = 0;

	if (move == NULL) {
		sc_board_get_tiles (priv->board, state->board_state);	
	} else {
		ScBoard *b2 = sc_board_copy (priv->board);
		points = sc_board_rate_move (priv->board, move);
		sc_board_place_move (b2, move);
		sc_board_get_tiles (b2, state->board_state);	
		g_object_unref (b2);
	}

	int i;
	for (i = 0; i < 2; i++) {
		state->scores[i] = priv->players[i]->points;

		if (player == priv->players[i]->player) {
			LID tiles[7];
			gint n_tiles;
			sc_rack_model_get_tiles (priv->players[i]->rack, tiles, &n_tiles);
			sc_rack_assign_letters (&(state->racks[i]), tiles, n_tiles);

			state->scores[i] += points;
		}
	}


	return state;
}


void
sc_game_restore_state (ScGame *game, ScGameState *state)
{
	ScGamePrivate *priv = game->priv;
	LID used_letters[SC_RACK_SIZE] = {0};
	int i;

	sc_board_set_tiles (priv->board, state->board_state);
	for (i = 0; i < BOARD_SIZE*BOARD_SIZE; i++) {
		int val = state->board_state[i];
		used_letters[sc_letter_value(val)]++;
	}


	for (i = 0; i < 2; i++) {
		LID tiles[7];
		gint n_tiles;
		int j;

		struct _ScPlayerCtx *ctx = priv->players[i];
		//ctx->points = state->scores[i];
		ctx->bingos = 0;
		ctx->time = 15*60;

		sc_rack_to_letters (&(state->racks[i]), tiles, &n_tiles);
		sc_rack_model_set_tiles (priv->players[i]->rack, tiles, n_tiles);
		for (j = 0; j < n_tiles; j++) {
			used_letters[tiles[j]]++;
		}
	}


	/* Reconstruct game */
	sc_bag_clear (priv->bag);
	for (i = 0; i < priv->al->n_letters; i++) {
		Letter *l = priv->al->letters + i;
		int n = l->count - used_letters[l->index];
		while (n-- > 0) {
			sc_bag_push (priv->bag, l->index);
		}
	}


	//sc_game_fill_rack (game, priv->players[i]->rack, priv->bag);

}



static void
sc_game_dispose (GObject *object)
{
	CHECK_THREAD (object);

	ScGame *self = (ScGame*) object;
	ScGamePrivate *priv = self->priv;


	/* Make sure dispose is called only once */
	if (priv->disposed) {
		return;
	}

	sc_game_unload_dictionary (self);
	g_main_context_unref (priv->loop_ctx);

	sc_game_set_player (self, 0, NULL);
	sc_game_set_player (self, 1, NULL);

	priv->disposed = TRUE;

	/* Chain up to the parent class */
	G_OBJECT_CLASS (sc_game_parent_class)->dispose (object);
}


static void
sc_game_finalize (GObject *object)
{
	CHECK_THREAD (object);

	ScGame *self = (ScGame*) object;
	ScGamePrivate *priv = self->priv;

	if (priv->timer_id != 0) {
		GSource *source = g_main_context_find_source_by_id (priv->loop_ctx, priv->timer_id);
		g_source_destroy (source);
		priv->timer_id = 0;
	}

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

	signals[TICK] = g_signal_new ("tick",
			G_TYPE_FROM_CLASS (klass),
			(GSignalFlags)(G_SIGNAL_RUN_LAST),
			0,
			NULL,
			NULL,
			g_cclosure_marshal_VOID__VOID,
			G_TYPE_NONE, 0,
			NULL);
}

