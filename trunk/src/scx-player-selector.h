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

#ifndef __SCX_PLAYER_SELECTOR_H__
#define __SCX_PLAYER_SELECTOR_H__

/* Includes here */
#include <gtk/gtkwidget.h>
#include <gtk/gtkwindow.h>


G_BEGIN_DECLS

#define SCX_TYPE_PLAYER_SELECTOR \
	(scx_player_selector_get_type ())
#define SCX_PLAYER_SELECTOR(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST ((obj), SCX_TYPE_PLAYER_SELECTOR, ScxPlayerSelector))
#define SCX_PLAYER_SELECTOR_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST ((klass), SCX_TYPE_PLAYER_SELECTOR, ScxPlayerSelectorClass))
#define SCX_IS_PLAYER_SELECTOR(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), SCX_TYPE_PLAYER_SELECTOR))
#define SCX_IS_PLAYER_SELECTOR_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), SCX_TYPE_PLAYER_SELECTOR))
#define SCX_PLAYER_SELECTOR_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), SCX_TYPE_PLAYER_SELECTOR, ScxPlayerSelectorClass))


typedef struct _ScxPlayerSelector		ScxPlayerSelector;
typedef struct _ScxPlayerSelectorPrivate		ScxPlayerSelectorPrivate;
typedef struct _ScxPlayerSelectorClass		ScxPlayerSelectorClass;

struct _ScxPlayerSelector{
	GtkTable		parent;
	ScxPlayerSelectorPrivate	*priv;
};

struct _ScxPlayerSelectorClass
{
	GtkTableClass		parent;

	/* Signals */
};



GType
scx_player_selector_get_type				(void);

GtkWidget*
scx_player_selector_new				(void);


void
scx_player_selector_set_types (ScxPlayerSelector  *self,
                               char             **types);


gchar *
scx_player_selector_get_type_name (ScxPlayerSelector *self);


const gchar *
scx_player_selector_get_name (ScxPlayerSelector *self);



G_END_DECLS

#endif
