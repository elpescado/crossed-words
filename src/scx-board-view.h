/*
 * Scrabble
 * --------
 *
 * Copyright (C) 2009 Przemysław Sitek
 * 
 */

#ifndef __SCX_BOARD_VIEW_H__
#define __SCX_BOARD_VIEW_H__

/* Includes here */
#include <gtk/gtkdrawingarea.h>

#include "sc-board.h"
#include "sc-move.h"

G_BEGIN_DECLS

#define SCX_TYPE_BOARD_VIEW \
	(scx_board_view_get_type ())
#define SCX_BOARD_VIEW(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST ((obj), SCX_TYPE_BOARD_VIEW, ScxBoardView))
#define SCX_BOARD_VIEW_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST ((klass), SCX_TYPE_BOARD_VIEW, ScxBoardViewClass))
#define SCX_IS_BOARD_VIEW(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), SCX_TYPE_BOARD_VIEW)
#define SCX_IS_BOARD_VIEW_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), SCX_TYPE_BOARD_VIEW))
#define SCX_BOARD_VIEW_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), SCX_TYPE_BOARD_VIEW, ScxBoardViewClass))


typedef struct _ScxBoardView		ScxBoardView;
typedef struct _ScxBoardViewPrivate		ScxBoardViewPrivate;
typedef struct _ScxBoardViewClass		ScxBoardViewClass;

struct _ScxBoardView{
	GtkDrawingArea		parent;
	ScxBoardViewPrivate	*priv;
};

struct _ScxBoardViewClass
{
	GtkDrawingAreaClass		parent;

	/* Signals */
};



GType
scx_board_view_get_type				(void);

GtkWidget*
scx_board_view_new				(void);


void
scx_board_view_set_board (ScxBoardView *self,
                          ScBoard      *board);


void
scx_board_view_set_move (ScxBoardView *self,
                        ScMove       *move);


void
scx_board_view_enable_selection (ScxBoardView *self);


void
scx_board_view_disable_selection (ScxBoardView *self);


gboolean
scx_board_view_get_selection (ScxBoardView *select, gint *i, gint *j);


void
scx_board_view_set_selection (ScxBoardView *select, gint i, gint j);


G_END_DECLS

#endif
