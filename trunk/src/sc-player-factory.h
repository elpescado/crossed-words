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

#ifndef __SC_PLAYER_FACTORY_H__
#define __SC_PLAYER_FACTORY_H__

/* Includes here */
#include <gtk/gtkwidget.h>
#include <gtk/gtkwindow.h>

#include "sc-player.h"

G_BEGIN_DECLS

#define SC_TYPE_PLAYER_FACTORY \
	(sc_player_factory_get_type ())
#define SC_PLAYER_FACTORY(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST ((obj), SC_TYPE_PLAYER_FACTORY, ScPlayerFactory))
#define SC_PLAYER_FACTORY_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST ((klass), SC_TYPE_PLAYER_FACTORY, ScPlayerFactoryClass))
#define SC_IS_PLAYER_FACTORY(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), SC_TYPE_PLAYER_FACTORY))
#define SC_IS_PLAYER_FACTORY_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), SC_TYPE_PLAYER_FACTORY))
#define SC_PLAYER_FACTORY_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), SC_TYPE_PLAYER_FACTORY, ScPlayerFactoryClass))


typedef struct _ScPlayerFactory		ScPlayerFactory;
typedef struct _ScPlayerFactoryPrivate		ScPlayerFactoryPrivate;
typedef struct _ScPlayerFactoryClass		ScPlayerFactoryClass;

struct _ScPlayerFactory{
	GObject		parent;
	ScPlayerFactoryPrivate	*priv;
};

struct _ScPlayerFactoryClass
{
	GObjectClass		parent;

	/* Signals */
};


typedef ScPlayer* (*ScPlayerConstructor)(void);



GType
sc_player_factory_get_type				(void);

ScPlayerFactory*
sc_player_factory_new				(void);


void
sc_player_factory_add_type (ScPlayerFactory     *self,
                            ScPlayerConstructor  func,
                            const gchar         *name,
                            const gchar         *desc);


gchar **
sc_player_factory_get_types (ScPlayerFactory *self);


ScPlayer *
sc_player_factory_create (ScPlayerFactory *self,
                          const gchar     *name);


G_END_DECLS

#endif
