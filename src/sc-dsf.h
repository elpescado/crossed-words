#ifndef __SC_DSF_H__
#define __SC_DSF_H__

#include <glib.h>

/* Disc file format description */

struct ScDsfHeader {
	/* File metadata */
	char      magic[4];		/* Magic value, should be "DAWG"  */
	guint8    version_maj;	/* Major version of file format   */
	guint8    version_min;	/* Minor version of file format   */
	guint16   header_len;	/* File header length, in bytes   */

	/* Graph metadata */
	guint32   n_vertices;	/* Number of vertices             */
	guint32   n_arcs;		/* Number of arcs                 */
};


struct ScDsfVertex {
	guint8    flags;	/* Arc flags          */
	guint8   _unused1;	/* Unused             */
	guint8   _unused2;	/* Unused             */
	guint8   _unused3;	/* Unused             */
	guint32   n_arcs;			/* Number of arcs originating from this vertex */
	guint32   first_arc_idx;	/* Index of first arc                          */
};


struct ScDsfArc {
	guint16   lid;		/* Letter ID          */
	guint32   dest;		/* Destination vertex */
};


/* Writer API */

typedef struct ScDsfWriter ScDsfWriter;


ScDsfWriter *
sc_dsf_writer_open (const gchar *file_name,
                    guint n_vertices,
                    guint n_edges);


gboolean
sc_dsf_writer_close (ScDsfWriter        *writer);


void
sc_dsf_writer_write_vertex (ScDsfWriter              *writer,
                            const struct ScDsfVertex *vertex);
                            
void
sc_dsf_writer_write_arc (ScDsfWriter           *writer,
                         const struct ScDsfArc *arc);


  
/* Reader API */

typedef struct ScDsfReader ScDsfReader;

struct ScDsfReaderCtx {
	void (*header_cb) (ScDsfReader *reader, struct ScDsfHeader *header, gpointer user_data);
	void (*vertex_cb) (ScDsfReader *reader, struct ScDsfVertex *vertex, gpointer user_data);
	void (*arc_cb)    (ScDsfReader *reader, struct ScDsfArc    *arc,    gpointer user_data);
};


ScDsfReader *
sc_dsf_reader_open (const gchar *file_name);


gboolean
sc_dsf_reader_close (ScDsfReader *reader);


gboolean
sc_dsf_reader_read (ScDsfReader           *reader,
                    struct ScDsfReaderCtx *ctx,
                    gpointer               user_data);


#endif /* __SC_DSF_H__ */
