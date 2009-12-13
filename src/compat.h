#include <gtk/gtk.h>

/* Various compatibility functions */


#if !GTK_CHECK_VERSION(2,14,0)

GdkWindow *         gtk_widget_get_window               (GtkWidget *widget);

GtkWidget *         gtk_dialog_get_content_area         (GtkDialog *dialog);

#endif
