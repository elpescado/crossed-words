#include <stdio.h>
#include <string.h>

#include "sc-dsf.h"

struct ScDsfWriter {
	FILE *fh;
};

struct ScDsfReader {
	FILE *fh;
};


/**
 * Prepare header structore to be written to disk
 **/
static void
sc_dsf_writer_init_header (ScDsfWriter        *writer,
                           struct ScDsfHeader *header,
                           guint               n_vertices,
						   guint               n_arcs)
{
	strncpy (header->magic, "DAWG", 4);
	header->version_maj = 1;
	header->version_min = 0;
	header->header_len = GUINT16_TO_LE (sizeof (struct ScDsfHeader));

	header->n_vertices = GUINT32_TO_LE (n_vertices);
	header->n_arcs = GUINT32_TO_LE (n_arcs);
}


/**
 * Create a graph writer
 **/
ScDsfWriter *
sc_dsf_writer_open (const gchar *file_name,
                    guint n_vertices,
                    guint n_arcs)
{
	FILE *f = fopen (file_name, "w");

	if (f == NULL)
		return NULL;

	ScDsfWriter *writer = g_new (ScDsfWriter, 1);
	writer->fh = f;

	struct ScDsfHeader header;
	sc_dsf_writer_init_header (writer, &header, n_vertices, n_arcs);

	fwrite (&header, sizeof (header), 1, writer->fh);

	return writer;
}


/**
 * Close a writer
 **/
gboolean
sc_dsf_writer_close (ScDsfWriter        *writer)
{
	fclose (writer->fh);
	g_free (writer);
	return TRUE;
}


void
sc_dsf_writer_write_vertex (ScDsfWriter        *writer,
                            const struct ScDsfVertex *vertex)
{
	struct ScDsfVertex v;
	v.n_arcs = GUINT32_TO_LE (vertex->n_arcs);
	v.first_arc_idx = GUINT32_TO_LE (vertex->first_arc_idx);

	fwrite (&v, sizeof (v), 1, writer->fh);
}
 

void
sc_dsf_writer_write_arc (ScDsfWriter        *writer,
                         const struct ScDsfArc    *arc)
{
	struct ScDsfArc a;
	a.lid     = GUINT16_TO_LE (arc->lid);
	a.dest    = GUINT32_TO_LE (arc->dest);

	fwrite (&a, sizeof(a), 1, writer->fh);
}




ScDsfReader *
sc_dsf_reader_open (const gchar *file_name)
{
	FILE *f = fopen (file_name, "w");

	if (f == NULL)
		return NULL;

	ScDsfReader *reader = g_new (ScDsfReader, 1);
	reader->fh = f;

	return reader;
}


gboolean
sc_dsf_reader_close (ScDsfReader *reader)
{
	fclose (reader->fh);
	g_free (reader);

	return TRUE;
}


gboolean
sc_dsf_reader_read (ScDsfReader           *reader,
                    struct ScDsfReaderCtx *ctx,
                    gpointer               user_data)
{
	struct ScDsfHeader header;
	struct ScDsfVertex vertex;
	struct ScDsfArc    arc;
	size_t             extra_bytes;
	int i;

	/* read and validate header */
	fread (&header, sizeof (header), 1, reader->fh);
	if (memcmp (header.magic, "DAWG", 4) != 0) {
		/* Invalid file format */
		return FALSE;
	}
	if (header.version_maj != 1) {
		/* Wrong format version */
		return FALSE;
	}

	header.header_len = GUINT16_FROM_LE (header.header_len);

	header.n_vertices = GUINT32_FROM_LE (header.n_vertices);
	header.n_arcs = GUINT32_FROM_LE (header.n_arcs);

	extra_bytes = header.header_len - sizeof (struct ScDsfHeader);

	if (extra_bytes != 0) {
		/* Skip extra bytes */
		g_printerr ("not implemented\n");
	}

	if (ctx->header_cb)
		ctx->header_cb (reader, &header, user_data);

	/* Read vertex data */
	for (i = 0; i < header.n_vertices; i++) {
		fread (&vertex, sizeof (vertex), 1, reader->fh);

		vertex.n_arcs = GUINT32_FROM_LE (vertex.n_arcs);
		vertex.first_arc_idx = GUINT32_FROM_LE (vertex.first_arc_idx);

		if (ctx->vertex_cb)
			ctx->vertex_cb (reader, &vertex, user_data);
	}


	/* Read arc data */
	for (i = 0; i < header.n_arcs; i++) {
		fread (&arc, sizeof (arc), 1, reader->fh);

		arc.lid  = GUINT16_FROM_LE (arc.lid);
		arc.dest = GUINT32_FROM_LE (arc.dest);

		if (ctx->arc_cb)
			ctx->arc_cb (reader, &arc, user_data);
	}


	return TRUE;
}


