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


typedef void (*ScSimulatorFunc)  (ScSimulator     *sim,
                                  ScSimulatorTask *task,
                                  gpointer         user_data);


ScSimulator *
sc_simulator_new (void);


void
sc_simulator_free (ScSimulator *sim);


void
sc_simulator_run (ScSimulator *sim,
		          ScGame   *game,
                  ScPlayer *player,
				  ScMove   *move,
				  ScSimulatorFunc callback,
				  gpointer        user_data);


ScSimulatorTask *
sc_simulator_task_ref (ScSimulatorTask *task);


void
sc_simulator_task_unref (ScSimulatorTask *task);


#endif /* __SC_SIMULATOR_H__ */
