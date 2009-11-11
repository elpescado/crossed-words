#include <sys/time.h>
#include <time.h>

#include "util.h"

double
foo_microtime (void)
{
	struct timeval t;
	gettimeofday (&t, NULL);
	return (double)t.tv_sec + (double)t.tv_usec/1000000;
}


