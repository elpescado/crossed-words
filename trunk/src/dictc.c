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



int main (int argc, char *argv[])
{
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
}
