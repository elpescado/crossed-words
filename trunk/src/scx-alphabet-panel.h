/*
 * Scrabble
 * --------
 *
 * Copyright (C) 2009 Przemysław Sitek
 * 
 */

#ifndef __SCX_ALPHABET_PANEL_H__
#define __SCX_ALPHABET_PANEL_H__

/* Includes here */
#include <gtk/gtkwidget.h>
#include <gtk/gtkwindow.h>

#include "alphabet.h"


G_BEGIN_DECLS

#define SCX_TYPE_ALPHABET_PANEL \
	(scx_alphabet_panel_get_type ())
#define SCX_ALPHABET_PANEL(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST ((obj), SCX_TYPE_ALPHABET_PANEL, ScxAlphabetPanel))
#define SCX_ALPHABET_PANEL_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST ((klass), SCX_TYPE_ALPHABET_PANEL, ScxAlphabetPanelClass))
#define SCX_IS_ALPHABET_PANEL(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), SCX_TYPE_ALPHABET_PANEL)
#define SCX_IS_ALPHABET_PANEL_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), SCX_TYPE_ALPHABET_PANEL))
#define SCX_ALPHABET_PANEL_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), SCX_TYPE_ALPHABET_PANEL, ScxAlphabetPanelClass))


typedef struct _ScxAlphabetPanel		ScxAlphabetPanel;
typedef struct _ScxAlphabetPanelPrivate		ScxAlphabetPanelPrivate;
typedef struct _ScxAlphabetPanelClass		ScxAlphabetPanelClass;

struct _ScxAlphabetPanel{
	GtkVBox		parent;
	ScxAlphabetPanelPrivate	*priv;
};

struct _ScxAlphabetPanelClass
{
	GtkVBoxClass		parent;

	/* Signals */
};



GType
scx_alphabet_panel_get_type				(void);


GtkWidget*
scx_alphabet_panel_new				(void);


void
scx_alphabet_panel_set_alphabet (ScxAlphabetPanel *self, Alphabet *al);


G_END_DECLS

#endif
