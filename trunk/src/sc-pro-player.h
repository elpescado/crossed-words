/*
 * Crossed Words
 * -------------
 *
 * Copyright (C) 2009 Przemysław Sitek
 * 
 */

#ifndef __SC_PRO_PLAYER_H__
#define __SC_PRO_PLAYER_H__

/* Includes here */
#include <gtk/gtkwidget.h>
#include <gtk/gtkwindow.h>

#include "sc-computer-player.h"

G_BEGIN_DECLS

#define SC_TYPE_PRO_PLAYER \
	(sc_pro_player_get_type ())
#define SC_PRO_PLAYER(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST ((obj), SC_TYPE_PRO_PLAYER, ScProPlayer))
#define SC_PRO_PLAYER_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST ((klass), SC_TYPE_PRO_PLAYER, ScProPlayerClass))
#define SC_IS_PRO_PLAYER(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), SC_TYPE_PRO_PLAYER))
#define SC_IS_PRO_PLAYER_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), SC_TYPE_PRO_PLAYER))
#define SC_PRO_PLAYER_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), SC_TYPE_PRO_PLAYER, ScProPlayerClass))


typedef struct _ScProPlayer		ScProPlayer;
typedef struct _ScProPlayerPrivate		ScProPlayerPrivate;
typedef struct _ScProPlayerClass		ScProPlayerClass;

struct _ScProPlayer{
	ScComputerPlayer		parent;
	ScProPlayerPrivate	*priv;
};

struct _ScProPlayerClass
{
	ScComputerPlayerClass		parent;

	/* Signals */
};



GType
sc_pro_player_get_type				(void);

ScProPlayer*
sc_pro_player_new				(void);


G_END_DECLS

#endif
