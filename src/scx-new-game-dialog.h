/*
 * Crossed Words
 * -------------
 *
 * Copyright (C) 2009 Przemysław Sitek
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __SCX_NEW_GAME_DIALOG_H__
#define __SCX_NEW_GAME_DIALOG_H__

/* Includes here */
#include <gtk/gtkwidget.h>
#include <gtk/gtkwindow.h>

#include "sc-player-factory.h"


G_BEGIN_DECLS

#define SCX_TYPE_NEW_GAME_DIALOG \
	(scx_new_game_dialog_get_type ())
#define SCX_NEW_GAME_DIALOG(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST ((obj), SCX_TYPE_NEW_GAME_DIALOG, ScxNewGameDialog))
#define SCX_NEW_GAME_DIALOG_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST ((klass), SCX_TYPE_NEW_GAME_DIALOG, ScxNewGameDialogClass))
#define SCX_IS_NEW_GAME_DIALOG(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), SCX_TYPE_NEW_GAME_DIALOG))
#define SCX_IS_NEW_GAME_DIALOG_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), SCX_TYPE_NEW_GAME_DIALOG))
#define SCX_NEW_GAME_DIALOG_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), SCX_TYPE_NEW_GAME_DIALOG, ScxNewGameDialogClass))


typedef struct _ScxNewGameDialog		ScxNewGameDialog;
typedef struct _ScxNewGameDialogPrivate		ScxNewGameDialogPrivate;
typedef struct _ScxNewGameDialogClass		ScxNewGameDialogClass;

struct _ScxNewGameDialog{
	GtkDialog		parent;
	ScxNewGameDialogPrivate	*priv;
};

struct _ScxNewGameDialogClass
{
	GtkDialogClass		parent;

	/* Signals */
};



GType
scx_new_game_dialog_get_type				(void);


GtkWidget*
scx_new_game_dialog_new				(void);


void
scx_new_game_dialog_set_factory (ScxNewGameDialog *self,
                                 ScPlayerFactory  *factory);


ScPlayer *
scx_new_game_dialog_create_player (ScxNewGameDialog *self,
                                   gint i);



G_END_DECLS

#endif
