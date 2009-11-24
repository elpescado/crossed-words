#include <stdlib.h>
#include <time.h>

#include <gtk/gtk.h>

#include "alphabet.h"
#include "letter.h"
#include "sc-board.h"
#include "sc-dag.h"
#include "sc-dag2.h"
#include "sc-dawg.h"
#include "sc-game.h"
#include "sc-move.h"
#include "sc-bag.h"
#include "scx-board-view.h"
#include "scx-move-entry.h"
#include "scx-rack-view.h"
#include "scx-main-window.h"
#include "util.h"


int main (int argc, char *argv[])
{
	GtkWidget *main_window;

	gtk_init (&argc, &argv);
	srand (time (NULL));

	#ifdef __WIN32
	gtk_rc_parse ("gtkrc");
	#endif

	main_window = scx_main_window_new ();
	gtk_widget_show (main_window);
	g_signal_connect (G_OBJECT (main_window), "delete-event", G_CALLBACK (gtk_main_quit), NULL);

	gtk_main ();

	return EXIT_SUCCESS;
}
