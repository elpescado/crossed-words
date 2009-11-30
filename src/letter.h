#ifndef __LETTER_H__
#define __LETTER_H__


#include <glib.h>

/**
 * Letter ID
 */
typedef int LID;

typedef struct {
	LID  index;		/**< Letter index                     */
	int  value;		/**< Point value for letter           */
	int  count;		/**< Number of tiles with that letter */
	char label[4];		/**< Unicode letter label             */
} Letter;

#define BLANK 128

/**
 * Check whether letter is blank
 **/
#define sc_letter_is_blank(l) (l & BLANK)

/**
 * Get letter index value
 **/
#define sc_letter_value(lid)  (lid & 0x7f)


#endif /* __LETTER_H__ */
