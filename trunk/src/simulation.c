
#include <stdlib.h>
#include <time.h>

#include <glib.h>

#include "sc-game.h"
#include "sc-computer-player.h"


#define N_SIMULATIONS 1000
#define N_PLAYERS 2

int scores[N_SIMULATIONS][N_PLAYERS] = {0};
int current_sim = 0;
GMainLoop *loop = NULL;

static void
setup_game (void);


static void
silence (const gchar *str)
{
	// do nothing
}


static void
summarize (void)
{
	int i;
	for (i = 0; i < N_PLAYERS; i++) {
		gint sum = 0;
		gint max = 0;
		gint min = 10000000;

		int j;
		int n = N_SIMULATIONS;
		for (j = 0; j < n; j++) {
			sum += scores[j][i];
			max = MAX(scores[j][i], max);
			min = MIN(scores[j][i], min);
		}

		gint avg = sum/n;
		g_printerr ("Player %d: avg %3d max %3d min %3d points\n", i+1, avg, max, min);
	}
}


static void
game_finished (ScGame *game, gpointer data)
{
	g_printerr ("%3d: ", current_sim+1);

	int i;
	for (i = 0; i < N_PLAYERS; i++) {
		ScPlayer *p = sc_game_get_player (game, i);
		int score = sc_game_get_players_score (game, p);
		g_printerr ("%4d ", score);

		scores[current_sim][i] = score;
	}
	g_printerr ("\n");

	g_object_unref (game);

	if (++current_sim >= N_SIMULATIONS) {
		g_main_loop_quit (loop);
	} else {
		setup_game ();
	}
}


static void
setup_game (void)
{
	ScGame *game = sc_game_new ();

	ScPlayer *p1 = sc_computer_player_new ();
	p1->game = game;
	sc_computer_player_enable_exchange (SC_COMPUTER_PLAYER (p1), TRUE);
	sc_game_set_player (game, 0, p1);

	ScPlayer *p2 = sc_computer_player_new ();
	p2->game = game;
	sc_game_set_player (game, 1, p2);


	g_signal_connect (game, "end", G_CALLBACK (game_finished), NULL);
	sc_game_start (game);
}


static void
setup_simulation (void)
{
	setup_game ();
}




int main (int argc, char *argv[])
{
	g_type_init ();
	srand (time (NULL));

	g_set_print_handler (silence);

	loop = g_main_loop_new (NULL, FALSE);

	setup_simulation ();

	g_main_loop_run (loop);

	summarize ();
	
	return EXIT_SUCCESS;
}