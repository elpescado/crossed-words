#include "sc-dawg.h"
#include "sc-dsf.h"
#include "sc-dag2.h"


static void
sc_dawg_init (ScDawg *dawg,
              gint n_vertices,
              gint n_arcs)
{
	dawg->vertices    = g_new (ScDawgVertex, n_vertices);
	dawg->n_vertices = n_vertices;

	dawg->arcs   = g_new (ScDawgArc, n_arcs);
	dawg->n_arcs = n_arcs;

	dawg->ai = dawg->vi = 0;
}


ScDawg *
sc_dawg_new (gint n_vertices,
             gint n_arcs)
{
	ScDawg *dawg = g_new0 (ScDawg, 1);

	sc_dawg_init (dawg, n_vertices, n_arcs);

	return dawg;
}


void
sc_dawg_free (ScDawg *dawg)
{
	g_free (dawg->vertices);
	g_free (dawg->arcs);
	g_free (dawg);
}


static void
sc_dawg_header_cb (ScDsfReader *reader, struct ScDsfHeader *header, gpointer user_data)
{
	g_print ("got_header\n");
	ScDawg *dawg = (ScDawg*)user_data;

	sc_dawg_init (dawg, header->n_vertices, header->n_arcs);
}


static void
sc_dawg_vertex_cb (ScDsfReader *reader, struct ScDsfVertex *vertex, gpointer user_data)
{
	ScDawg *dawg = (ScDawg*)user_data;

	ScDawgVertex *v = dawg->vertices + dawg->vi++;
	v->first_arc = dawg->arcs + dawg->vai;
	v->flags     = vertex->flags;
	v->n_arcs    = vertex->n_arcs;
	dawg->vai += vertex->n_arcs;
}


static void
sc_dawg_arc_cb    (ScDsfReader *reader, struct ScDsfArc    *arc,    gpointer user_data)
{
	ScDawg *dawg = (ScDawg*)user_data;

	ScDawgArc *a = dawg->arcs + dawg->ai++;
	a->lid  = arc->lid;
	a->dest = dawg->vertices + arc->dest;
}


ScDawg *
sc_dawg_load (const gchar *file_name)
{
	g_print ("load(%s)\n", file_name);
	ScDsfReader *reader = sc_dsf_reader_open (file_name);

	ScDawg *dawg = g_new0 (ScDawg, 1);
	struct ScDsfReaderCtx ctx = {
		sc_dawg_header_cb,
		sc_dawg_vertex_cb,
		sc_dawg_arc_cb
	};

	if (! sc_dsf_reader_read (reader, &ctx, dawg)) {
		g_print ("Error reading dawg\n");
		g_free (dawg);
		sc_dsf_reader_close (reader);
		return NULL;
	}
	sc_dsf_reader_close (reader);
	
	return dawg;
}


gboolean
sc_dawg_save (ScDawg *dawg, const gchar *file_name)
{
	/* Not implemented */
	return FALSE;
}


gboolean
sc_dawg_test_word (ScDawg *self, const gchar *word, Alphabet *al)
{
	glong len = g_utf8_strlen (word, -1);
	if (len > 15) {
		g_print ("too long, discarding: '%s'\n", word);
		return FALSE;
	}

	LID letters[15];
	if (! alphabet_translate (al, word, letters))
		return FALSE;

	return sc_dawg_test_word_translated (self, letters, len);
}

ScDawgVertex *
sc_dawg_vertex_child (const ScDawgVertex *vertex, LID lid)
{
	int i;
	for (i = 0; i < vertex->n_arcs; i++) {
		ScDawgArc *arc = vertex->first_arc + i;
		if (arc->lid == lid) {
			return arc->dest;
		}
	}

	return NULL;
}



gboolean
sc_dawg_test_word_translated (ScDawg *self, LID *letters, glong len)
{
	ScDawgVertex *vert = sc_dawg_root (self);

	//g_print ("Testing ");

	gint i;
	for (i = 0; i < len; i++) {
		LID lid = letters[i];
		vert = sc_dawg_vertex_child (vert, lid);
	//	g_print (" -> %d", vert - self->vertices);
		if (vert == NULL) {
	//		g_print ("FAIL\n");
			return FALSE;
		}
	}
	//g_print ("\n");

	return vert->flags & SC_DAG2_NODE_FINAL;
}


void
sc_dawg_print (ScDawg *self, Alphabet *al)
{
	int i;
	for (i = 0; i < self->n_vertices; i++) {
		ScDawgVertex *v = self->vertices + i;
		g_print ("%3d %02x: ", i, (int)v->flags);
		int j;


		for (j = 0; j < v->n_arcs; j++) {
			ScDawgArc *a = v->first_arc + j;

			Letter *l = alphabet_lookup_letter (al, a->lid);
			g_print ("%s -> %d   ", l ? l->label : "_", (int)(a->dest - self->vertices));
		}
		g_print ("\n");

	}
}


ScDawgVertex *
sc_dawg_root (ScDawg *self)
{
	return self->vertices + self->n_vertices - 1;
}

