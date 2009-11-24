/*
 * Crossed Words
 * -------------
 *
 * Copyright (C) 2009 Przemysław Sitek
 * 
 */

#ifndef __SCX_GAME_PANEL_H__
#define __SCX_GAME_PANEL_H__

/* Includes here */
#include <gtk/gtkwidget.h>
#include <gtk/gtkwindow.h>

#include "sc-game.h"

G_BEGIN_DECLS

#define SCX_TYPE_GAME_PANEL \
	(scx_game_panel_get_type ())
#define SCX_GAME_PANEL(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST ((obj), SCX_TYPE_GAME_PANEL, ScxGamePanel))
#define SCX_GAME_PANEL_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST ((klass), SCX_TYPE_GAME_PANEL, ScxGamePanelClass))
#define SCX_IS_GAME_PANEL(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), SCX_TYPE_GAME_PANEL))
#define SCX_IS_GAME_PANEL_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), SCX_TYPE_GAME_PANEL))
#define SCX_GAME_PANEL_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), SCX_TYPE_GAME_PANEL, ScxGamePanelClass))


typedef struct _ScxGamePanel		ScxGamePanel;
typedef struct _ScxGamePanelPrivate		ScxGamePanelPrivate;
typedef struct _ScxGamePanelClass		ScxGamePanelClass;

struct _ScxGamePanel{
	GtkVBox		parent;
	ScxGamePanelPrivate	*priv;
};

struct _ScxGamePanelClass
{
	GtkVBoxClass		parent;

	/* Signals */
};



GType
scx_game_panel_get_type				(void);

GtkWidget*
scx_game_panel_new				(void);


void
scx_game_panel_update (ScxGamePanel *self, ScGame *game);


G_END_DECLS

#endif
