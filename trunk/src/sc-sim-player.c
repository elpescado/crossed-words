/*
 * Crossed Words
 * -------------
 *
 * Copyright (C) 2009 Przemysław Sitek
 * 
 */
#include <string.h>

#include <gtk/gtk.h>

#include "sc-sim-player.h"
#include "sc-game.h"


G_DEFINE_TYPE (ScSimPlayer, sc_sim_player, SC_TYPE_COMPUTER_PLAYER)

struct _ScSimPlayerPrivate
{
	/* Private members go here */
	gint moves_to_consider;

	GThread **worker_threads;

	gboolean disposed;
};


typedef struct {
	gint   rating;
	ScRack rack;
} _Player;

#define SC_SIM_PLAYER_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	SC_TYPE_SIM_PLAYER, ScSimPlayerPrivate))


ScSimPlayer*
sc_sim_player_new (void)
{
	ScSimPlayer *self = g_object_new (SC_TYPE_SIM_PLAYER, NULL);
	return self;
}


static gpointer
worker_thread (gpointer data)
{
	GMainContext *ctx = g_main_context_new ();
	GMainLoop *loop = g_main_loop_new (ctx, FALSE);

	g_main_loop_run (loop);
}


static void
sc_sim_player_init (ScSimPlayer *self)
{
	self->priv = SC_SIM_PLAYER_GET_PRIVATE (self);
	ScSimPlayerPrivate *priv = self->priv;
	priv->disposed = FALSE;

	int n_threads = 2;
	int i;

	priv->worker_threads = g_new0 (GThread*, n_threads);
	for (i = 0; i < n_threads; i++) {
		GError *err = NULL;
		GThread *thread = g_thread_create (worker_thread, self, TRUE, &err);
		if (thread == NULL || err) {
			// handle error;
			g_printerr ("Thread creation failed: #%d: %s\n", err->code, err->message);
		}
		priv->worker_threads[i] = thread;
	}
}


static void
sc_sim_player_calculate_opponent_rack (ScSimPlayer      *self,
                                       ScBoard          *board,
                                       Alphabet         *al,
                                       ScRack           *rack)
{
	int i, j;
	ScRack my_rack;

	sc_player_get_rack (SC_PLAYER (self), &my_rack);

	rack->letters[0] = 2; // Two blanks

	/* Init rack with all tiles */
	for (i = 0; i < al->n_letters; i++) {
		Letter *l = al->letters+i;
		rack->letters[l->index] = l->count;
	}

	/* Subtract tiles on board */
	for (i = 0; i < BOARD_SIZE; i++) {
		for (j = 0; j < BOARD_SIZE; j++) {
			LID lid = sc_board_get_lid (board, i, j);
			if (lid != 0) {
				if (sc_letter_is_blank (lid)) {
					sc_rack_remove (rack, 0);
				} else {
					sc_rack_remove (rack, lid);
				}
			}
		}
	}

	/* Subtract players rack */
	for (i = 0; i < al->n_letters; i++) {
		rack->letters[i] -= my_rack.letters[i];
	}
}


#if 0
static int
sc_sim_player_endgame (ScSimPlayer *self,
                       ScBoard     *board,
                       _Player     *me,
                       _Player     *opponent,
                       gboolean     my_turn)
{
	GList *moves;
	GList *node;
	
	sc_computer_player_generate_moves (SC_COMPUTER_PLAYER (self), board, &(me->rack));
	moves = sc_computer_player_steal_stored_moves (SC_COMPUTER_PLAYER (self));

	for (node = moves; node; node = node->next) {
		_MoveProposal *mp = node->data;
		LID tiles[15];
		gint n_tiles;
		gint rating;
		gint i;
		_Player p;

		memcpy (&p, my_turn ? me : opponent, sizeof (p));
		//_Player *p = my_turn ? me : opponent;

		//g_printerr ("Move: %d points\n", mp->rating);
		if (mp->combined_rating < 16)
			continue;

		ScBoard *b2 = sc_board_copy (board);
		rating = sc_board_rate_move (b2, &(mp->move));
		sc_board_get_needed_tiles (b2, &(mp->move), tiles, &n_tiles);
		sc_board_place_move (b2, &(mp->move));
		p.rating += rating;

		for (i = 0; i < n_tiles; i++)
			sc_rack_remove (&(p.rack), tiles[i]);

		sc_sim_player_endgame (self, b2,
				               my_turn ? &p       : me,
							   my_turn ? opponent : &p,
		                       !my_turn);
		g_object_unref (b2);
	}

	g_list_foreach (moves, (GFunc)g_free, NULL);
	g_list_free (moves);
	return 1;
}
                       


/**
 * Explore game state space and pick best strategy
 **/
static void
sc_sim_player_do_endgame_move (ScSimPlayer *self)
{
	//ScSimPlayerPrivate *priv = self->priv;
	ScBoard *board;
	ScMove move;
//	ScRack my_rack;
//	ScRack opponent_rack;
	_Player me;
	_Player opponent;
	Alphabet *al = sc_game_get_alphabet (SC_GAME (SC_PLAYER(self)->game));
	GList *moves;
	GList *node;

	g_printerr ("Endgame move");

	board = sc_player_get_board (SC_PLAYER (self));
	
	sc_player_get_rack (SC_PLAYER (self), &(me.rack));
	sc_sim_player_calculate_opponent_rack (self, board, al, &(opponent.rack));
	
	sc_computer_player_generate_moves (SC_COMPUTER_PLAYER (self), board, &(me.rack));
	moves = sc_computer_player_steal_stored_moves (SC_COMPUTER_PLAYER (self));

	for (node = moves; node; node = node->next) {
		_MoveProposal *mp = node->data;

		ScBoard *b2 = sc_board_copy (board);
		sc_board_place_move (b2, &(mp->move));
	}

	sc_sim_player_endgame (self, board, &me, &opponent, TRUE);

	/* Analyze moves */
	for (node = moves; node; node = node->next) {
	}

	g_list_foreach (moves, (GFunc)g_free, NULL);
	g_list_free (moves);


	move.type = SC_MOVE_TYPE_PASS;
	sc_player_do_move (SC_PLAYER (self), &move);	
	g_printerr (" done\n");
}
#endif


/*
static void
sc_sim_player_simulate_move (ScComputerPlayer *self,
                             ScMove           *move)
{
}
*/


/**
 * Analyze moves
 **/
static ScMove *
_sc_sim_player_analyze_moves (ScComputerPlayer *cp)
{
	ScSimPlayer *self = SC_SIM_PLAYER (cp);
	ScSimPlayerPrivate *priv = self->priv;

	gint max_score = 0;
	ScMove *best_move = NULL;
	GList *tmp;
	gint i;

	sc_computer_player_sort_moves (SC_COMPUTER_PLAYER (self));

	for (tmp = sc_computer_player_get_stored_moves (SC_COMPUTER_PLAYER (self)), i = 0;
	     tmp && i < priv->moves_to_consider; tmp = tmp->next, i++) {
	}


	for (tmp = sc_computer_player_get_stored_moves (SC_COMPUTER_PLAYER (self));
	     tmp; tmp = tmp->next) {
		_MoveProposal *mp = tmp->data;
		if (mp->combined_rating > max_score) {
			max_score = mp->combined_rating;
			best_move = &(mp->move);
		}
	}
	return best_move;
}



static void
sc_sim_player_your_turn (ScPlayer *player)
{
	ScSimPlayer *self = SC_SIM_PLAYER (player);
	ScGame *game = SC_PLAYER(self)->game;
//	if (sc_game_get_remaining_tiles (game) == 0) {
//		sc_sim_player_do_endgame_move (self);
//	} else {
		ScSimPlayerClass *pro;
		ScComputerPlayerClass *comp;

		pro = SC_SIM_PLAYER_GET_CLASS (self);
		comp = g_type_class_peek_parent (pro);

		SC_PLAYER_CLASS(comp)->your_turn (SC_PLAYER(self));
//	}
}



static void
sc_sim_player_dispose (GObject *object)
{
	ScSimPlayer *self = (ScSimPlayer*) object;
	ScSimPlayerPrivate *priv = self->priv;


	/* Make sure dispose is called only once */
	if (priv->disposed) {
		return;
	}
	priv->disposed = TRUE;


	/* Chain up to the parent class */
	G_OBJECT_CLASS (sc_sim_player_parent_class)->dispose (object);
}


static void
sc_sim_player_finalize (GObject *object)
{
	G_OBJECT_CLASS (sc_sim_player_parent_class)->finalize (object);
}

	
static void
sc_sim_player_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
	//ScSimPlayer* self = SC_SIM_PLAYER (object);
	//ScSimPlayerPrivate* priv = self->priv;

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
sc_sim_player_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
	//ScSimPlayer* self = SC_SIM_PLAYER (object);
	//ScSimPlayerPrivate* priv = self->priv;

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
sc_sim_player_class_init (ScSimPlayerClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->get_property = sc_sim_player_get_property;
	gobject_class->set_property = sc_sim_player_set_property;
	gobject_class->dispose = sc_sim_player_dispose;
	gobject_class->finalize = sc_sim_player_finalize;

	SC_COMPUTER_PLAYER_CLASS (klass)->analyze_moves = _sc_sim_player_analyze_moves;
//	SC_PLAYER_CLASS (klass)->your_turn = sc_sim_player_your_turn;

	g_type_class_add_private (klass, sizeof (ScSimPlayerPrivate));
}

