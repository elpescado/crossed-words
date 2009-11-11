/*
 * Scrabble
 * --------
 *
 * Copyright (C) 2009 Przemysław Sitek
 * 
 */

#ifndef __SCX_CONSOLE_H__
#define __SCX_CONSOLE_H__

/* Includes here */
#include <gtk/gtkwidget.h>
#include <gtk/gtkwindow.h>


G_BEGIN_DECLS

#define SCX_TYPE_CONSOLE \
	(scx_console_get_type ())
#define SCX_CONSOLE(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST ((obj), SCX_TYPE_CONSOLE, ScxConsole))
#define SCX_CONSOLE_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST ((klass), SCX_TYPE_CONSOLE, ScxConsoleClass))
#define SCX_IS_CONSOLE(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), SCX_TYPE_CONSOLE)
#define SCX_IS_CONSOLE_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), SCX_TYPE_CONSOLE))
#define SCX_CONSOLE_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), SCX_TYPE_CONSOLE, ScxConsoleClass))


typedef struct _ScxConsole		ScxConsole;
typedef struct _ScxConsolePrivate		ScxConsolePrivate;
typedef struct _ScxConsoleClass		ScxConsoleClass;

struct _ScxConsole{
	GtkVBox		parent;
	ScxConsolePrivate	*priv;
};

struct _ScxConsoleClass
{
	GtkVBoxClass		parent;

	/* Signals */
};



GType
scx_console_get_type				(void);

GtkWidget*
scx_console_new				(void);


void
scx_console_print (ScxConsole *self, const gchar *msg);


G_END_DECLS

#endif
