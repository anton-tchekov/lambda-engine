#include "timer.h"
#include <time.h>

void time_hms(i32 *h, i32 *m, i32 *s)
{
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	*h = tm.tm_hour;
	*m = tm.tm_min;
	*s = tm.tm_sec;
}

void time_ymd_hms(i32 *yy, i32 *mm, i32 *dd, i32 *h, i32 *m, i32 *s)
{
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	*yy = tm.tm_year + 1900;
	*mm = tm.tm_mon + 1;
	*dd = tm.tm_mday;
	*h = tm.tm_hour;
	*m = tm.tm_min;
	*s = tm.tm_sec;
}
