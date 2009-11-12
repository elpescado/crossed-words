#ifndef __SC_DAG_H__
#define __SC_DAG_H__

#include <glib.h>

#include "alphabet.h"

typedef struct ScDagNode {
	char               lid;
	guint8             flags;
	guint16            hash_code;
	struct ScDagNode  *children[35];
} ScDagNode;

typedef enum {
	SC_DAG_NODE_FINAL = 1
} ScDagNodeFlags;


typedef struct {
	gint  n_nodes;
	gsize size;

	ScDagNode *root;
} ScDag;


ScDag *
sc_dag_new (void);


void
sc_dag_free (ScDag *self);


ScDagNode *
sc_dag_alloc_node (ScDag *self);

void
sc_dag_add_word_translated (ScDag *self, LID *letters, glong len);

gboolean
sc_dag_test_word_translated (ScDag *self, LID *letters, glong len);


void
sc_dag_add_word (ScDag *self, const gchar *word, Alphabet *al);


gboolean
sc_dag_load_file (ScDag *self, const gchar *file_name, Alphabet *al);


gboolean
sc_dag_test_word (ScDag *self, const gchar *word, Alphabet *al);


void
sc_dag_print_stats (ScDag *self);


#endif /* __SC_DAG_H__ */
