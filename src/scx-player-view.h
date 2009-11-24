/*
 * Crossed Words
 * -------------
 *
 * Copyright (C) 2009 Przemysław Sitek
 * 
 */

#ifndef __SCX_PLAYER_VIEW_H__
#define __SCX_PLAYER_VIEW_H__

/* Includes here */
#include <gtk/gtkwidget.h>
#include <gtk/gtkwindow.h>

#include "sc-game.h"
#include "sc-player.h"


G_BEGIN_DECLS

#define SCX_TYPE_PLAYER_VIEW \
	(scx_player_view_get_type ())
#define SCX_PLAYER_VIEW(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST ((obj), SCX_TYPE_PLAYER_VIEW, ScxPlayerView))
#define SCX_PLAYER_VIEW_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST ((klass), SCX_TYPE_PLAYER_VIEW, ScxPlayerViewClass))
#define SCX_IS_PLAYER_VIEW(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), SCX_TYPE_PLAYER_VIEW))
#define SCX_IS_PLAYER_VIEW_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), SCX_TYPE_PLAYER_VIEW))
#define SCX_PLAYER_VIEW_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), SCX_TYPE_PLAYER_VIEW, ScxPlayerViewClass))


typedef struct _ScxPlayerView		ScxPlayerView;
typedef struct _ScxPlayerViewPrivate		ScxPlayerViewPrivate;
typedef struct _ScxPlayerViewClass		ScxPlayerViewClass;

struct _ScxPlayerView{
	GtkTable		parent;
	ScxPlayerViewPrivate	*priv;
};

struct _ScxPlayerViewClass
{
	GtkTableClass		parent;

	/* Signals */
};



GType
scx_player_view_get_type				(void);

GtkWidget*
scx_player_view_new				(void);


void
scx_player_view_set_player (ScxPlayerView *self,
                            ScGame        *game,
                            ScPlayer      *player);


G_END_DECLS

#endif
