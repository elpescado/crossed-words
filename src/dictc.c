#include <stdlib.h>
#include <time.h>

#include <gtk/gtk.h>
#include <glib.h>

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

#define verbose(args...) {if(opt_verbose)g_print(args);}

/* Command line options */

gchar   *opt_alphabet_file = NULL;
gint     opt_max_length = 15;
gboolean opt_gaddag = FALSE;
gboolean opt_verify = FALSE;
gboolean opt_verbose = FALSE;


static GOptionEntry entries[] = {
	{"alphabet",   'a', 0, G_OPTION_ARG_STRING, &opt_alphabet_file, "Alphabet file", "FILE"},
	{"max-length", 'm', 0, G_OPTION_ARG_INT,    &opt_max_length,    "Max word length", "N"},
	{"gaddag",     'g', 0, G_OPTION_ARG_NONE,   &opt_gaddag,        "Build graph as GADDAG", NULL},
	{"verify",     'v', 0, G_OPTION_ARG_NONE,   &opt_verify,        "Verify results", NULL},
	{"verbose",    'V', 0, G_OPTION_ARG_NONE,   &opt_verbose,       "Verbose output", NULL},
	{NULL}
};


typedef gboolean (*DictionaryTestFunc) (void *, LID *, gint);

void test_word (ScDag2 *dag, Alphabet *al, const gchar *word)
{
	g_print (" -> testing '%s' -> %s\n", word,
			sc_dag2_test_word (dag, word, al) ? "true" : "false");
}


gboolean
test_dictionary (void *dict, Alphabet *al, const gchar *file_name, DictionaryTestFunc func, gboolean expected_result)
{
	FILE *f = fopen (file_name, "r");
	if (f == NULL) {
		return FALSE;
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
		if (len > opt_max_length)
			continue;

		n_words++;
		if (func (dict, letters, len) != expected_result) {
		//	g_print ("    Word '%s' not present\n", word);
			missed++;
		}
	}
	double t1 = foo_microtime ();

	fclose (f);

	g_print ("%d of %d missed, %lf seconds, %d%% words lost\n", missed, n_words, t1 - t0, 100*missed/n_words);
	if (expected_result == TRUE)
		return missed == 0;
	else
		return missed == n_words;
}

gboolean
load_file (ScDag2 *self, const gchar *file_name, Alphabet *al)
{
	FILE *f = fopen (file_name, "r");
	if (f == NULL) {
		return FALSE;
	}

	gchar buffer[128];

	while (fgets (buffer, 128, f)) {
		char *word = g_strstrip (buffer);
		if (g_utf8_strlen (word, -1) > opt_max_length)
			continue;

		if (opt_gaddag)
			sc_dag2_add_drowword (self, word, al);
		else
			sc_dag2_add_word (self, word, al);

		//if (--max == 0)
		//	break;
	}

	fclose (f);

	return TRUE;
}




int main (int argc, char *argv[])
{
	GError *error = NULL;
	GOptionContext *context;

	Alphabet  *al;
	ScDag2 *dag;
	ScDawg *dawg;

	gtk_init (&argc, &argv);
	srand (time (NULL));

	/* Parse command line options */
	context = g_option_context_new ("- dictionary files compiler");
	g_option_context_add_main_entries (context, entries, "dictc");
	g_option_context_add_group (context, gtk_get_option_group (TRUE));
	if (!g_option_context_parse (context, &argc, &argv, &error)) {
		g_printerr ("option parsing failed: %s\n", error->message);
		return EXIT_FAILURE;
	}

	if (opt_alphabet_file == NULL || argc < 3) {
		g_printerr ("Bad arguments\n");
		return EXIT_FAILURE;
	}
	const gchar *dict_file = argv[1];
	const gchar *out_file = argv[2];

	
	/* Load alphabet */

	al = alphabet_new ();
	alphabet_load (al, opt_alphabet_file);


	/* Load input file */

	dag = sc_dag2_new ();

	verbose ("Loading dictionary file...");
	double t0 = foo_microtime ();
	double t1;
	if (load_file (dag, dict_file, al)) {
		t1 = foo_microtime ();
		verbose (" ok\n");
	} else {
		verbose (" failed\n");
		return EXIT_FAILURE;
	}

	if (opt_verify) {
		verbose ("Verifying loaded words... ");
		if (!test_dictionary (dag, al, dict_file, (DictionaryTestFunc)sc_dag2_test_word_translated, TRUE)) {
			g_printerr ("Verification failed\n");
			return EXIT_FAILURE;
		}
		//test_dictionary (dag, al, "bad-words.txt", sc_dag2_test_word_translated, FALSE);
	}

	if (opt_verbose) {
		sc_dag2_print_stats (dag);
	}

	/* Minimize graph */

	verbose ("Minimizing graph...");
	sc_dag2_minimize (dag);
	verbose (" ok\n");


	/* Verify minimized graph */

	if (opt_verify) {
		verbose ("Verifying minimized graph... ");
		if (!test_dictionary (dag, al, dict_file, (DictionaryTestFunc)sc_dag2_test_word_translated, TRUE)) {
			g_printerr ("Verification failed\n");
			return EXIT_FAILURE;
		}
		//test_dictionary (dag, al, "bad-words.txt", sc_dag2_test_word_translated, FALSE);
	}


	/* Save binary file */

	verbose ("Saving output file...");
	if (!sc_dag2_save (dag, out_file)) {
		verbose (" failed\n");
		g_printerr ("Cannot write output file\n");
	}
	verbose (" ok\n");


	/* Verify written file */

	verbose ("Verifying output file...");
	dawg = sc_dawg_load (out_file);
	if (dawg == NULL) {
		verbose (" failed\n");
		g_printerr ("Cannot load output file '%s' for verification\n", out_file);
		return EXIT_FAILURE;
	}
	if (!test_dictionary (dawg, al, dict_file, (DictionaryTestFunc)sc_dawg_test_word_translated, TRUE)) {
		verbose (" failed\n");
		g_printerr ("Output file verification failed\n");
		return EXIT_FAILURE;
	}
	//test_dictionary (dawg, al, "bad-words.txt", sc_dawg_test_word_translated, FALSE);

	verbose (" ok\n");


	return EXIT_SUCCESS;
}
