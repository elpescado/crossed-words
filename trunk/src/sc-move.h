#ifndef __SC_MOVE_H__
#define __SC_MOVE_H__

#include <glib.h>

#include "letter.h"


/**
 * Word orientation
 **/
typedef enum {
	SC_HORIZONTAL,	/**< Left-to-right */
	SC_VERTICAL		/**< Top-to-bottom */
} ScOrientation;


/**
 * Move type
 **/
typedef enum {
	SC_MOVE_TYPE_MOVE,		/**< Normal move   */
	SC_MOVE_TYPE_EXCHANGE,	/**< Rack exchange */
	SC_MOVE_TYPE_PASS		/**< Pass          */
} ScMoveType;

typedef struct {
	ScMoveType type;
	LID   letters[15];
	gint  n_letters;
	gint  x;
	gint  y;
	ScOrientation  orientation;
} ScMove;

#endif /* __SC_MOVE_H__ */
