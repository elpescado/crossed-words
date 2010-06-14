#include <stdlib.h>
#include <glib.h>
#include "sc-dawg.h"

gint main (gint argc, char *argv[])
{
	ScDawg *dawg = sc_dawg_load (argv[1]);
	g_print ("Vertices: %8d\n", dawg->n_vertices);
	g_print ("Edges:    %8d\n", dawg->n_arcs);
	return EXIT_SUCCESS;
}
