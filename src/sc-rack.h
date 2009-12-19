#ifndef __SC_RACK_H__
#define __SC_RACK_H__

#include <glib.h>

#include "alphabet.h"

#define SC_RACK_SIZE 64

typedef struct {
	gint letters[SC_RACK_SIZE];
} ScRack;


#define sc_rack_contains(rack,lid) (rack->letters[sc_letter_is_blank (lid) ? 0 : sc_letter_value (lid)] > 0)

ScRack *
sc_rack_new (void);

void
sc_rack_init (ScRack *rack);

ScRack *
sc_rack_copy (ScRack *rack);

void
sc_rack_free (ScRack *rack);

void
sc_rack_assign (ScRack *lhs, ScRack *rhs);

/*
gboolean
sc_rack_contains (ScRack *rack, LID lid);
*/

void
sc_rack_add (ScRack *rack, LID lid);

void
sc_rack_remove (ScRack *rack, LID lid);


void
sc_rack_assign_letters (ScRack *rack, LID *letters, int n_letters);

void
sc_rack_to_letters (ScRack *rack, LID *letters, int *n_letters);


void
sc_rack_print (ScRack *rack, Alphabet *al);


#endif /* __SC_RACK_H__ */
