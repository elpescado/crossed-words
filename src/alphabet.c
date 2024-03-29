#include <stdio.h>
#include <string.h>

#include "alphabet.h"

#define INITIAL_ALPHABET_SIZE 40
#define BUFFER_SIZE 4096
#define FIRST_LETTER_INDEX 1



Alphabet *
alphabet_new (void)
{
	Alphabet *al = g_new0 (Alphabet, 1);

	al->letters = g_new0 (Letter, INITIAL_ALPHABET_SIZE);
	al->letters_size = INITIAL_ALPHABET_SIZE;
	al->n_letters = 0;

	return al;
}


void
alphabet_free (Alphabet *al)
{
	g_free (al->letters);
	g_free (al);
}


/**
 * Load alphabet desctiption from file
 **/
gboolean
alphabet_load (Alphabet *al, const gchar *file_name)
{
	FILE *f;
	gchar buffer[BUFFER_SIZE];

	f = fopen (file_name, "r");
	if (f == NULL) {
		return FALSE;
	}

	while (fgets (buffer, BUFFER_SIZE, f) != NULL) {
		char label[32];
		char flags[32] = "\0";
		gint value;
		gint count;
		gint n_tokens = sscanf (buffer, "%s %d %d %s", label, &count, &value, flags);

		if (n_tokens >= 3) {
			Letter *l = &(al->letters[al->n_letters]);

			l->index = FIRST_LETTER_INDEX + al->n_letters;
			l->value = value;
			l->count = count;
			strncpy (l->label, label, 4);

			/* Parse flags */
			char *c;
			for (c = flags; *c; c++) {
				switch (*c) {
					case 'c': l->flags |= LETTER_CONSONANT; break;
					case 'v': l->flags |= LETTER_VOWEL; break;
					default: break;
				}
			}


			/*
			g_print ("LETTER %2d '%s' count = %d, value = %d points\n",
			         l->index, l->label, l->count, l->value);
			*/
			
			al->n_letters++;
		}
	}


	fclose (f);

	return TRUE;
}


Letter *
alphabet_lookup_letter (Alphabet *al, LID index)
{
	if (index == 0)
		return NULL;

	index =	sc_letter_value (index);
	return index == 0
		? NULL
		: &(al->letters[index - FIRST_LETTER_INDEX]);
}


LID
alphabet_translate_char (Alphabet *al, const gchar *utf8)
{
	int i;
	for (i = 0; i < al->n_letters; i++) {
		if (strcmp (al->letters[i].label, utf8) == 0)
			return al->letters[i].index;
	}
	return 0;
}


gboolean
alphabet_translate (Alphabet *al, const gchar *word, LID *out, gint *out_len)
{
	gchar *tmp = g_utf8_strup (word, -1);
	const gchar *c = tmp;
	int i = 0;
	while (c && *c) {
		gchar cbuf[4] = {0};
		g_utf8_strncpy(cbuf, c, 1);
		if (strcmp (cbuf, "?") == 0) {
			out[i-1] |= BLANK;
		} else {
			LID lid = alphabet_translate_char (al, cbuf);
			if (lid == 0)
				return FALSE;
			out[i++] = lid;
			//g_print ("alphabet_translate %s -> %d\n", cbuf, lid);
		}
		c = g_utf8_find_next_char (c, NULL);
	}
	*out_len = i;
	g_free (tmp);
	return TRUE;
}
