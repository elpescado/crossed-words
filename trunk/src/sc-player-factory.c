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
#include "sc-player-factory.h"


G_DEFINE_TYPE (ScPlayerFactory, sc_player_factory, G_TYPE_OBJECT)

struct _ScPlayerFactoryPrivate
{
	/* Private members go here */
	GHashTable *types;
	gint        n_types;

	gboolean disposed;
};


typedef struct {
	ScPlayerConstructor  constructor;
	gchar               *name;
	gchar               *description;
} _TypeInfo;


#define SC_PLAYER_FACTORY_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	SC_TYPE_PLAYER_FACTORY, ScPlayerFactoryPrivate))


ScPlayerFactory*
sc_player_factory_new (void)
{
	ScPlayerFactory *self = g_object_new (SC_TYPE_PLAYER_FACTORY, NULL);
	return self;
}


static _TypeInfo *
sc_player_factory_create_type_info (ScPlayerConstructor  func,
                                    const gchar         *name,
                                    const gchar         *desc)
{
	_TypeInfo *ti = g_new(_TypeInfo, 1);
	ti->constructor = func;
	ti->name = g_strdup (name);
	ti->description = g_strdup (desc);
	return ti;
}


static void
sc_player_factory_free_type_info (_TypeInfo *ti)
{
	g_free (ti->name);
	g_free (ti->description);
	g_free (ti);
}



static void
sc_player_factory_init (ScPlayerFactory *self)
{
	self->priv = SC_PLAYER_FACTORY_GET_PRIVATE (self);
	ScPlayerFactoryPrivate *priv = self->priv;

	priv->disposed = FALSE;

	priv->types = g_hash_table_new_full (g_str_hash, g_str_equal, NULL, (GDestroyNotify)sc_player_factory_free_type_info);
	priv->n_types = 0;
}


/**
 * Add a type to factory
 * 
 * @param self A factory
 * @param func Function that creates instances of given type
 * @param name A generic type name
 * @param desc Human-readable type description
 */
void
sc_player_factory_add_type (ScPlayerFactory     *self,
                            ScPlayerConstructor  func,
                            const gchar         *name,
                            const gchar         *desc)
{
	ScPlayerFactoryPrivate *priv = self->priv;
	_TypeInfo *ti = sc_player_factory_create_type_info (func, name, desc);
	g_hash_table_replace (priv->types, ti->name, ti);
	priv->n_types++;
}


/**
 * Get all types that can be created by factory
 *
 * Return array of string pairs (name, description)
 * Array should be freed with g_free()
 **/
gchar **
sc_player_factory_get_types (ScPlayerFactory *self)
{
	ScPlayerFactoryPrivate *priv = self->priv;
	gchar ** data = g_new (char*, 2*priv->n_types + 1);
	gint i = 0;

	GHashTableIter iter;
	gpointer key, value;


	g_hash_table_iter_init (&iter, priv->types);
	while (g_hash_table_iter_next (&iter, &key, &value)) 
	{
		_TypeInfo *ti = value;
		data[2*i] =   ti->name;
		data[2*i+1] = ti->description;
		i++;
	}
	data[2*i] = NULL;
	return data;
}


/**
 * Create instance of given type
 * 
 * @param self A factory
 * @param name Type name, as passed to sc_player_factory_add_type
 **/
ScPlayer *
sc_player_factory_create (ScPlayerFactory *self,
                          const gchar     *name)
{
	ScPlayerFactoryPrivate *priv = self->priv;
	_TypeInfo *ti = g_hash_table_lookup (priv->types, name);
	if (ti == NULL || ti->constructor == NULL)
		return NULL;
	return ti->constructor();
}



static void
sc_player_factory_dispose (GObject *object)
{
	ScPlayerFactory *self = (ScPlayerFactory*) object;
	ScPlayerFactoryPrivate *priv = self->priv;


	/* Make sure dispose is called only once */
	if (priv->disposed) {
		return;
	}
	priv->disposed = TRUE;


	/* Chain up to the parent class */
	G_OBJECT_CLASS (sc_player_factory_parent_class)->dispose (object);
}


static void
sc_player_factory_finalize (GObject *object)
{
	G_OBJECT_CLASS (sc_player_factory_parent_class)->finalize (object);
}

	
static void
sc_player_factory_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
	//ScPlayerFactory* self = SC_PLAYER_FACTORY (object);
	//ScPlayerFactoryPrivate* priv = self->priv;

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
sc_player_factory_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
	//ScPlayerFactory* self = SC_PLAYER_FACTORY (object);
	//ScPlayerFactoryPrivate* priv = self->priv;

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}


static void
sc_player_factory_class_init (ScPlayerFactoryClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->get_property = sc_player_factory_get_property;
	gobject_class->set_property = sc_player_factory_set_property;
	gobject_class->dispose = sc_player_factory_dispose;
	gobject_class->finalize = sc_player_factory_finalize;

	g_type_class_add_private (klass, sizeof (ScPlayerFactoryPrivate));
}

