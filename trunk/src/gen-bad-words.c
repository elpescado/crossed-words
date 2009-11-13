#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <gtk/gtk.h>

#include "alphabet.h"
#include "letter.h"
#include "sc-board.h"
#include "sc-dag.h"
#include "sc-game.h"
#include "sc-move.h"
#include "sc-bag.h"
#include "scx-board-view.h"
#include "scx-move-entry.h"
#include "scx-rack-view.h"
#include "scx-main-window.h"
#include "util.h"

#define MAX_LEN 15


const char *dictionary_file = "lang/pl/dictionary.txt";


/**
 * Generate file with random words not belonging to dictionary
 **/
void
generate_bad_words (ScDag *allowed_words, Alphabet *al, gint n_words, const gchar *file_name)
{
	FILE *f = fopen (file_name, "w");

	while (n_words > 0) {
		int l = 3 + rand()%11;
		char word[MAX_LEN*3];
		word[0] = '\0';

		int i;
		for (i = 0; i < l; i++) {
			LID lid = 1 + (rand() % al->n_letters);
			Letter *l = alphabet_lookup_letter (al, lid);
			strcat (word, l->label);
		}
		//word[i] = '\0';

		if (! sc_dag_test_word (allowed_words, word, al)) {
			fprintf (f, "%s\n", word);
			n_words--;
		}
	}

	fclose (f);
}


int main (int argc, char *argv[])
{
	GtkWidget *main_window;

	Alphabet  *al;
	ScDag *dag;
	al = alphabet_new ();
	alphabet_load (al, "lang/pl/alphabet.txt");

	dag = sc_dag_new ();
	sc_dag_load_file (dag, dictionary_file, al);

	generate_bad_words (dag, al, 4000000, "bad-words.txt");

	return EXIT_SUCCESS;
}
