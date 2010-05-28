#include "sc-simulator.h"
#include "sc-game.h"
#include "sc-computer-player.h"

int main (int argc, char *argv[])
{
	g_type_init ();
	g_thread_init (NULL);
	srand (time (NULL));


	ScSimulator *sim = sc_simulator_new ();
	ScGame *game = sc_game_new (NULL);

	ScPlayer *p1 = SC_PLAYER (sc_computer_player_new ());
	p1->game = game;
	sc_game_set_player (game, 0, p1);

	ScPlayer *p2 = SC_PLAYER (sc_computer_player_new ());
	p2->game = game;
	sc_game_set_player (game, 1, p2);


	sc_simulator_run (sim, game, p1);
	


	return 0;
}
