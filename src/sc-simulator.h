/*
 * Scrabble
 * --------
 *
 * Copyright (C) 2009 Przemysław Sitek
 * 
 */

#ifndef __SC_SIMULATOR_H__
#define __SC_SIMULATOR_H__

#include <glib.h>

#include "sc-game.h"


typedef struct _ScSimulatorTask ScSimulatorTask;
typedef struct _ScSimulator ScSimulator;


ScSimulator *
sc_simulator_new (void);


void
sc_simulator_free (ScSimulator *sim);


void
sc_simulator_run (ScSimulator *sim,
		          ScGame   *game,
                  ScPlayer *player,
				  ScMove   *move);


#endif /* __SC_SIMULATOR_H__ */
