/*
 * Scrabble
 * --------
 *
 * Copyright (C) 2009 Przemysław Sitek
 * 
 */

#include <string.h>

#include <gtk/gtk.h>
#include "sc-board.h"

#include "letter.h"
#include "common.h"



G_DEFINE_TYPE (ScBoard, sc_board, G_TYPE_OBJECT)

struct _ScBoardPrivate
{
	/* Private members go here */

	Alphabet *al;
	LID       letters[BOARD_SIZE*BOARD_SIZE];
	char      current_modifiers[BOARD_SIZE*BOARD_SIZE];	/**< Array of board modifiers that has not been used */

	gboolean disposed;
};


#define SC_BOARD_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	SC_TYPE_BOARD, ScBoardPrivate))


static char
board_modifiers[] = ""
"T..d...T...d..T"
".D...t...t...D."
"..D...d.d...D.."
"d..D...d...D..d"
"....D.....D...."
".t...t...t...t."
"..d...d.d...d.."
"T..d...D...d..T"
"..d...d.d...d.."
".t...t...t...t."
"....D.....D...."
"d..D...d...D..d"
"..D...d.d...D.."
".D...t...t...D."
"T..d...T...d..T";


ScBoard*
sc_board_new (Alphabet *al)
{
	ScBoard *self = g_object_new (SC_TYPE_BOARD, NULL);

	self->priv->al = al;


	return self;
}


static void
sc_board_init (ScBoard *self)
{
	self->priv = SC_BOARD_GET_PRIVATE (self);
	ScBoardPrivate *priv = self->priv;

	priv->letters[109] = 25;
	priv->letters[110] = 5;
	priv->letters[111] = 24;
	priv->letters[112] = 2;
	priv->letters[113] = 4;
	priv->letters[114] = 4;
	priv->letters[115] = 16;
	priv->letters[116] = 8;


	memcpy (priv->current_modifiers, board_modifiers, sizeof(priv->current_modifiers));

	priv->disposed = FALSE;
}


void
sc_board_clear (ScBoard *self)
{
	ScBoardPrivate *priv = self->priv;

	memset (priv->letters, 0, BOARD_SIZE*BOARD_SIZE * sizeof(LID));
}


FieldModifier
sc_board_get_field_modifier (ScBoard *board, gint x, gint y)
{
	gint pos = y * BOARD_SIZE + x;

	switch (board_modifiers[pos]) {
		case 'd': return FIELD_MODIFIER_DOUBLE_LETTER;
		case 't': return FIELD_MODIFIER_TRIPLE_LETTER;
		case 'D': return FIELD_MODIFIER_DOUBLE_WORD;
		case 'T': return FIELD_MODIFIER_TRIPLE_WORD;
		default: return FIELD_MODIFIER_NONE;
	}
}


LID
sc_board_get_lid (ScBoard *board, gint x, gint y)
{
	ScBoardPrivate *priv = board->priv;
	gint pos = y * BOARD_SIZE + x;

	return priv->letters[pos];
}


Letter *
sc_board_get_letter (ScBoard *board, gint x, gint y)
{
	ScBoardPrivate *priv = board->priv;

	LID idx = sc_board_get_lid (board, x, y);
	return alphabet_lookup_letter (priv->al, idx);
}



Alphabet *
sc_board_get_alphabet (ScBoard *self)
{
	ScBoardPrivate *priv = self->priv;

	return priv->al;
}


/**
 * Get a list of tiles that are necessary to form a move
 **/
void
sc_board_get_needed_tiles (ScBoard *self,
                           ScMove  *move,
                           LID     *tiles,
						   gint    *n_tiles)
{
	ScBoardPrivate *priv = self->priv;

	int n = 0;

	int di = (move->orientation == SC_HORIZONTAL) ? 1 : 0;
	int dj = (move->orientation == SC_HORIZONTAL) ? 0 : 1;
	int k;

	for (k = 0; k < move->n_letters; k++) {
		int i = move->x + k*di;
		int j = move->y + k*dj;
		int pos = j * BOARD_SIZE + i;

		if (priv->letters[pos] == 0) {
			/* We need tile move->letters[k] */
			tiles[n++] = move->letters[k];
			g_print ("We need %d\n", move->letters[k]);
		}
	}

	if (n_tiles)
		*n_tiles = n;
}



/**
 * Check whether a move is valid
 **/
gboolean
sc_board_validate_move (ScBoard *self, ScMove *move)
{
	ScBoardPrivate *priv = self->priv;

	int di = (move->orientation == SC_HORIZONTAL) ? 1 : 0;
	int dj = (move->orientation == SC_HORIZONTAL) ? 0 : 1;
	int k;

	for (k = 0; k < move->n_letters; k++) {
		int i = move->x + k*di;
		int j = move->y + k*dj;
		int pos = j * BOARD_SIZE + i;

		if (priv->letters[pos] != 0) {
			if (priv->letters[pos] != move->letters[k]) {
				// incompatible tiles
				return FALSE;
			}
		}
	}

	/* Validate - a big TODO */
	return TRUE;
}


/**
 * Place a move on a board
 **/
gboolean
sc_board_place_move (ScBoard *self, ScMove *move)
{
	ScBoardPrivate *priv = self->priv;

	if (! sc_board_validate_move (self, move))
		return FALSE;

	int di = (move->orientation == SC_HORIZONTAL) ? 1 : 0;
	int dj = (move->orientation == SC_HORIZONTAL) ? 0 : 1;
	int k;

	for (k = 0; k < move->n_letters; k++) {
		int i = move->x + k*di;
		int j = move->y + k*dj;
		int pos = j * BOARD_SIZE + i;

		g_print ("Placing %d @ %d, %d (%d), right now %d\n", move->letters[k], i, j, pos, priv->letters[pos]);

		if (priv->letters[pos] == 0)
			priv->letters[pos] = move->letters[k];
	}
	return TRUE;
}

/*
static gboolean
sc_board_get_tile (ScBoard *self, ScMove *move, int i, int j)
{
	ScBoardPrivate *priv = self->priv;

	return TRUE;
}
*/

static gint
sc_board_rate_prefix (ScBoard *self, int i, int j, int di, int dj, gboolean *has)
{
	ScBoardPrivate *priv = self->priv;
	gint rating = 0;

	for (i-=di, j-=dj; i >= 0 && j >= 0; i-=di, j-=dj) {
		int pos = j * BOARD_SIZE + i;
	
		LID lid = priv->letters[pos];
		if (lid == 0)
			break;

		if (! sc_letter_is_blank (lid)) {
			Letter *l = alphabet_lookup_letter (priv->al, lid);
			rating += l->value;
		}
		if (has) *has = TRUE;
	}

	return rating;
}



static gint
sc_board_rate_suffix (ScBoard *self, int i, int j, int di, int dj, gboolean *has)
{
	ScBoardPrivate *priv = self->priv;
	gint rating = 0;

	for (i+=di, j+=dj; i < BOARD_SIZE && j < BOARD_SIZE; i+=di, j+=dj) {
		int pos = j * BOARD_SIZE + i;
	
		LID lid = priv->letters[pos];
		if (lid == 0)
			break;

		if (! sc_letter_is_blank (lid)) {
			Letter *l = alphabet_lookup_letter (priv->al, lid);
			rating += l->value;
		}
		if (has) *has = TRUE;
	}

	return rating;
}



/**
 * Rates a given move
 **/
gint
sc_board_rate_move (ScBoard *self, ScMove *move)
{
	ScBoardPrivate *priv = self->priv;

	int rating = 0;
	int multiplier = 1;

	int di = (move->orientation == SC_HORIZONTAL) ? 1 : 0;
	int dj = (move->orientation == SC_HORIZONTAL) ? 0 : 1;
	int k;

	gint needed_tiles = 0;

	/* Rate "main" word */

	/* main part */
	for (k = 0; k < move->n_letters; k++) {
		int i = move->x + k*di;
		int j = move->y + k*dj;
		int pos = j * BOARD_SIZE + i;
		Letter *l = alphabet_lookup_letter (priv->al, move->letters[k]);
		FieldModifier fm = sc_board_get_field_modifier (self, i, j);
		//g_printerr ("rate_move (%d -> %p)\n", move->letters[k], l);

		gboolean is_blank =
			(priv->letters[pos] == 0) 
			 ? sc_letter_is_blank(move->letters[k])
			 : sc_letter_is_blank(priv->letters[pos]);

		int letter_value = is_blank ? 0 : l->value;
		if (priv->letters[pos] == 0) {
			needed_tiles++;
			if (fm == FIELD_MODIFIER_DOUBLE_LETTER) {
				letter_value *= 2;
			} else if (fm == FIELD_MODIFIER_TRIPLE_LETTER) {
				letter_value *= 3;
			}

			if (fm == FIELD_MODIFIER_DOUBLE_WORD) {
				multiplier *= 2;
			} else if (fm == FIELD_MODIFIER_TRIPLE_WORD) {
				multiplier *= 3;
			}
		}
		rating += letter_value;
	}

	if (needed_tiles == 0)
		return 0;

	/* Prefix */
	rating += sc_board_rate_prefix (self, move->x, move->y, di, dj, NULL);
	rating += sc_board_rate_suffix (self, move->x + di*(move->n_letters-1), move->y + dj*(move->n_letters-1), di, dj, NULL);

	/* Crosswords */
	int di2 = (move->orientation == SC_HORIZONTAL) ? 0 : 1;
	int dj2 = (move->orientation == SC_HORIZONTAL) ? 1 : 0;
	for (k = 0; k < move->n_letters; k++) {
		int i = move->x + k*di;
		int j = move->y + k*dj;
		int pos = j * BOARD_SIZE + i;
		Letter *l = alphabet_lookup_letter (priv->al, move->letters[k]);
		if (priv->letters[pos] == 0) {
			gboolean has_prefix = FALSE;
			gboolean has_suffix = FALSE;
			int r1 = sc_board_rate_prefix (self, i, j, di2, dj2, &has_prefix);
			int r2 = sc_board_rate_suffix (self, i, j, di2, dj2, &has_suffix);
			int r0 = sc_letter_is_blank (move->letters[k]) ? 0 : l->value;

			if (has_prefix || has_suffix) {
				int r = r1 + r2 + r0;
				rating += r;
				//g_print ("Crossword: %d", r);
			}
		}
	}



	rating *= multiplier;
	

	if (needed_tiles == 7)
		rating += 50; // bingo bonus


	return rating;
}


/**
 * Rates a given move
 **/
gint
sc_board_rate_move2 (ScBoard *self, ScMove *move)
{
	/* TODO: this rating mechanism is over-simplified */
	ScBoardPrivate *priv = self->priv;

	int rating = 0;
	int multiplier = 1;

	int di = (move->orientation == SC_HORIZONTAL) ? 1 : 0;
	int dj = (move->orientation == SC_HORIZONTAL) ? 0 : 1;
	int k;

	for (k = 0; k < move->n_letters; k++) {
		int i = move->x + k*di;
		int j = move->y + k*dj;
		int pos = j * BOARD_SIZE + i;
		Letter *l = alphabet_lookup_letter (priv->al, move->letters[k]);
		FieldModifier fm = sc_board_get_field_modifier (self, i, j);


		if (priv->letters[pos] == 0) {
			int letter_value = l->value;
			if (fm == FIELD_MODIFIER_DOUBLE_LETTER) {
				letter_value *= 2;
			} else if (fm == FIELD_MODIFIER_TRIPLE_LETTER) {
				letter_value *= 3;
			}
			g_print ("Rating for %s - %d points\n", l->label, letter_value);
			rating += letter_value;
		}

		if (fm == FIELD_MODIFIER_DOUBLE_WORD) {
			multiplier *= 2;
		} else if (fm == FIELD_MODIFIER_TRIPLE_WORD) {
			multiplier *= 3;
		}
	}
	rating *= multiplier;
	return rating;

}


static void
sc_board_dispose (GObject *object)
{
	ScBoard *self = (ScBoard*) object;
	ScBoardPrivate *priv = self->priv;


	/* Make sure dispose is called only once */
	if (priv->disposed) {
		return;
	}
	priv->disposed = TRUE;


	/* Chain up to the parent class */
	G_OBJECT_CLASS (sc_board_parent_class)->dispose (object);
}


static void
sc_board_finalize (GObject *object)
{
	G_OBJECT_CLASS (sc_board_parent_class)->finalize (object);
}

	
static void
sc_board_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
	ScBoard* self = SC_BOARD (object);
	G_UNUSED (self);

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
sc_board_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
	ScBoard* self = SC_BOARD (object);
	G_UNUSED (self);

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
sc_board_class_init (ScBoardClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->get_property = sc_board_get_property;
	gobject_class->set_property = sc_board_set_property;
	gobject_class->dispose = sc_board_dispose;
	gobject_class->finalize = sc_board_finalize;

	g_type_class_add_private (klass, sizeof (ScBoardPrivate));
}

