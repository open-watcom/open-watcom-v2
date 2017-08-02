#ifndef _basics_h
#define _basics_h


//Revision 1.1  1994/04/08  15:27:59  peter
//Initial revision
//

#define bool BOOL
#define true TRUE
#define false FALSE

typedef unsigned int bool;
const bool true = (0 == 0);
const bool false = (!true);

typedef unsigned int 	uint;
typedef unsigned char 	uchar, byte;
typedef unsigned short 	ushort, word;
typedef unsigned long 	ulong, dword;

#endif
