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

#ifndef __SC_NOOB_PLAYER_H__
#define __SC_NOOB_PLAYER_H__

/* Includes here */
#include <gtk/gtkwidget.h>
#include <gtk/gtkwindow.h>

#include "sc-computer-player.h"

G_BEGIN_DECLS

#define SC_TYPE_NOOB_PLAYER \
	(sc_noob_player_get_type ())
#define SC_NOOB_PLAYER(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST ((obj), SC_TYPE_NOOB_PLAYER, ScNoobPlayer))
#define SC_NOOB_PLAYER_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST ((klass), SC_TYPE_NOOB_PLAYER, ScNoobPlayerClass))
#define SC_IS_NOOB_PLAYER(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), SC_TYPE_NOOB_PLAYER))
#define SC_IS_NOOB_PLAYER_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), SC_TYPE_NOOB_PLAYER))
#define SC_NOOB_PLAYER_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), SC_TYPE_NOOB_PLAYER, ScNoobPlayerClass))


typedef struct _ScNoobPlayer		ScNoobPlayer;
typedef struct _ScNoobPlayerPrivate		ScNoobPlayerPrivate;
typedef struct _ScNoobPlayerClass		ScNoobPlayerClass;

struct _ScNoobPlayer{
	ScComputerPlayer		parent;
	ScNoobPlayerPrivate	*priv;
};

struct _ScNoobPlayerClass
{
	ScComputerPlayerClass		parent;

	/* Signals */
};



GType
sc_noob_player_get_type				(void);

ScNoobPlayer*
sc_noob_player_new				(void);


G_END_DECLS

#endif
