/* GetTime.h: Provide Current Time */
/* Author: Yifan Xu(Max) */
/* Version: 1.0, 03/15/2017 */
/* V1.0: File Create */

#include "GetTime.h"

/* Time accruate system */
time_t GetCurrentTime(void)	/*  print/update the current real time */
{
	time_t CurrentTime; /* seconds since 1970 (see 'man 2 time') */
	char   *TimeString;	/* printable time string (see 'man ctime') */

	CurrentTime = time(NULL);	/* get current real time (in seconds) */
	TimeString = ctime(&CurrentTime);	/* convert to printable format */
	/* strncpy(ClockBuffer, TimeString, 25); */
	/* ClockBuffer[24] = 0; */
	return CurrentTime;
} /* end of PrintCurrentTime */