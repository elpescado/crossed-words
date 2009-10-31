#ifndef __ALPHABET_H__
#define __ALPHABET_H__


#include <glib.h>

#include "letter.h"



typedef struct {
	Letter *letters;
	gint    letters_size;
	gint    n_letters;
} Alphabet;



Alphabet *
alphabet_new (void);


void
alphabet_free (Alphabet *al);


gboolean
alphabet_load (Alphabet *al, const gchar *file_name);


Letter *
alphabet_lookup_letter (Alphabet *al, LID index);


gboolean
alphabet_translate (Alphabet *al, const gchar *word, LID *out);


#endif /* __ALHPABET_H__ */
