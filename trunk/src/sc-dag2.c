#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sc-dag2.h"

/*
#define sc_dag2_node_child(node,lid) (node->children[lid])
#define sc_dag2_node_set_child(node,lid,child) (node->children[lid] = child)
*/


ScDag2Node *
sc_dag2_node_child (const ScDag2Node *node, LID lid)
{
	if (lid == 0) return node->parent;
	int i;
	for (i = 0; i < node->n_children; i++)
		if (node->children[i].lid == lid)
			return node->children[i].node;

	return NULL;
}


void
sc_dag2_node_set_child (ScDag2Node *node, LID lid, ScDag2Node *child)
{
	if (lid == 0) {
		node->parent = child;
		return;
	}

	int i;
	for (i = 0; i < node->n_children; i++) {
		if (node->children[i].lid == lid) {
			node->children[i].node = child;
			return;
		}
	}

	if (node->n_children == node->children_size) {
		node->children_size *= 2;
	//	g_print ("More memory!!! (%d)\n", node->children_size);
		struct ScDag2NodeChildEntry *tmp = g_malloc (sizeof (struct ScDag2NodeChildEntry) * node->children_size);
		memcpy (tmp, node->children, sizeof (struct ScDag2NodeChildEntry) * node->n_children);
		g_free (node->children);
		node->children = tmp;
	}

	i = node->n_children++;
	node->children[i].lid = lid;
	node->children[i].node = child;
	/*
	struct ScDag2NodeChildEntry *n = &(node->children[node->n_children++]);
	n->lid = lid;
	n->node = child;
	*/
}




ScDag2 *
sc_dag2_new (void)
{
	ScDag2 *self = g_new0 (ScDag2, 1);

	self->root = sc_dag2_alloc_node (self);

	return self;
}


void
sc_dag2_free (ScDag2 *self)
{
	g_free (self);
}


ScDag2Node *
sc_dag2_alloc_node (ScDag2 *self)
{
	ScDag2Node *node = g_new0 (ScDag2Node, 1);
	node->children_size = 2;
	node->children = g_malloc (sizeof(struct ScDag2NodeChildEntry) * node->children_size);


	self->n_nodes++;
	self->size += sizeof (ScDag2Node);

	return node;
}


guint16
sc_dag2_node_hash (ScDag2Node *node)
{
	return 0x1234;
	guint32 h = 0x12345678 ^ node->lid ^ (node->flags << 8);
	gint i = 0;
	for (i = 0; i < node->n_children; i++) {
		h ^= GPOINTER_TO_INT(sc_dag2_node_child(node, i));
	}

	return ((guint16)h) ^ (h >> 16);
}

void
sc_dag2_add_drowword (ScDag2 *self, const gchar *word, Alphabet *al)
{
	glong len = g_utf8_strlen (word, -1);
	if (len > 10) {
	//	g_print ("too long, discarding: '%s'\n", word);
		return;
	}
	
	LID letters[15];
	alphabet_translate (al, word, letters);


	int i, j;
	for (i = 0; i < len; i++) {
		LID new_letters[17];
		memset (new_letters, '\0', sizeof (new_letters));
		for (j = 0; j <= i; j++) {
			new_letters[i - j] = letters[j];
		}
		new_letters[i+1] = 0;
		for (j = i+1; j <= len; j++) {
			new_letters[j+1] = letters[j];
		}

		/*
		for (j = 0; j <= len; j++) {
			Letter *l = alphabet_lookup_letter (al, new_letters[j]);
			g_print ("%s", l ? l->label : "_");
		}
		g_print ("\n");
		*/
		sc_dag2_add_word_translated (self, new_letters, len+1);
	}
}


void
sc_dag2_add_word (ScDag2 *self, const gchar *word, Alphabet *al)
{
	glong len = g_utf8_strlen (word, -1);
	if (len > 15) {
		g_print ("too long, discarding: '%s'\n", word);
		return;
	}

	LID letters[15];
	if (! alphabet_translate (al, word, letters)) {
		//g_print ("sc_dag2_add_word: Invalid word '%s', discarding\n", word);
		return;
	}

	sc_dag2_add_word_translated (self, letters, len);
}

void
sc_dag2_add_word_translated (ScDag2 *self, LID *letters, glong len)
{
	ScDag2Node *node = self->root;

//	g_print ("%s", word);

	gint i;
	for (i = 0; i < len; i++) {
		LID lid = letters[i];
//		g_print (" -> %d", lid);
		if (sc_dag2_node_child (node, lid) == NULL) {
			ScDag2Node *tmp = sc_dag2_alloc_node (self);
			tmp->lid = lid;
			sc_dag2_node_set_child (tmp, 0, node);
			sc_dag2_node_set_child (node, lid, tmp);

			node = tmp;
		} else {
			node = sc_dag2_node_child (node, lid);
		}
	}
	node->flags |= SC_DAG_NODE_FINAL;
//	g_print ("\n");
}


gboolean
sc_dag2_test_word (ScDag2 *self, const gchar *word, Alphabet *al)
{
	glong len = g_utf8_strlen (word, -1);
	if (len > 15) {
		g_print ("too long, discarding: '%s'\n", word);
		return FALSE;
	}

	LID letters[15];
	if (! alphabet_translate (al, word, letters))
		return FALSE;

	return sc_dag2_test_word_translated (self, letters, len);
}


gboolean
sc_dag2_test_word_translated (ScDag2 *self, LID *letters, glong len)
{
	ScDag2Node *node = self->root;

	gint i;
	for (i = 0; i < len; i++) {
		LID lid = letters[i];
		if (sc_dag2_node_child (node, lid) == NULL) {
			return FALSE;
		} else {
			node = sc_dag2_node_child (node, lid);
		}
	}

	return node->flags & SC_DAG_NODE_FINAL;
}


gboolean
sc_dag2_load_file (ScDag2 *self, const gchar *file_name, Alphabet *al)
{
	FILE *f = fopen (file_name, "r");
	if (f == NULL) {
		return FALSE;
	}

	gchar buffer[128];

	while (fgets (buffer, 128, f)) {
		char *word = g_strstrip (buffer);
		sc_dag2_add_word (self, word, al);
	}

	fclose (f);	

	return TRUE;
}


static int nodes_per_level[16] = {0};
static ScDag2Node **nodes_tmp[16] = {0};


static int _max_arcs_per_node = 0;
static guint64 total_arcs = 0;

static gint
_traverse_tree (ScDag2 *dag, ScDag2Node *node)
{
	gint depth = 0;
	gint i;

	dag->size += sizeof (ScDag2Node);
	dag->size += node->children_size * sizeof (struct ScDag2NodeChildEntry);

	gint arcs = 0;
	for (i = 1; i < 33; i++) {
		if (sc_dag2_node_child (node, i) != NULL) {
			arcs++;
			total_arcs++;
			gint child_depth = _traverse_tree (dag, sc_dag2_node_child (node, i));
			if (child_depth > depth) {
				depth = child_depth;
			}
		}
	}
	if (arcs > _max_arcs_per_node)
		_max_arcs_per_node = arcs;

//	node->hash_code = sc_dag2_node_hash (node);
	int idx = nodes_per_level[depth]++;
	nodes_tmp[depth][idx] = node;
	return depth+1;
}


static int
_sort_cmp_func (const void *pa, const void *pb)
{
	ScDag2Node *a = *(ScDag2Node**)pa;
	ScDag2Node *b = *(ScDag2Node**)pb;
	return a->hash_code - b->hash_code;
}


static int n_comparisons = 0;

gboolean
sc_dag2_node_equal (const ScDag2Node *a, const ScDag2Node *b)
{
	n_comparisons++;

	if (a->lid != b->lid)
		return FALSE;

	if (a->flags != b->flags)
		return FALSE;

	int i;
	for (i = 1; i < 33; i++) {
		if (sc_dag2_node_child (a, i) != sc_dag2_node_child (b, i))
			return FALSE;
	}
	return TRUE;
}


void
sc_dag2_print_stats (ScDag2 *self)
{
	g_print ("DAG: n_nodes=%d, size=%d bytes\n",
	         self->n_nodes, self->size);


	gint i;
	for (i = 0; i < 16; i++) {
		nodes_tmp[i] = g_malloc (sizeof (ScDag2Node *) * 8*1024*1024/*1948644*/);
	}

	self->size = 0;
	_traverse_tree (self, self->root);
	g_print (" => computed DAG size: %d <= \n", self->size);

	g_print ("Total arcs %lld max per node %d avg arcs per node %lld\n",
			total_arcs, _max_arcs_per_node, total_arcs / self->n_nodes);

	gint total_nodes = 0;
	for (i = 0; i < 16; i++) {
		g_print ("Level %2d: %8d nodes\n", i, nodes_per_level[i]);
		total_nodes += nodes_per_level[i];
	}

	g_print ("Total nodes = %d (%s)\n", total_nodes, total_nodes == self->n_nodes ? "ok" : "fail");

	gint total_edges = total_nodes - 1;

	g_print ("Sorting...\n");
	for (i = 0; i < 16; i++) {
		int j;
		for (j = 0; j < nodes_per_level[i]; j++) {
			ScDag2Node *node = nodes_tmp[i][j];
			node->hash_code = sc_dag2_node_hash (node);
		}

		qsort (nodes_tmp[i], nodes_per_level[i], sizeof (ScDag2Node*),
		       _sort_cmp_func);

		gint n_duplicates = 0;
		n_comparisons = 0;

		/* Remove duplicates */
		for (j = 0; j < nodes_per_level[i]; j++) {
			ScDag2Node *node1 = nodes_tmp[i][j];
			if (node1 == NULL)
				continue;

			int k;
			for (k = j+1; k < nodes_per_level[i]; k++) {
				ScDag2Node *node2 = nodes_tmp[i][k];
				if (node2 == NULL)
					continue;
				if (node2->hash_code != node1->hash_code)
					break;

				if (sc_dag2_node_equal (node1, node2)) {
					/* Found duplicate */
					ScDag2Node *parent = sc_dag2_node_child (node2, 0);
					LID lid = node1->lid;
				//	g_print ("node1 = %p, node2 = %p, parent = %p, lid = %d\n",
				//	         node1, node2, parent, (int) lid);
					sc_dag2_node_set_child (parent, lid, node1);
					nodes_tmp[i][k] = NULL; //node2 = NULL;
					n_duplicates++;

					/* Count edges originating from node2 */
					int l;
					for (l = 1; l < 33; l++)
						if (sc_dag2_node_child (node2, l) != NULL)
							total_edges--;


					g_free (node2);
					self->n_nodes--;
					self->size -= sizeof (ScDag2Node);
				} // if
			} // for k
		} // for j
		g_print (" %2d: %8d duplicates, %8d unique, %8d total, %12d comparisons, ratio %lf\n", i, n_duplicates,
				 nodes_per_level[i] - n_duplicates,
				 nodes_per_level[i], n_comparisons,
				(double)n_comparisons/(double)nodes_per_level[i] );
	} // for i

	g_print ("After minimization:\n");

	g_print ("DAG: n_nodes=%d, size=%d bytes, n_edges=%d\n",
	         self->n_nodes, self->size, total_edges);
}