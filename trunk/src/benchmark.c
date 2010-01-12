#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "sc-dawg.h"
#include "util.h"

const int N = 25;


struct mstat_t {
	long mst_size;
	long mst_rss;
	long mst_share;
	long mst_text;
	long mst_lib;
	long mst_data;
	long mst_dt;
};


static gboolean
load_file (void *self, const gchar *file_name)
{
	FILE *f = fopen (file_name, "r");
	if (f == NULL) {
		return FALSE;
	}

	gchar buffer[128];

	while (fgets (buffer, 128, f)) {
		const char *word = g_strstrip (buffer);

#if defined(DATA_BST)
		g_tree_insert ((GTree *) self, g_strdup (word), GINT_TO_POINTER (1));
#elif defined(DATA_HASH)
		g_hash_table_insert ((GHashTable *) self, g_strdup (word), GINT_TO_POINTER (1));
#elif defined(DATA_DAWG)
#endif
	}

	fclose (f);

	return TRUE;
}


static int
my_mstat (pid_t pid, struct mstat_t *st)
{
	char path[32];
	snprintf (path, 32, "/proc/%d/statm", (int) pid);

	FILE *f = fopen (path, "r");
	if (f == NULL)
		return -1;

	fscanf (f, "%ld %ld %ld %ld %ld %ld %ld",
		&(st->mst_size),
		&(st->mst_rss),
		&(st->mst_share),
		&(st->mst_text),
		&(st->mst_lib),
		&(st->mst_data),
		&(st->mst_dt)
	);

	fclose (f);

	return 0;
}


static void
report_memory_usage (void)
{
	struct mstat_t st;
	my_mstat (getpid (), &st);

	long ps = sysconf(_SC_PAGESIZE);
	g_printerr ("Size: %ld RSS: %ld data: %ld\n",
			ps*st.mst_size, ps*st.mst_rss, ps*st.mst_data);
}


static double
benchmark (void *self, const gchar *file_name)
{
	FILE *f = fopen (file_name, "r");
	if (f == NULL) {
		exit (1);
	}

	gchar buffer[128];

	GList *words = NULL;

	while (fgets (buffer, 128, f)) {
		const char *word = g_strstrip (buffer);

		words = g_list_prepend (words, g_strdup (word));
	}

	fclose (f);

	GList *node;
	int n = 0;
	

	double t0 = foo_microtime ();

	int i = 0;
	for (i = 0; i < N; i++) {
		for (node = words; node; node = node->next) {
			const gchar *word = node->data;
			gpointer found;

#if defined(DATA_BST)
			found = g_tree_lookup ((GTree *) self, word);
#elif defined(DATA_HASH)
			found = g_hash_table_lookup ((GHashTable *) self, word);
#elif defined(DATA_DAWG)
#endif
			if (found == NULL)
				exit (2);

			n++;
		}
	}
	double t1 = foo_microtime ();

	while (words) {
		g_free (words->data);
		words = g_list_delete_link (words, words);
	}

	g_printerr ("Matched words: %d\n", n);

	return t1 - t0;
}


int main (int argc, char *argv[])
{
	/* Init data structure */
#if defined(DATA_BST)
	GTree *self = g_tree_new_full ((GCompareDataFunc)strcmp, NULL, g_free, NULL);
#elif defined(DATA_HASH)
	GHashTable *self = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
#elif defined(DATA_DAWG)
#endif

	const gchar *file = "lang/pl/dictionary.txt";

	load_file (self, file);

	report_memory_usage ();

	double time = benchmark (self, file);

	g_printerr ("Time: %lf\n", time);

	/* Free data structure */
#if defined(DATA_BST)
	g_tree_unref ((GTree *) self);
#elif defined(DATA_HASH)
	g_hash_table_unref ((GHashTable *) self);
#elif defined(DATA_DAWG)
#endif

	return 0;
}
