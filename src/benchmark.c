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


struct pstat_t {
	int pst_pid;
	char pst_comm[64];
	char pst_state;
	int pst_ppid;
	int pst_pgrp;
	int pst_session;
	int pst_tty_nr;
	int pst_tpgid;
	unsigned pst_flags;
	unsigned long pst_minflt;
	unsigned long pst_cminflt;
	unsigned long pst_majflt;
	unsigned long pst_cmajflt;
	unsigned long pst_utime;
	unsigned long pst_stime;
	long pst_cutime;
	long pst_cstime;
	long pst_priority;
	long pst_nice;
	long pst_num_threads;
	long pst_itrealvalue;
	unsigned long long pst_starttime;
	unsigned long pst_vsize;
	long pst_rss;
	unsigned long pst_rsslim;
	unsigned long pst_startcode;
	unsigned long pst_endcode;
	unsigned long pst_startstack;
	unsigned long pst_kstkesp;
	unsigned long pst_kstkeip;
	unsigned long pst_signal;
	unsigned long pst_blocked;
	unsigned long pst_sigignore;
	unsigned long pst_sigcatch;
	unsigned long pst_wchan;
	unsigned long pst_nswap;
	unsigned long pst_cnswap;
	int pst_exit_signal;
	unsigned pst_processor;
	unsigned pst_rt_priority;
	unsigned pst_policy;
	unsigned long long pst_delayacct_blkio_ticks;
	unsigned long pst_guest_time;
	long pst_cguest_time;	
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
my_pstat (struct pstat_t *pst)
{
	char path[32];
	snprintf (path, 32, "/proc/%d/stat", (int)getpid());

	FILE *f = fopen (path, "r");
	fscanf (f, "%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu %lu %lu %lu %ld %ld %ld %llu %lu %ld %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %d %d %u %u %llu %lu %ld",
			&(pst->pst_pid),
			pst->pst_comm,
			&(pst->pst_state),
			&(pst->pst_ppid),
			&(pst->pst_pgrp),
			&(pst->pst_session),
			&(pst->pst_tty_nr),
			&(pst->pst_tpgid),
			&(pst->pst_flags),
			&(pst->pst_minflt),
			&(pst->pst_cminflt),
			&(pst->pst_majflt),
			&(pst->pst_cmajflt),
			&(pst->pst_utime),
			&(pst->pst_stime),
			&(pst->pst_cutime),
			&(pst->pst_cstime),
			&(pst->pst_priority),
			&(pst->pst_nice),
			&(pst->pst_num_threads),
			&(pst->pst_itrealvalue),
			&(pst->pst_starttime),
			&(pst->pst_vsize),
			&(pst->pst_rss),
			&(pst->pst_rsslim),
			&(pst->pst_startcode),
			&(pst->pst_endcode),
			&(pst->pst_startstack),
			&(pst->pst_kstkesp),
			&(pst->pst_kstkeip),
			&(pst->pst_signal),
			&(pst->pst_blocked),
			&(pst->pst_sigignore),
			&(pst->pst_sigcatch),
			&(pst->pst_wchan),
			&(pst->pst_nswap),
			&(pst->pst_cnswap),
			&(pst->pst_exit_signal),
			&(pst->pst_processor),
			&(pst->pst_rt_priority),
			&(pst->pst_policy),
			&(pst->pst_delayacct_blkio_ticks),
			&(pst->pst_guest_time),
			&(pst->pst_cguest_time)
		);
	fclose (f);
	return 0;
}


static void
report_memory_usage (void)
{
	/*
	struct rusage usage;

	if (getrusage (RUSAGE_SELF, &usage) == 0) {
		g_printerr ("Memory: %ld\n", usage.ru_idrss);
	}
	*/
	struct pstat_t st;
	my_pstat (&st);
	g_printerr ("Memory: %ld\n", st.pst_rss);
	
	sleep (10);
	g_printerr ("Resuming\n");
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
