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

struct _ScSimulatorTask {
	ScGameState *game_state;
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
	for (i = 0; i < 100; i++) {
		sc_game_restore_state (game, st);

		sc_game_start (game);
		g_main_loop_run (loop);

		/* Summarize */
		int j;
		for (j = 0; j < N_PLAYERS; j++) {
			ScPlayer *player = sc_game_get_player (game, j);
			int score = sc_game_get_players_score (game, player);

			g_printerr ("%4d ", score);
		}
		g_printerr ("\n");
	}

	g_object_unref (game);
}


void
sc_simulator_run (ScSimulator *sim,
		          ScGame   *game,
                  ScPlayer *player,
				  ScMove   *move)
{
	ScSimulatorTask task;
	ScGameState *state = sc_game_save_state (game, player, move);

	task.game_state = state;

	sc_simulator_thread (sim, &state);
}


