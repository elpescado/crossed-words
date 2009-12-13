#include "compat.h"

#if !GTK_CHECK_VERSION(2,14,0)

GdkWindow *         gtk_widget_get_window               (GtkWidget *widget)
{
	return widget->window;
}


GtkWidget *         gtk_dialog_get_content_area         (GtkDialog *dialog)
{
	return dialog->vbox;
}

#endif
