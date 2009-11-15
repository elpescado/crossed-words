#ifndef __SC_DAWG_H__
#define __SC_DAWG_H__

#include <glib.h>

#include "alphabet.h"

typedef struct ScDawg       ScDawg;
typedef struct ScDawgVertex ScDawgVertex;
typedef struct ScDawgArc    ScDawgArc;




struct ScDawgVertex {
	ScDawgArc *first_arc;
	guint8     flags;
	guint8    _unused;
	guint16    n_arcs;
};


struct ScDawgArc {
	LID           lid;	/* Letter ID */
	ScDawgVertex *dest;	/* Destination vertex */
};


struct ScDawg {
	ScDawgVertex *vertices;
	guint         n_vertices;

	ScDawgArc    *arcs;
	guint         n_arcs;

	guint ai;
	guint vi;
	guint vai;
};



ScDawg *
sc_dawg_new (gint n_verices,
             gint n_arcs);


void
sc_dawg_free (ScDawg *dawg);


ScDawg *
sc_dawg_load (const gchar *file_name);


gboolean
sc_dawg_save (ScDawg *dawg, const gchar *file_name);


gboolean
sc_dawg_test_word (ScDawg *self, const gchar *word, Alphabet *al);


gboolean
sc_dawg_test_word_translated (ScDawg *self, LID *letters, glong len);

		
void
sc_dawg_print (ScDawg *self, Alphabet *al);


ScDawgVertex *
sc_dawg_root (ScDawg *self);



#endif /* __SC_DAWG_H__ */
