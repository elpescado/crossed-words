#include <stdlib.h>
#include <time.h>

#include <gtk/gtk.h>

#include "alphabet.h"
#include "letter.h"
#include "sc-board.h"
#include "sc-dag.h"
#include "sc-dag2.h"
#include "sc-dawg.h"
#include "sc-game.h"
#include "sc-move.h"
#include "sc-bag.h"
#include "scx-board-view.h"
#include "scx-move-entry.h"
#include "scx-rack-view.h"
#include "scx-main-window.h"
#include "util.h"

#define DICT_FILE "lang/pl/dictionary.txt"

typedef gboolean (*DictionaryTestFunc) (void *, LID *, gint);

void test_word (ScDag2 *dag, Alphabet *al, const gchar *word)
{
	g_print (" -> testing '%s' -> %s\n", word,
			sc_dag2_test_word (dag, word, al) ? "true" : "false");
}


void test_dictionary (void *dict, Alphabet *al, const gchar *file_name, DictionaryTestFunc func, gboolean expected_result)
{
	return;
	g_print ("test dictionary: ");	
	FILE *f = fopen (file_name, "r");
	if (f == NULL) {
		return;
	}

	gchar buffer[128];

	gint missed = 0;
	gint n_words = 0;

	LID letters[15];

	double t0 = foo_microtime ();
	while (fgets (buffer, 128, f)) {
		char *word = g_strstrip (buffer);

		if (!alphabet_translate (al, word, letters))
			continue;

		glong len = g_utf8_strlen (word, -1);

		n_words++;
		if (func (dict, letters, len) != expected_result) {
		//	g_print ("    Word '%s' not present\n", word);
			missed++;
		}
	}
	double t1 = foo_microtime ();

	fclose (f);	

	g_print ("%d tested, %d missed, %lf seconds, %lf%% words lost\n", n_words, missed, t1 - t0, 100*(double)missed/(double)n_words);
}

/*
void test_bad_dictionary (ScDag2 *dag, Alphabet *al, const gchar *file_name)
{
	g_print ("test dictionary: ");	
	FILE *f = fopen (file_name, "r");
	if (f == NULL) {
		return;
	}

	gchar buffer[128];

	gint missed = 0;
	gint n_words = 0;

	LID letters[15];

	double t0 = foo_microtime ();
	while (fgets (buffer, 128, f)) {
		char *word = g_strstrip (buffer);

		if (!alphabet_translate (al, word, letters))
			continue;

		glong len = g_utf8_strlen (word, -1);

		n_words++;
		if (sc_dag2_test_word_translated (dag, letters, len)) {
		//	g_print ("    Word '%s' present\n", word);
			missed++;
		}
	}
	double t1 = foo_microtime ();

	fclose (f);	

	g_print ("%d tested, %d missed, %lf seconds, %lf%% words lost\n", n_words, missed, t1 - t0, 100*(double)missed/(double)n_words);
}
*/


int main (int argc, char *argv[])
{
	GtkWidget *main_window;
	/*
	GtkWidget *board_view;
	GtkWidget *move_entry;
	GtkWidget *main_window2;
	GtkWidget *main_window3;
	Alphabet  *al;
	ScBoard   *board;
	ScGame    *game;
	ScMove     move;
	ScRack    *rack;
	GtkWidget *rack_view;
	ScBag     *bag;
	ScPlayer  *p1;
	ScPlayer  *p2;
	*/
	Alphabet  *al;
	ScDag2 *dag;
	ScDawg *dawg;

	gtk_init (&argc, &argv);
	srand (time (NULL));

	al = alphabet_new ();
	alphabet_load (al, "lang/pl/alphabet.txt");
#if 1
	dag = sc_dag2_new ();
	/*
	sc_dag2_add_drowword (dag, "COPY", al);
	sc_dag2_add_drowword (dag, "ATLAS", al);
	sc_dag2_add_drowword (dag, "ABCDEF", al);
	sc_dag2_add_drowword (dag, "WARYJAT", al);
	sc_dag2_add_drowword (dag, "KWIATOSTAN", al);
	sc_dag2_add_drowword (dag, "ABDOMINOPLASTYK", al);
	return 0;
	*/

	double t0 = foo_microtime ();
	//sc_dag2_load_file (dag, "test.txt", al);
	sc_dag2_load_file (dag, DICT_FILE, al, atoi(argv[1]));
	double t1 = foo_microtime ();

	test_word (dag, al, "alfabet");
	test_word (dag, al, "asdaf");
	test_dictionary (dag, al, DICT_FILE, sc_dag2_test_word_translated, TRUE);
	test_dictionary (dag, al, "bad-words.txt", sc_dag2_test_word_translated, FALSE);

	sc_dag2_print_stats (dag);
	sc_dag2_minimize (dag);
	sc_dag2_save (dag, "dictionary.dag");
	g_print ("Construction took %lf secs\n", t1 - t0);

	test_word (dag, al, "alfabet");
	test_word (dag, al, "asdaf");
	/*
	test_dictionary (dag, al, DICT_FILE);
	test_bad_dictionary (dag, al, "bad-words.txt");
	*/
	test_dictionary (dag, al, DICT_FILE, sc_dag2_test_word_translated, TRUE);
	test_dictionary (dag, al, "bad-words.txt", sc_dag2_test_word_translated, FALSE);
#endif
	g_print (" -- DAWG -- \n");
	dawg = sc_dawg_load ("dictionary.dag");
	if (dawg == NULL) {
		g_print ("dawg == NULL\n");
		return EXIT_FAILURE;
	}
	//sc_dawg_print (dawg, al);
	test_dictionary (dawg, al, DICT_FILE, sc_dawg_test_word_translated, TRUE);
	test_dictionary (dawg, al, "bad-words.txt", sc_dawg_test_word_translated, FALSE);

	return 0;
	/*
	al = alphabet_new ();
	alphabet_load (al, "lang/pl/alphabet.txt");
	LID word[15];
	alphabet_translate (al, "Dzierżążno", word);

	board = sc_board_new (al);
	board_view = scx_board_view_new ();
	scx_board_view_set_board (SCX_BOARD_VIEW (board_view), board);
	
	game = sc_game_new ();
	sc_game_set_board (game, board);
	sc_game_init_move (game, &move, 5, 4, SC_VERTICAL, "pescado");
//	scx_board_view_set_move (board_view, &move);
//	sc_board_place_move (board, &move);

	p1 = sc_player_new ();
	sc_game_set_player (game, 0, p1);

	p2 = sc_player_new ();
	sc_game_set_player (game, 1, p2);

	sc_game_start (game);

	sc_player_do_move (p1, &move);

	main_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_container_add (GTK_CONTAINER (main_window), board_view);
	gtk_widget_show_all (main_window);

	move_entry = scx_move_entry_new ();

	main_window2 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_container_add (GTK_CONTAINER (main_window2), move_entry);

	gtk_widget_show (move_entry);
	gtk_widget_show (main_window2);

	bag = sc_bag_new ();
	sc_bag_load (bag, al);

	rack = sc_rack_new (al);
	sc_game_fill_rack (game, rack, bag);
	*/
	/*
	sc_rack_add_tile (rack, 7);
	sc_rack_add_tile (rack, 9);
	sc_rack_add_tile (rack, 3);
	sc_rack_add_tile (rack, 6);
	sc_rack_add_tile (rack, 8);
	sc_rack_add_tile (rack, 8);
	*/

	/*
	rack_view = scx_rack_view_new ();
	scx_rack_view_set_rack (rack_view, rack);

	main_window3 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_container_add (GTK_CONTAINER (main_window3), rack_view);

	gtk_widget_show (rack_view);
	gtk_widget_show (main_window3);

	*/
	
	main_window = scx_main_window_new ();
	gtk_widget_show (main_window);
	g_signal_connect (G_OBJECT (main_window), "delete-event", G_CALLBACK (gtk_main_quit), NULL);
	

	//g_signal_connect (G_OBJECT (main_window), "delete-event", G_CALLBACK (gtk_main_quit), NULL);

	g_print ("gtk_main()\n");
	gtk_main ();

	//alphabet_free (al);

	return EXIT_SUCCESS;
}
