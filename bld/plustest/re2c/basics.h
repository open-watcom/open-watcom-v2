#ifndef _basics_h
#define _basics_h

/* $Log:        BASICS.H $
Revision 1.4  96/02/08  11:17:18  NT_Test_Machine
.

Revision 1.3  95/03/15  09:11:14  NT_Test_Machine
.

Revision 1.2  95/01/17  09:28:24  NT_Test_Machine
.

Revision 1.1  94/09/14  12:59:42  Anthony_Scian
.

 * Revision 1.1  1992/08/20  18:24:33  peter
 * Initial revision
 *
 */

// useful types

typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned short ushort;
typedef unsigned char uchar;
typedef unsigned char byte;

#ifndef __WATCOM_BOOL__

typedef int bool;

const bool true  = 0 == 0;
const bool false = !true;

#endif

#endif
