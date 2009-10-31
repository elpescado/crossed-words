/*
 * Scrabble
 * --------
 *
 * Copyright (C) 2009 Przemysław Sitek
 * 
 */

#ifndef __SCX_MOVE_ENTRY_H__
#define __SCX_MOVE_ENTRY_H__

/* Includes here */
#include <gtk/gtkvbox.h>

#include "sc-move.h"

G_BEGIN_DECLS

#define SCX_TYPE_MOVE_ENTRY \
	(scx_move_entry_get_type ())
#define SCX_MOVE_ENTRY(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST ((obj), SCX_TYPE_MOVE_ENTRY, ScxMoveEntry))
#define SCX_MOVE_ENTRY_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST ((klass), SCX_TYPE_MOVE_ENTRY, ScxMoveEntryClass))
#define SCX_IS_MOVE_ENTRY(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), SCX_TYPE_MOVE_ENTRY)
#define SCX_IS_MOVE_ENTRY_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), SCX_TYPE_MOVE_ENTRY))
#define SCX_MOVE_ENTRY_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), SCX_TYPE_MOVE_ENTRY, ScxMoveEntryClass))


typedef struct _ScxMoveEntry		ScxMoveEntry;
typedef struct _ScxMoveEntryPrivate		ScxMoveEntryPrivate;
typedef struct _ScxMoveEntryClass		ScxMoveEntryClass;

struct _ScxMoveEntry{
	GtkVBox		parent;
	ScxMoveEntryPrivate	*priv;
};

struct _ScxMoveEntryClass
{
	GtkVBoxClass		parent;

	/* Signals */
};



GType
scx_move_entry_get_type				(void);

GtkWidget*
scx_move_entry_new				(void);


ScOrientation
scx_move_entry_get_orientation (ScxMoveEntry *self);


const gchar *
scx_move_entry_get_text (ScxMoveEntry *self);



G_END_DECLS

#endif
