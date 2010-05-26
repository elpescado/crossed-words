/*
 * Crossed Words
 * -------------
 *
 * Copyright (C) 2009 Przemysław Sitek
 * 
 */

#ifndef __SC_SIM_PLAYER_H__
#define __SC_SIM_PLAYER_H__

/* Includes here */
#include <gtk/gtkwidget.h>
#include <gtk/gtkwindow.h>

#include "sc-computer-player.h"

G_BEGIN_DECLS

#define SC_TYPE_SIM_PLAYER \
	(sc_sim_player_get_type ())
#define SC_SIM_PLAYER(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST ((obj), SC_TYPE_SIM_PLAYER, ScSimPlayer))
#define SC_SIM_PLAYER_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST ((klass), SC_TYPE_SIM_PLAYER, ScSimPlayerClass))
#define SC_IS_SIM_PLAYER(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), SC_TYPE_SIM_PLAYER))
#define SC_IS_SIM_PLAYER_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), SC_TYPE_SIM_PLAYER))
#define SC_SIM_PLAYER_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), SC_TYPE_SIM_PLAYER, ScSimPlayerClass))


typedef struct _ScSimPlayer		ScSimPlayer;
typedef struct _ScSimPlayerPrivate		ScSimPlayerPrivate;
typedef struct _ScSimPlayerClass		ScSimPlayerClass;

struct _ScSimPlayer{
	ScComputerPlayer		parent;
	ScSimPlayerPrivate	*priv;
};

struct _ScSimPlayerClass
{
	ScComputerPlayerClass		parent;

	/* Signals */
};



GType
sc_sim_player_get_type				(void);

ScSimPlayer*
sc_sim_player_new				(void);


G_END_DECLS

#endif
