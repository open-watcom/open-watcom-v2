/* type-definitions
   (c) 1992 John Tromp
*/

#include <sys/types.h>
#include <stdio.h>
extern void exit();

extern char *calloc();
#define allocate(N, T)  ((T *)calloc((unsigned)N,sizeof(T)))
#define UNK		2
#define WIN2		3
#define DRIN2		4
#define DRAW		5
#define DRIN1		6
#define WIN1		7
#define EXACT(S)	((S)==WIN2 || (S)==DRAW || (S)==WIN1)
#define SCORE(X)	((X) & 7)
#define WORK(X)		((X) >> 3)
#define LASTMOVE        moves[plycnt-1]

typedef unsigned char  u_char;
typedef unsigned short u_short;
typedef unsigned int   u_int;

#ifdef SMALL
typedef unsigned char  uint8;
typedef unsigned short uint16;
#else
typedef int uint8;
typedef int uint16;
#endif
typedef uint8 B8[8];
typedef uint8 hashentry;
#ifdef MOD64
typedef long long uint64;
#endif
