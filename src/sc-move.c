
#include "sc-move.h"


void
sc_move_vector (ScOrientation o, gint *dx, gint *dy)
{
	*dx = o == SC_HORIZONTAL ? 1 : 0;
	*dy = o == SC_HORIZONTAL ? 0 : 1;
}


