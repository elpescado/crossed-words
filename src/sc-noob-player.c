/*
 * Crossed Words
 * -------------
 *
 * Copyright (C) 2009 Przemysław Sitek
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <gtk/gtk.h>
#include "sc-noob-player.h"


G_DEFINE_TYPE (ScNoobPlayer, sc_noob_player, SC_TYPE_COMPUTER_PLAYER)

struct _ScNoobPlayerPrivate
{
	/* Private members go here */
	gfloat level;

	gboolean disposed;
};


#define SC_NOOB_PLAYER_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	SC_TYPE_NOOB_PLAYER, ScNoobPlayerPrivate))


ScNoobPlayer*
sc_noob_player_new (void)
{
	ScNoobPlayer *self = g_object_new (SC_TYPE_NOOB_PLAYER, NULL);
	return self;
}


static void
sc_noob_player_init (ScNoobPlayer *self)
{
	self->priv = SC_NOOB_PLAYER_GET_PRIVATE (self);
	ScNoobPlayerPrivate *priv = self->priv;

	priv->level = 0.7f;

	priv->disposed = FALSE;
}


static ScMove *
_sc_noob_player_analyze_moves (ScComputerPlayer *self)
{
	ScNoobPlayerPrivate *priv = self->priv;

	gint max_score = 0;
	ScMove *best_move = NULL;
	GList *tmp;
	for (tmp = sc_computer_player_get_stored_moves (SC_COMPUTER_PLAYER (self)); tmp; tmp = tmp->next) {
		_MoveProposal *mp = tmp->data;
		if (mp->combined_rating > max_score) {
			max_score = mp->combined_rating;
		}
		best_move = &(mp->move);
	}

	for (tmp = sc_computer_player_get_stored_moves (SC_COMPUTER_PLAYER (self)); tmp; tmp = tmp->next) {
		_MoveProposal *mp = tmp->data;
		if (mp->combined_rating >= priv->level * max_score) {
			return &(mp->move);
		}
	}

	return best_move;
}



static void
sc_noob_player_dispose (GObject *object)
{
	ScNoobPlayer *self = (ScNoobPlayer*) object;
	ScNoobPlayerPrivate *priv = self->priv;


	/* Make sure dispose is called only once */
	if (priv->disposed) {
		return;
	}
	priv->disposed = TRUE;


	/* Chain up to the parent class */
	G_OBJECT_CLASS (sc_noob_player_parent_class)->dispose (object);
}


static void
sc_noob_player_finalize (GObject *object)
{
	G_OBJECT_CLASS (sc_noob_player_parent_class)->finalize (object);
}

	
static void
sc_noob_player_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
	//ScNoobPlayer* self = SC_NOOB_PLAYER (object);
	//ScNoobPlayerPrivate* priv = self->priv;

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
sc_noob_player_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
	//ScNoobPlayer* self = SC_NOOB_PLAYER (object);
	//ScNoobPlayerPrivate* priv = self->priv;

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
sc_noob_player_class_init (ScNoobPlayerClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->get_property = sc_noob_player_get_property;
	gobject_class->set_property = sc_noob_player_set_property;
	gobject_class->dispose = sc_noob_player_dispose;
	gobject_class->finalize = sc_noob_player_finalize;

	SC_COMPUTER_PLAYER_CLASS (klass)->analyze_moves = _sc_noob_player_analyze_moves;

	g_type_class_add_private (klass, sizeof (ScNoobPlayerPrivate));
}

