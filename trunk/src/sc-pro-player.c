/*
 * Crossed Words
 * -------------
 *
 * Copyright (C) 2009 Przemysław Sitek
 * 
 */

#include <gtk/gtk.h>
#include "sc-pro-player.h"
#include "sc-game.h"


G_DEFINE_TYPE (ScProPlayer, sc_pro_player, SC_TYPE_COMPUTER_PLAYER)

struct _ScProPlayerPrivate
{
	/* Private members go here */
	gint moves_to_consider;

	gboolean disposed;
};


#define SC_PRO_PLAYER_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	SC_TYPE_PRO_PLAYER, ScProPlayerPrivate))


ScProPlayer*
sc_pro_player_new (void)
{
	ScProPlayer *self = g_object_new (SC_TYPE_PRO_PLAYER, NULL);
	return self;
}


static void
sc_pro_player_init (ScProPlayer *self)
{
	self->priv = SC_PRO_PLAYER_GET_PRIVATE (self);
	ScProPlayerPrivate *priv = self->priv;

	priv->disposed = FALSE;
}


static void
sc_pro_player_calculate_opponent_rack (ScComputerPlayer *self,
                                       ScBoard          *board,
                                       Alphabet         *al,
                                       ScRack           *rack)
{
	int i, j;
	ScRack my_rack;

	sc_player_get_rack (SC_PLAYER (self), &my_rack);

	rack->letters[0] = 2; // Two blanks

	/* Init rack with all tiles */
	for (i = 0; i < al->n_letters; i++) {
		Letter *l = al->letters+i;
		rack->letters[l->index] = l->count;
	}

	/* Subtract tiles on board */
	for (i = 0; i < BOARD_SIZE; i++) {
		for (j = 0; j < BOARD_SIZE; j++) {
			LID lid = sc_board_get_lid (board, i, j);
			if (lid != 0) {
				if (sc_letter_is_blank (lid)) {
					sc_rack_remove (rack, 0);
				} else {
					sc_rack_remove (rack, lid);
				}
			}
		}
	}

	/* Subtract players rack */
	for (i = 0; i < al->n_letters; i++) {
		rack->letters[i] -= my_rack.letters[i];
	}
}


static void
sc_pro_player_do_endgame_move (ScProPlayer *self)
{
	ScComputerPlayerPrivate *priv = self->priv;
	ScBoard *board;
	ScMove move;
	ScRack my_rack;
	ScRack opponent_rack;
	Alphabet *al = sc_game_get_alphabet (SC_GAME (SC_PLAYER(self)->game));
	GList *moves;
	GList *node;

	sc_player_get_rack (SC_PLAYER (self), &my_rack);
	sc_pro_player_calculate_opponent_rack (self, board, al, &opponent_rack);

	board = sc_player_get_board (SC_PLAYER (self));
	
	
	sc_computer_player_generate_moves (SC_COMPUTER_PLAYER (self), board, &my_rack);
	moves = sc_computer_player_steal_stored_moves (SC_COMPUTER_PLAYER (self));

	for (node = moves; node; node = node->next) {
		_MoveProposal *mp = node->data;


	}
}




static void
sc_pro_player_simulate_move (ScComputerPlayer *self,
                             ScMove           *move)
{
}


/**
 * Analyze moves
 **/
static ScMove *
_sc_pro_player_analyze_moves (ScComputerPlayer *self)
{
	ScProPlayerPrivate *priv = self->priv;

	gint max_score = 0;
	ScMove *best_move = NULL;
	GList *tmp;
	gint i;

	sc_computer_player_sort_moves (SC_COMPUTER_PLAYER (self));

	for (tmp = sc_computer_player_get_stored_moves (SC_COMPUTER_PLAYER (self)), i = 0;
	     tmp && i < priv->moves_to_consider; tmp = tmp->next, i++) {
	}


	for (tmp = sc_computer_player_get_stored_moves (SC_COMPUTER_PLAYER (self));
	     tmp; tmp = tmp->next) {
		_MoveProposal *mp = tmp->data;
		if (mp->rating > max_score) {
			max_score = mp->rating;
			best_move = &(mp->move);
		}
	}
	return best_move;
}



static void
sc_pro_player_dispose (GObject *object)
{
	ScProPlayer *self = (ScProPlayer*) object;
	ScProPlayerPrivate *priv = self->priv;


	/* Make sure dispose is called only once */
	if (priv->disposed) {
		return;
	}
	priv->disposed = TRUE;


	/* Chain up to the parent class */
	G_OBJECT_CLASS (sc_pro_player_parent_class)->dispose (object);
}


static void
sc_pro_player_finalize (GObject *object)
{
	G_OBJECT_CLASS (sc_pro_player_parent_class)->finalize (object);
}

	
static void
sc_pro_player_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
	//ScProPlayer* self = SC_PRO_PLAYER (object);
	//ScProPlayerPrivate* priv = self->priv;

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
sc_pro_player_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
	//ScProPlayer* self = SC_PRO_PLAYER (object);
	//ScProPlayerPrivate* priv = self->priv;

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
sc_pro_player_class_init (ScProPlayerClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->get_property = sc_pro_player_get_property;
	gobject_class->set_property = sc_pro_player_set_property;
	gobject_class->dispose = sc_pro_player_dispose;
	gobject_class->finalize = sc_pro_player_finalize;

	SC_COMPUTER_PLAYER_CLASS (klass)->analyze_moves = _sc_pro_player_analyze_moves;

	g_type_class_add_private (klass, sizeof (ScProPlayerPrivate));
}

