/*
 * Scrabble
 * --------
 *
 * Copyright (C) 2009 Przemysław Sitek
 * 
 */

#include "sc-simulator.h"

#include "sc-computer-player.h"


#define N_PLAYERS 2
#define N_SIMULATIONS 5

struct _ScSimulatorTask {
	ScGameState *game_state;
	gint         scores[N_SIMULATIONS][N_PLAYERS];
	gint         avg[N_PLAYERS];
	gint         wins[N_PLAYERS];

	ScSimulatorFunc callback;
	gpointer        callback_data;

	gint            ref_count;
};

struct _ScSimulator {
};


ScSimulator *
sc_simulator_new (void)
{
	ScSimulator *sim = g_new0 (ScSimulator, 1);

	return sim;
}


void
sc_simulator_free (ScSimulator *sim)
{
	g_free (sim);
}


static void
sc_simulator_thread (gpointer data,
                     gpointer user_data)
{
	ScSimulator *sim = (ScSimulator*) data;
	ScSimulatorTask *task = (ScSimulatorTask*) user_data;

	sc_simulator_task_ref (task);

	GMainContext *ctx = g_main_context_new ();
	GMainLoop *loop = g_main_loop_new (ctx, FALSE);

	ScGame *game = sc_game_new (ctx);
	g_signal_connect_swapped (game, "end", G_CALLBACK(g_main_loop_quit), loop);
	ScGameState *st = task->game_state;

	ScPlayer *p1 = SC_PLAYER (sc_computer_player_new ());
	p1->game = game;
	sc_game_set_player (game, 0, p1);

	ScPlayer *p2 = SC_PLAYER (sc_computer_player_new ());
	p2->game = game;
	sc_game_set_player (game, 1, p2);


	int i;
	for (i = 0; i < N_SIMULATIONS; i++) {
		sc_game_restore_state (game, st);

		sc_game_start (game);
		g_main_loop_run (loop);

		/* Summarize */
		int j;
//		g_printerr (" * ");
		for (j = 0; j < N_PLAYERS; j++) {
			ScPlayer *player = sc_game_get_player (game, j);
			int score = sc_game_get_players_score (game, player);

			task->scores[i][j] = score;

//			g_printerr ("%4d ", score);
		}
//		g_printerr ("\n");
	}

	/* Count totals */
	int j;
	for (j = 0; j < N_PLAYERS; j++) {
		for (i = 0; i < N_SIMULATIONS; i++) {
			task->avg[j] += task->scores[i][j];
		}
		task->avg[j] /= N_SIMULATIONS;
	}

	for (i = 0; i < N_SIMULATIONS; i++) {
		if (task->scores[i][0] > task->scores[i][1])
			task->wins[0]++;
		if (task->scores[i][1] > task->scores[i][0])
			task->wins[1]++;
	}

	g_main_context_unref (ctx);
	g_object_unref (game);
	g_main_loop_unref (loop);

	if (task->callback) {
		task->callback (sim, task, task->callback_data);
	}

	sc_simulator_task_unref (task);
}


void
sc_simulator_run (ScSimulator    *sim,
		          ScGame         *game,
                  ScPlayer       *player,
				  ScMove         *move,
				  ScSimulatorFunc callback,
				  gpointer        user_data)
{
	ScSimulatorTask *task = g_new0 (ScSimulatorTask, 1);
	ScGameState *state = sc_game_save_state (game, player, move);

	task->game_state = state;
	task->callback = callback;
	task->callback_data = user_data;
	task->ref_count = 1;

	sc_simulator_thread (sim, task);

	g_printerr (" scores -> %3d vs %3d = %d ", task->avg[0], task->avg[1],
			 task->avg[0] - task->avg[1]);
	g_printerr (" | wins -> %3d vs %3d = %d \n", task->wins[0], task->wins[1],
			 task->wins[0] - task->wins[1]);

	sc_simulator_task_unref (task);
}



ScSimulatorTask *
sc_simulator_task_ref (ScSimulatorTask *task)
{
	task->ref_count++;
	return task;
}


void
sc_simulator_task_unref (ScSimulatorTask *task)
{
	if (--task->ref_count == 0) {
		g_printerr ("Freeing task\n");
		g_free (task);
	}
}

