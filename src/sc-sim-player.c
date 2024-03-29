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
#include "sc-simulator.h"
#include "sc-game.h"


#define SC_INF 10000

G_DEFINE_TYPE (ScSimPlayer, sc_sim_player, SC_TYPE_COMPUTER_PLAYER)

struct _ScSimPlayerPrivate
{
	/* Private members go here */
	gint moves_to_consider;
	ScSimulator *simulator;

	gint   n_simulations_pending;
	gint   best_move_rating;
	gint   best_move_wins;
	ScMove best_move;

	gint   min_rating;

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



static void
sc_sim_player_init (ScSimPlayer *self)
{
	self->priv = SC_SIM_PLAYER_GET_PRIVATE (self);
	ScSimPlayerPrivate *priv = self->priv;
	priv->disposed = FALSE;
	priv->moves_to_consider = 5;
	priv->min_rating = 20;

	priv->simulator = sc_simulator_new ();

	/*
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
	*/
}


static void
sc_sim_player_what_a_terrible_failure (ScSimPlayer *self)
{
//	ScMove move;
//	move.type = SC_MOVE_TYPE_PASS;
//	sc_player_do_move (SC_PLAYER (self), &move);

	// call ScComputerPlayer::analyze_moves
	ScSimPlayerClass *sim;
	ScComputerPlayerClass *comp;

	sim = SC_SIM_PLAYER_GET_CLASS (self);
	comp = g_type_class_peek_parent (sim);

	ScMove *m_move = comp->analyze_moves (SC_COMPUTER_PLAYER (self));
	if (m_move && sc_player_do_move (SC_PLAYER (self), m_move)) {
	} else {
		ScMove move;
		if (sc_computer_player_exchange_enabled (SC_COMPUTER_PLAYER (self))) {
			move.type = SC_MOVE_TYPE_EXCHANGE;

			ScRack rack;
			sc_player_get_rack (SC_PLAYER (self), &rack);
			sc_rack_to_letters (&rack, move.letters, &(move.n_letters));
			
			if (sc_game_get_remaining_tiles (SC_GAME (SC_PLAYER(self)->game)) < 8)
				move.type = SC_MOVE_TYPE_PASS;

		} else {
			move.type = SC_MOVE_TYPE_PASS;
		}
		sc_player_do_move (SC_PLAYER (self), &move);
	}

}





static void
sc_sim_player_simulation_finished (ScSimPlayer     *self,
                                   ScSimulatorTask *task)
{
	ScSimPlayerPrivate *priv = self->priv;
	ScGame *game = SC_GAME (SC_PLAYER(self)->game);
	gint avg;
	gint wins;
	gint rating;

	if (sc_game_is_running (game)) {

		sc_simulator_task_get_scores (task, &avg, &wins);
		rating = sc_simulator_task_get_move_rating (task);
		//hack
		gint sc = wins;

		/* Now, check if it's a good move */
		if (sc < priv->best_move_wins ||
			(sc == priv->best_move_wins && rating > priv->best_move_rating)) {
			// Try to minimize `sc'
			priv->best_move_wins = sc;
			priv->best_move_rating = rating;
			memcpy (&(priv->best_move), sc_simulator_task_get_move (task), sizeof (ScMove));
		}

		if (--priv->n_simulations_pending == 0) {
			/* All of simulations complete */
			if (priv->best_move_wins != SC_INF) {
				sc_player_do_move (SC_PLAYER (self), &(priv->best_move));
			} else {
				sc_sim_player_what_a_terrible_failure (self);
			}
		}

	}

	g_object_unref (game);
}


static gboolean
sc_sim_player_simulation_finished_proxy2 (gpointer data)
{
	void **args = (void **)data;

	ScSimPlayer *sim = SC_SIM_PLAYER (args[0]);
	ScSimulatorTask *task = (ScSimulatorTask *)args[1];

	sc_sim_player_simulation_finished (sim, task);
	sc_simulator_task_unref (task);
	return FALSE;
}


static void
sc_sim_player_simulation_finished_proxy (ScSimulator     *sim,
                                         ScSimulatorTask *task,
                                         gpointer         user_data)
{
	ScPlayer *player = SC_PLAYER (user_data);
	ScGame *game = SC_GAME (player->game);


	void **args = (gpointer *) g_malloc (sizeof(gpointer*) * 2);
	args[0] = user_data;
	args[1] = sc_simulator_task_ref (task);

	GSource *source = g_timeout_source_new (0);
	g_source_set_callback (source, (GSourceFunc) sc_sim_player_simulation_finished_proxy2,
			               args, g_free);
	g_source_attach (source, sc_game_get_ctx (game));
	g_source_unref (source);
}

static void
sc_sim_player_simulate_move (ScSimPlayer      *self,
                             ScMove           *move,
							 gint              rating)
{
	//ScSimulator *sim = sc_simulator_new ();
	ScSimPlayerPrivate *priv = self->priv;
	ScSimulator *sim = priv->simulator;
	ScGame *game = SC_PLAYER(self)->game;

	g_object_ref (game);

	priv->n_simulations_pending++;
	priv->best_move_wins = SC_INF;
	priv->best_move_rating = 0;

	sc_simulator_run (sim, game, SC_PLAYER (self), move, rating,
			          sc_sim_player_simulation_finished_proxy, self);
//	sc_simulator_free (sim);
}



/**
 * Analyze moves
 **/
static ScMove *
_sc_sim_player_analyze_moves (ScComputerPlayer *cp)
{
	ScSimPlayer *self = SC_SIM_PLAYER (cp);
	ScSimPlayerPrivate *priv = self->priv;

	GList *tmp;
	gint i;

	priv->n_simulations_pending = 0;

	sc_computer_player_sort_moves (SC_COMPUTER_PLAYER (self));

	GList *moves = sc_computer_player_get_stored_moves (SC_COMPUTER_PLAYER (self));
	if (moves) {
		_MoveProposal *mp0 = moves->data;

		for (tmp = moves->next, i = 0; tmp && i+1 < priv->moves_to_consider; tmp = tmp->next, i++) {
			_MoveProposal *mp = tmp->data;
			
			if (100 * mp->move_rating < 86 * mp0->move_rating)
				break;

			if (mp->move_rating < priv->min_rating)
				break;
		}

		gint n_moves = i + 1;
		g_print ("Moves worth considering: %d\n", n_moves);

		if (n_moves > 1) {
			for (tmp = moves, i = 0; tmp && i < n_moves; tmp = tmp->next, i++) {
				_MoveProposal *mp = tmp->data;

				sc_sim_player_simulate_move (self, &(mp->move), mp->move_rating);
			}
		} else {
			/* One sensible move... */
			sc_player_do_move (SC_PLAYER (self), &(mp0->move));
		}

	} else {
		sc_sim_player_what_a_terrible_failure (self);
	}


	return NULL;
}



static void
sc_sim_player_your_turn (ScPlayer *player)
{
	ScComputerPlayer *self = SC_COMPUTER_PLAYER (player);
	ScBoard *board;
	ScRack rack;

	sc_player_get_rack (SC_PLAYER (self), &rack);
	board = sc_player_get_board (SC_PLAYER (self));
	sc_computer_player_generate_moves (self, board, &rack);

	ScMove *m_move = sc_computer_player_analyze_moves (self);
	if (m_move && sc_player_do_move (SC_PLAYER (self), m_move)) {
		// Ok, move was done
	}
	sc_computer_player_clear_moves (self);
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
	ScSimPlayer *self = (ScSimPlayer*) object;
	ScSimPlayerPrivate *priv = self->priv;
	sc_simulator_free (priv->simulator);
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
	SC_PLAYER_CLASS (klass)->your_turn = sc_sim_player_your_turn;

	g_type_class_add_private (klass, sizeof (ScSimPlayerPrivate));
}

