#ifndef __UTIL_H__
#define __UTIL_H__

#include <glib.h>
#include <stdlib.h>

double
foo_microtime (void);


#define foo_assert(x) do { if (!(x)) { g_printerr ("Assertion `" #x "' failed\n"); abort(); }} while (0)


#endif /* __UTIL_H__ */
