/*
 * Scrabble
 * --------
 *
 * Copyright (C) 2009 Przemysław Sitek
 * 
 */

#ifndef __SC_BOARD_H__
#define __SC_BOARD_H__

/* Includes here */
#include <glib.h>

#include "letter.h"
#include "alphabet.h"
#include "sc-move.h"


G_BEGIN_DECLS

#define SC_TYPE_BOARD \
	(sc_board_get_type ())
#define SC_BOARD(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST ((obj), SC_TYPE_BOARD, ScBoard))
#define SC_BOARD_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST ((klass), SC_TYPE_BOARD, ScBoardClass))
#define SC_IS_BOARD(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), SC_TYPE_BOARD)
#define SC_IS_BOARD_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), SC_TYPE_BOARD))
#define SC_BOARD_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), SC_TYPE_BOARD, ScBoardClass))


typedef struct _ScBoard		ScBoard;
typedef struct _ScBoardPrivate		ScBoardPrivate;
typedef struct _ScBoardClass		ScBoardClass;

struct _ScBoard{
	GObject		parent;
	ScBoardPrivate	*priv;
};

struct _ScBoardClass
{
	GObjectClass		parent;

	/* Signals */
};


typedef enum {
	FIELD_MODIFIER_NONE,
	FIELD_MODIFIER_DOUBLE_LETTER,
	FIELD_MODIFIER_TRIPLE_LETTER,
	FIELD_MODIFIER_DOUBLE_WORD,
	FIELD_MODIFIER_TRIPLE_WORD,
} FieldModifier;


GType
sc_board_get_type				(void);

ScBoard*
sc_board_new				(Alphabet *al);


void
sc_board_clear (ScBoard *self);


FieldModifier
sc_board_get_field_modifier (ScBoard *board, gint x, gint y);


Letter *
sc_board_get_letter (ScBoard *board, gint x, gint y);


Alphabet *
sc_board_get_alphabet (ScBoard *self);


void
sc_board_get_needed_tiles (ScBoard *self,
                           ScMove  *move,
                           LID     *tiles,
						   gint    *n_tiles);


gboolean
sc_board_validate_move (ScBoard *self, ScMove *move);


gboolean
sc_board_place_move (ScBoard *self, ScMove *move);


gint
sc_board_rate_move (ScBoard *self, ScMove *move);


G_END_DECLS

#endif
