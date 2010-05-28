#include <string.h>
#include <stdlib.h>

#include "sc-rack.h"

#define VALIDATE_LID(lid) {if(lid < 0 || lid >= SC_RACK_SIZE) {g_printerr("Invalid lid '%d'\n", lid); abort();}}

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


/*
gboolean
sc_rack_contains (ScRack *rack, LID lid)
{
	VALIDATE_LID (lid);
	lid = sc_letter_is_blank (lid) ? 0 : sc_letter_value (lid);
	return rack->letters[lid] > 0;
}
*/


void
sc_rack_add (ScRack *rack, LID lid)
{
	VALIDATE_LID (lid);
	lid = sc_letter_is_blank (lid) ? 0 : sc_letter_value (lid);
	rack->letters[lid]++;
}


void
sc_rack_remove (ScRack *rack, LID lid)
{
	VALIDATE_LID (lid);
	lid = sc_letter_is_blank (lid) ? 0 : sc_letter_value (lid);
	if (rack->letters[lid] > 0)
		rack->letters[lid]--;
}



void
sc_rack_assign_letters (ScRack *rack, LID *letters, int n_letters)
{
	memset (rack, '\0', sizeof (ScRack));
	while (--n_letters >= 0) {
//	do {
		LID lid = sc_letter_value(letters[n_letters]);
		VALIDATE_LID (lid);
		rack->letters[lid]++;
	}
//	} while (n_letters > 0);
}


void
sc_rack_to_letters (ScRack *rack, LID *letters, int *n_letters)
{
	int i, j;
	int n = 0;
	for (i = 0; i < 34; i++) {
		for (j = 0; j < rack->letters[i]; j++) {
			letters[n++] = i;
		}
	}
	*n_letters = n;
}


void
sc_rack_print (ScRack *rack, Alphabet *al)
{
	int i, j;
	for (i = 0; i < 34; i++) {
		for (j = 0; j < rack->letters[i]; j++) {
			Letter *l = alphabet_lookup_letter (al, i);
			if (l)
				g_print ("%s ", l->label);
			else
				g_print ("? ");
		}
	}
}



