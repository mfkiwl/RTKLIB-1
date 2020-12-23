#include "rtklib.h"

/* dummy application functions for shared library ----------------------------*/
extern int showmsg(char *format,...) {return 0;}
extern void settspan(gtime_t ts, gtime_t te) {}
extern void settime(gtime_t time) {}