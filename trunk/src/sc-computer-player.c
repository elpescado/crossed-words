/*
 * crossed-words
 * -------------
 *
 * Copyright (C) 2009 Przemysław Sitek
 * 
 */
#include <string.h>

#include <gtk/gtk.h>

#include "sc-computer-player.h"
#include "sc-dawg.h"
#include "sc-game.h"
#include "sc-move.h"

G_DEFINE_TYPE (ScComputerPlayer, sc_computer_player, SC_TYPE_PLAYER)

struct _ScComputerPlayerPrivate
{
	/* Private members go here */
	ScDawg *dawg;
	ScDawg *vdawg;

	GList *moves;
	gint   n_moves;

	gboolean disposed;
};



typedef struct {
	ScMove move;
	gint   rating;
} _MoveProposal;


typedef struct {
	ScBoard      *board;
	gint          si;
	gint          sj;
	LID          *letters;
	ScOrientation orient;
} _TraverseCtx;




#define SC_COMPUTER_PLAYER_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	SC_TYPE_COMPUTER_PLAYER, ScComputerPlayerPrivate))


ScComputerPlayer*
sc_computer_player_new (void)
{
	ScComputerPlayer *self = g_object_new (SC_TYPE_COMPUTER_PLAYER, NULL);
	return self;
}


static void
_print_word (ScPlayer *p, LID *letters, gint n_letters)
{
	Alphabet *al = sc_game_get_alphabet (SC_GAME (p->game));
	int i;
	for (i = 0; i < n_letters; i++) {
		Letter *l = alphabet_lookup_letter (al, letters[i]);
		g_print ("%s", l->label);
	}
//	g_print ("\n");
}



void
sc_computer_player_save_move (ScComputerPlayer *self, ScMove *move, gint rating)
{
	ScComputerPlayerPrivate *priv = self->priv;

	_MoveProposal *mp = g_new(_MoveProposal, 1);
	memcpy (&(mp->move), move, sizeof (ScMove));
	mp->rating = rating;

	priv->moves = g_list_prepend (priv->moves, mp);
	priv->n_moves++;
}


void
sc_computer_player_clear_moves (ScComputerPlayer *self)
{
	ScComputerPlayerPrivate *priv = self->priv;
	g_list_foreach (priv->moves, (GFunc)g_free, NULL);
	g_list_free (priv->moves);
	priv->moves = NULL;
	priv->n_moves = 0;
}


static void
_found_word (ScComputerPlayer *self,
            _TraverseCtx      *ctx,
			/*
             ScBoard          *board,
             gint              si,
			 gint              sj,
			 LID              *letters,
			 */
			 gint              left_letters,
			 gint              n_letters)
{
	ScComputerPlayerPrivate *priv = self->priv;
	int i;
	gint di, dj;
	sc_move_vector (ctx->orient, &di, &dj);
	ScMove move;
	move.type = SC_MOVE_TYPE_MOVE;
	move.n_letters = n_letters;
	move.x = ctx->si - di * (left_letters - 1);
	move.y = ctx->sj - dj * (left_letters - 1);
	move.orientation = ctx->orient;//SC_HORIZONTAL;

	/* Copy letters */
	//memcpy (move.letters, letters, sizeof(LID)*n_letters);
	for (i = 0; i < left_letters; i++) {
		move.letters[i] = ctx->letters[left_letters-i-1];
	}
	for (i = left_letters; i < n_letters; i++) {
		move.letters[i] = ctx->letters[i];
	}



	if (0&&! sc_board_validate_move (ctx->board, &move)) {
		g_print ("Huh?\n");
	} else {
		gint rating = sc_board_rate_move (ctx->board, &move);
		gboolean ok = sc_dawg_test_word_translated (priv->vdawg, move.letters, n_letters);
		
		g_print ("Found word ");
		//_print_word (SC_PLAYER (self), letters, n_letters);
		//g_print (" = ");
		_print_word (SC_PLAYER (self), move.letters, n_letters);
		g_print ("(%d, %d)", move.x, move.y);
		//g_print ("[%d, %d] ", left_letters, n_letters);
		g_print (" (%d), %s\n", rating, ok ? "ok" : "bad");


		sc_computer_player_save_move (self, &move, rating);
//		_move_acc_push (priv->moves, &move, rating);
	}
//	sc_game_init_move (SC_GAME (SC_PLAYER(self)->game), si, sj, SC_ORIENTATION_HORIZONTAL, 
}


static gboolean
_check_crosswords (ScComputerPlayer *self,
                   gint              si,
				   gint              sj,
				   ScOrientation     or)
{
	return TRUE;
}


static void
_traverse_tree_right(ScComputerPlayer *self,
                    _TraverseCtx      *ctx,
					 gint              l_idx,
					 gint              idx,
				     ScDawgVertex     *node,
				     ScRack           *rack)
{
	g_printerr (".");

	gint di, dj;
	sc_move_vector (ctx->orient, &di, &dj);

	gint i =  ctx->si + (idx+1)*di;
	gint j =  ctx->sj + (idx+1)*dj;

	if (i < 0 || i >= BOARD_SIZE || j < 0 || j >= BOARD_SIZE)
		return;

//	g_print ("Exploring %d, %d\n",               ctx->si + (idx+1)*di, ctx->sj + (idx+1)*dj);
	Letter *l = sc_board_get_letter (ctx->board, i, j);
	if (l) {
		/* There is a tile on board in this place... */
		LID lid = l->index;

		ScDawgVertex *v2 = sc_dawg_vertex_child (node, lid);
		if (v2) {
			ctx->letters[l_idx+idx] = lid;
			if (sc_dawg_vertex_is_final (v2)) {
				_found_word (self, ctx/*->board, ctx->si, ctx->sj, ctx->letters*/, l_idx, idx+1+l_idx);
			}
			_traverse_tree_right (self, ctx, l_idx, idx+1, v2, rack);
		}
	} else {
		gint i;
		for (i = 0; i < node->n_arcs; i++) {
			ScDawgArc *a = node->first_arc+i;
			LID lid = a->lid;
			if (sc_rack_contains (rack, lid)
				&& _check_crosswords (self, i, j, !ctx->orient)) {
				ScDawgVertex *v2 = a->dest;
				ctx->letters[l_idx+idx] = lid;
				if (sc_dawg_vertex_is_final (v2)) {
					_found_word (self, ctx/*->board, ctx->si, ctx->sj, ctx->letters*/, l_idx, idx+1+l_idx);
				}

				sc_rack_remove (rack, lid);
				_traverse_tree_right (self, ctx, l_idx, idx+1, v2, rack);
				sc_rack_add (rack, lid);
			}
		}
	}
}



static void
_traverse_tree_left (ScComputerPlayer *self,
                    _TraverseCtx      *ctx,
					 gint              idx,
				     ScDawgVertex     *node,
				     ScRack           *rack)
{
	g_printerr (".");

	gint di, dj;
	sc_move_vector (ctx->orient, &di, &dj);

	gint i =  ctx->si-+ idx*di;
	gint j =  ctx->sj - idx*dj;
//	g_print ("Exploring %d, %d\n",               ctx->si-+ idx*di, ctx->sj - idx*dj);
	Letter *l;
	if ((i > 0) && (j > 0) && (l = sc_board_get_letter (ctx->board, i, j))) {
		/* There is a tile on board in this place... */
		LID lid = l->index;

		ScDawgVertex *v2 = sc_dawg_vertex_child (node, lid);
		if (v2) {
			ctx->letters[idx] = lid;
			if (sc_dawg_vertex_is_final (v2)) {
				_found_word (self, ctx/*->board, ctx->si, ctx->sj, ctx->letters*/, idx+1, idx+1);
			}
			_traverse_tree_left (self, ctx, idx+1, v2, rack);
		}
	} else {
		gint i;
		for (i = 0; i < node->n_arcs; i++) {
			ScDawgArc *a = node->first_arc+i;
			LID lid = a->lid;
			if (lid == 0) {
				/* Flip marker */
				
				/*
				g_print ("Traversing right @%d, so far: ", idx);
				_print_word (SC_PLAYER(self), ctx->letters, idx);
				g_print ("\n");
				*/
				

				_traverse_tree_right (self, ctx, idx/*+1*/, 0, a->dest, rack);	

			} else if (i > 0 && j > 0
				&& sc_rack_contains (rack, lid)
				&& _check_crosswords (self, i, j, !ctx->orient)) {
				ScDawgVertex *v2 = a->dest;
				ctx->letters[idx] = lid;
				if (sc_dawg_vertex_is_final (v2)) {
					_found_word (self, ctx/*->board, ctx->si, ctx->sj, ctx->letters*/, idx+1, idx+1);
				}

				sc_rack_remove (rack, lid);
				_traverse_tree_left (self, ctx, idx+1, v2, rack);
				sc_rack_add (rack, lid);
			}
		}
	}
}


static void
sc_computer_player_explore_anchor_square (ScComputerPlayer *self,
                                          ScBoard          *board,
                                          gint              si,
										  gint              sj)
{
	ScComputerPlayerPrivate *priv = self->priv;

	Letter *l =  sc_board_get_letter (board, si, sj);
	ScDawgVertex *root = sc_dawg_root (priv->dawg);
	ScDawgVertex *v = sc_dawg_vertex_child (root, l->index);
	if (v == NULL)
		return;

	ScRack rack;
	sc_player_get_rack (SC_PLAYER (self), &rack);
	LID letters[15];

	letters[0] = l->index;
	_TraverseCtx ctx_h = {board, si, sj, letters, SC_HORIZONTAL};
	_traverse_tree_left (self, &ctx_h, 1, v, &rack);

	letters[0] = l->index;
	_TraverseCtx ctx_v = {board, si, sj, letters, SC_VERTICAL};
	_traverse_tree_left (self, &ctx_v, 1, v, &rack);
	
}


static ScMove *
sc_computer_player_analyze_moves (ScComputerPlayer *self)
{
	ScComputerPlayerPrivate *priv = self->priv;

	gint max_score = 0;
	ScMove *best_move = NULL;
	GList *tmp;
	for (tmp = priv->moves; tmp; tmp = tmp->next) {
		_MoveProposal *mp = tmp->data;
		if (mp->rating > max_score) {
			max_score = mp->rating;
			best_move = &(mp->move);
		}
	}
	return best_move;
}


static void
sc_computer_player_your_turn (ScComputerPlayer *self)
{

	ScMove move;
	ScBoard *board;
	int i,j;

	board = sc_player_get_board (SC_PLAYER (self));
	ScComputerPlayerPrivate *priv = self->priv;

	Alphabet *al = sc_game_get_alphabet (SC_GAME (SC_PLAYER(self)->game));
	ScRack rack;
	sc_player_get_rack (SC_PLAYER (self), &rack);
	g_print ("My rack: ");
	sc_rack_print (&rack, al);
	g_print ("\n");
	
	gint anchor_squares = 0;

	g_printerr ("searching");
	/* Scan for anchor squares */
	for (i = 0; i < BOARD_SIZE; i++) {
		for (j = 0; j < BOARD_SIZE; j++) {
			Letter *l = sc_board_get_letter (board, i, j);

			if (l != NULL) {
				g_print ("Found anchor square '%s' @ %d, %d\n", l->label, i, j);
				sc_computer_player_explore_anchor_square (self, board, i, j);
				anchor_squares++;
			}
		}
	}
	if (anchor_squares == 0) {
		/* First move */
		ScDawgVertex *root = sc_dawg_root (priv->dawg);
		ScRack rack;
		sc_player_get_rack (SC_PLAYER (self), &rack);
		LID letters[15];
		_TraverseCtx ctx_h = {board, 7, 7, letters, SC_HORIZONTAL};
		_traverse_tree_left (self, &ctx_h, 0, root, &rack);

	}
	g_printerr ("\ndone\n");
	g_print ("%d moves found\n", priv->n_moves);


	ScMove *m_move = sc_computer_player_analyze_moves (self);
	if (m_move && sc_player_do_move (SC_PLAYER (self), m_move)) {
		g_print ("OK\n");
	} else {
		g_print ("Giving up\n");
		move.type = SC_MOVE_TYPE_PASS;
		sc_player_do_move (SC_PLAYER (self), &move);
	}

	/* Cleanup */
}



static void
sc_computer_player_init (ScComputerPlayer *self)
{
	self->priv = SC_COMPUTER_PLAYER_GET_PRIVATE (self);
	ScComputerPlayerPrivate *priv = self->priv;

	g_signal_connect (self, "your-turn",
	                  G_CALLBACK (sc_computer_player_your_turn), self);

	priv->dawg = sc_dawg_load ("gaddag.dag");
	priv->vdawg = sc_dawg_load ("dawg.dag");

	priv->disposed = FALSE;
}


static void
sc_computer_player_dispose (GObject *object)
{
	ScComputerPlayer *self = (ScComputerPlayer*) object;
	ScComputerPlayerPrivate *priv = self->priv;


	/* Make sure dispose is called only once */
	if (priv->disposed) {
		return;
	}
	priv->disposed = TRUE;


	/* Chain up to the parent class */
	G_OBJECT_CLASS (sc_computer_player_parent_class)->dispose (object);
}


static void
sc_computer_player_finalize (GObject *object)
{
	G_OBJECT_CLASS (sc_computer_player_parent_class)->finalize (object);
}

	
static void
sc_computer_player_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
	//ScComputerPlayer* self = SC_COMPUTER_PLAYER (object);
	//ScComputerPlayerPrivate* priv = self->priv;

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
sc_computer_player_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
	//ScComputerPlayer* self = SC_COMPUTER_PLAYER (object);
	//ScComputerPlayerPrivate* priv = self->priv;

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
sc_computer_player_class_init (ScComputerPlayerClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->get_property = sc_computer_player_get_property;
	gobject_class->set_property = sc_computer_player_set_property;
	gobject_class->dispose = sc_computer_player_dispose;
	gobject_class->finalize = sc_computer_player_finalize;

	g_type_class_add_private (klass, sizeof (ScComputerPlayerPrivate));
}

