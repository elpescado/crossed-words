#include "sc-dawg.h"
#include "sc-dsf.h"


static void
sc_dawg_init (ScDawg *dawg,
              gint n_vertices,
              gint n_arcs)
{
	dawg->vertices    = g_new (ScDawgVertex, n_vertices);
	dawg->n_vertices = n_vertices;

	dawg->arcs   = g_new (ScDawgArc, n_arcs);
	dawg->n_arcs = n_arcs;
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
	ScDawg *dawg = (ScDawg*)user_data;

	sc_dawg_init (dawg, header->n_vertices, header->n_arcs);
}


static void
sc_dawg_vertex_cb (ScDsfReader *reader, struct ScDsfVertex *vertex, gpointer user_data)
{
	ScDawg *dawg = (ScDawg*)user_data;

	
}


static void
sc_dawg_arc_cb    (ScDsfReader *reader, struct ScDsfArc    *arc,    gpointer user_data)
{
	ScDawg *dawg = (ScDawg*)user_data;
}


ScDawg *
sc_dawg_load (const gchar *file_name)
{
	ScDsfReader *reader = sc_dsf_reader_open (file_name);

	ScDawg *dawg = g_new0 (ScDawg, 1);
	struct ScDsfReaderCtx ctx = {
		sc_dawg_header_cb,
		sc_dawg_vertex_cb,
		sc_dawg_arc_cb
	};

	sc_dsf_reader_read (reader, &ctx, dawg);
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
	return FALSE;
}


gboolean
sc_dawg_test_word_translated (ScDawg *self, LID *letters, glong len)
{
	return FALSE;
}


