#include <string.h>

#include "sc-rack.h"

ScRack *
sc_rack_new (void)
{
	return g_new0 (ScRack, 1);
}


void
sc_rack_init (ScRack *rack)
{
	memset (rack, '\0', sizeof (ScRack));
}


ScRack *
sc_rack_copy (ScRack *rack)
{
	ScRack *self = g_new (ScRack, 1);
	memcpy (self, rack, sizeof (ScRack));
	return self;
}


void
sc_rack_free (ScRack *rack)
{
	g_free (rack);
}


void
sc_rack_assign (ScRack *lhs, ScRack *rhs)
{
	memcpy (lhs, rhs, sizeof (ScRack));
}


gboolean
sc_rack_contains (ScRack *rack, LID lid)
{
	return rack->letters[lid] > 0;
}


void
sc_rack_add (ScRack *rack, LID lid)
{
	rack->letters[lid]++;
}


void
sc_rack_remove (ScRack *rack, LID lid)
{
	if (rack->letters[lid] > 0)
		rack->letters[lid]--;
}



void
sc_rack_assign_letters (ScRack *rack, LID *letters, int n_letters)
{
	memset (rack, '\0', sizeof (ScRack));
	do {
		rack->letters[letters[--n_letters]]++;
	} while (n_letters > 0);
}



void
sc_rack_print (ScRack *rack, Alphabet *al)
{
	int i, j;
	for (i = 0; i < 34; i++) {
		for (j = 0; j < rack->letters[i]; j++) {
			Letter *l = alphabet_lookup_letter (al, i);
			g_print ("%s ", l->label);
		}
	}
}



