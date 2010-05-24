
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <signal.h>
#include <math.h>
#include <string.h>

#include <glib.h>

#include "sc-game.h"
#include "sc-computer-player.h"
#include "sc-noob-player.h"
#include "sc-pro-player.h"


#define N_SIMULATIONS 1000
#define N_PLAYERS 2

int scores[N_SIMULATIONS][N_PLAYERS] = {{0}};
int current_sim = 0;
GMainLoop *loop = NULL;
gboolean silent = TRUE;

static gint player_flags[2] = {0};

#define player_n(sim, i) ((sim+i)%N_PLAYERS)

static void
setup_game (void);


static void
toggle_silence (int num)
{
	silent = !silent;
}


static void
silence (const gchar *str)
{
	if (!silent) {
		fputs (str, stdout);
		fflush (stdout);
	}
}


static void
summarize (void)
{
	int i,j;
	int n = N_SIMULATIONS;

	gint victories[N_PLAYERS];
	memset (victories, 0, sizeof (victories));
	gint draws = 0;

	for (j = 0; j < n; j++) {
		/* TODO: this works only for two players */
		if (scores[j][0] > scores[j][1])
			victories[0]++;

		else if (scores[j][1] > scores[j][0])
			victories[1]++;

		else
			draws++;
//		for (i = 0; i < N_PLAYERS; i++) {

//		}
	}

	for (i = 0; i < N_PLAYERS; i++) {
		gint sum = 0;
		gint max = 0;
		gint min = 10000000;

		int j;
		for (j = 0; j < n; j++) {
			sum += scores[j][i];
			max = MAX(scores[j][i], max);
			min = MIN(scores[j][i], min);
		}

		gint avg = sum/n;

		double var = 0;
		for (j = 0; j < n; j++) {
			gint d = avg - scores[j][i];
			var += d*d;
		}
		var /= (n-1);
		double sd = sqrt (var);


		g_printerr ("Player %d: %d victories, avg %3d max %3d min %3d var %10.3lf sd %8.3lf points\n",
				i+1, victories[i],
				avg, max, min,
				var, sd);
	}

	extern gint n_bingos;
	g_printerr ("draws %d, total bingos %d\n", draws, n_bingos);

	if (draws + victories[0] + victories[1] != n)
		g_printerr (":-/\n");
}


static void
save_csv (const gchar *filename)
{
	FILE *f = fopen (filename, "w");
	if (f == NULL)
		return;

	int j;
	int n = N_SIMULATIONS;
	int i;
	for (j = 0; j < n; j++) {
		for (i = 0; i < N_PLAYERS; i++) {
			fprintf (f, "%d%s", scores[j][i], i == N_PLAYERS-1? "\n": ", ");
		}
	}
	fclose (f);
}


static void
game_finished (ScGame *game, gpointer data)
{
	//g_printerr ("%5d: ", current_sim+1);

	int i;
	for (i = 0; i < N_PLAYERS; i++) {
		ScPlayer *p = sc_game_get_player (game, player_n(current_sim,i));
		int score = sc_game_get_players_score (game, p);
		//g_printerr ("%4d ", score);

		scores[current_sim][i] = score;

		//g_printerr ("%s(%d) -> %d\n", sc_player_get_name (p), score, i);
	}
	//g_printerr ("\n");

	g_object_unref (game);

	if (++current_sim >= N_SIMULATIONS) {
		g_main_loop_quit (loop);
	} else {
		setup_game ();
	}
}


static gint
parse_desc (const gchar *desc)
{
	gchar **tokens;
	gint i;

	tokens = g_strsplit (desc, ",", 0);

	gint flags = 0;

	for (i = 1; tokens[i]; i++) {
		if (strcmp (tokens[i], "exchange") == 0) {
			flags |= SC_EXCHANGE_TILES;
		} else if (strcmp (tokens[i], "rack") == 0) {
			flags |= SC_CONSIDER_RACK_LEAVE;
		} else if (strcmp (tokens[i], "endgame") == 0) {
			flags |= SC_ENDGAME_HINT;
		} else {
			g_printerr ("Invalid token '%s'\n", tokens[i]);
			exit (1);
		}

	}

	g_strfreev (tokens);

	return flags;
}


static void
setup_game (void)
{
	ScGame *game = sc_game_new ();

	ScPlayer *p1 = SC_PLAYER (sc_computer_player_new ());
	//ScPlayer *p1 = create_player (p1d);
	sc_player_set_name (p1, "Ambroży");
	p1->game = game;
	sc_game_set_player (game, player_n(current_sim,0), p1);

	sc_computer_player_set_hints (SC_COMPUTER_PLAYER (p1), player_flags[0]);

	//sc_computer_player_enable_exchange (SC_COMPUTER_PLAYER (p1), TRUE);

	ScPlayer *p2 = SC_PLAYER (sc_computer_player_new ());
	//ScPlayer *p2 = create_player (p2d);
	sc_player_set_name (p2, "Barnaba");
	p2->game = game;
	sc_game_set_player (game, player_n(current_sim,1), p2);

	sc_computer_player_set_hints (SC_COMPUTER_PLAYER (p2), player_flags[1]);

	//sc_computer_player_set_hint (SC_COMPUTER_PLAYER (p2), SC_CONSIDER_RACK_LEAVE, TRUE);
	//sc_computer_player_enable_exchange (SC_COMPUTER_PLAYER (p2), TRUE);


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

	player_flags[0] = parse_desc (argv[1]);
	player_flags[1] = parse_desc (argv[2]);

	g_set_print_handler (silence);
	signal (SIGUSR1, toggle_silence);

	loop = g_main_loop_new (NULL, FALSE);

	setup_simulation ();

	g_main_loop_run (loop);

	summarize ();
	save_csv ("results.csv");
	
	return EXIT_SUCCESS;
}
