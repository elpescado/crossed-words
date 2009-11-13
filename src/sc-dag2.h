#ifndef __SC_DAG2_H__
#define __SC_DAG2_H__

#include <glib.h>

#include "alphabet.h"

typedef struct ScDag2Node {
	char               lid;
	guint8             flags;
	guint16            hash_code;
	struct ScDag2Node *parent;
	gint16             n_children;
	gint16             children_size;
	struct ScDag2NodeChildEntry {
		LID                lid;
		struct ScDag2Node *node;
	} *children;
} ScDag2Node;

typedef enum {
	SC_DAG_NODE_FINAL = 1
} ScDag2NodeFlags;


typedef struct {
	gint  n_nodes;
	gsize size;

	ScDag2Node *root;
} ScDag2;


ScDag2 *
sc_dag2_new (void);


void
sc_dag2_free (ScDag2 *self);


ScDag2Node *
sc_dag2_alloc_node (ScDag2 *self);

void
sc_dag2_add_word_translated (ScDag2 *self, LID *letters, glong len);

gboolean
sc_dag2_test_word_translated (ScDag2 *self, LID *letters, glong len);


void
sc_dag2_add_word (ScDag2 *self, const gchar *word, Alphabet *al);


gboolean
sc_dag2_load_file (ScDag2 *self, const gchar *file_name, Alphabet *al);


gboolean
sc_dag2_test_word (ScDag2 *self, const gchar *word, Alphabet *al);


void
sc_dag2_print_stats (ScDag2 *self);


#endif /* __SC_DAG2_H__ */
