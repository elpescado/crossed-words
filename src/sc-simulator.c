/*
 * Scrabble
 * --------
 *
 * Copyright (C) 2009 Przemysław Sitek
 * 
 */

#include "sc-simulator.h"


struct _ScSimulator {
};


ScSimulator *
sc_simulator_new (void)
{
	ScSimulator *sim = g_new0 (ScSimulator, 1);

	return sim;
}


void
sc_simulator_free (ScSimulator *sim)
{
	g_free (sim);
}


void
sc_simulator_run (void)
{
}


