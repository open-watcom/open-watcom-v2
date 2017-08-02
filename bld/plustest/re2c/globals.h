#ifndef _globals_h
#define _globals_h

/* $Log:        GLOBALS.H $
Revision 1.1  92/08/20  15:50:30  Anthony_Scian
.

 * Revision 1.1  1992/08/20  17:14:07  peter
 * Initial revision
 *
 */

#include "basics.h"

extern char *fileName;
extern bool sFlag;
extern bool bFlag;

extern char asc2ebc[256];
extern char ebc2asc[256];
extern uchar xlat[256], talx[256];

#endif
