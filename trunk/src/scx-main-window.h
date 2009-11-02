/*
 * Scrabble
 * --------
 *
 * Copyright (C) 2009 Przemysław Sitek
 * 
 */

#ifndef __SCX_MAIN_WINDOW_H__
#define __SCX_MAIN_WINDOW_H__

/* Includes here */
#include <gtk/gtkwidget.h>
#include <gtk/gtkwindow.h>

G_BEGIN_DECLS

#define SCX_TYPE_MAIN_WINDOW \
	(scx_main_window_get_type ())
#define SCX_MAIN_WINDOW(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST ((obj), SCX_TYPE_MAIN_WINDOW, ScxMainWindow))
#define SCX_MAIN_WINDOW_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST ((klass), SCX_TYPE_MAIN_WINDOW, ScxMainWindowClass))
#define SCX_IS_MAIN_WINDOW(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), SCX_TYPE_MAIN_WINDOW)
#define SCX_IS_MAIN_WINDOW_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), SCX_TYPE_MAIN_WINDOW))
#define SCX_MAIN_WINDOW_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), SCX_TYPE_MAIN_WINDOW, ScxMainWindowClass))


typedef struct _ScxMainWindow		ScxMainWindow;
typedef struct _ScxMainWindowPrivate		ScxMainWindowPrivate;
typedef struct _ScxMainWindowClass		ScxMainWindowClass;

struct _ScxMainWindow{
	GtkWindow		parent;
	ScxMainWindowPrivate	*priv;
};

struct _ScxMainWindowClass
{
	GtkWindowClass		parent;

	/* Signals */
};



GType
scx_main_window_get_type				(void);

GtkWidget*
scx_main_window_new				(void);


G_END_DECLS

#endif
