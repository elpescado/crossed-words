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
	priv->moves_to_consider = 1;

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




static void
sc_sim_player_simulate_move (ScComputerPlayer *self,
                             ScMove           *move)
{

}


static void
sc_sim_player_simulation_finished (ScSimPlayer     *self,
                                   ScSimulatorTask *task)
{
//	g_printerr ("OK!!!\n");
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
		ScSimulator *sim = sc_simulator_new ();
		ScGame *game = SC_PLAYER(self)->game;
		_MoveProposal *mp = tmp->data;

		g_printerr ("best(%d):  ", mp->move_rating);
		sc_simulator_run (sim, game, SC_PLAYER (self), &(mp->move),
				          sc_sim_player_simulation_finished_proxy, self);
		sc_simulator_free (sim);
	}

	for (tmp = g_list_last(sc_computer_player_get_stored_moves (SC_COMPUTER_PLAYER (self))), i = 0;
	     tmp && i < priv->moves_to_consider; tmp = tmp->next, i++) {
		ScSimulator *sim = sc_simulator_new ();
		ScGame *game = SC_PLAYER(self)->game;
		_MoveProposal *mp = tmp->data;

		g_printerr ("worst(%d): ", mp->move_rating);
		sc_simulator_run (sim, game, SC_PLAYER (self), &(mp->move),
				          sc_sim_player_simulation_finished_proxy, self);
		sc_simulator_free (sim);
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

