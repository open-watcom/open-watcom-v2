/* internal move routines 
   (c) 1992 John Tromp
*/

#include "c4.h"

static B8 linit = {7,6,5,4,0,3,2,1},
          rinit = {4,7,6,5,3,2,1,0};
static uint8 msafe[43];
static uint8 four[128];
static uint16 xrows[8], orows[8], xdias[21], odias[21];
static uint8 initdias[21] = {0,0,0,1,2,3,5,6,7,0,0,0,1,2,3,5,6,7,0,0,0};
static uint8 typemask[8] = {0, 0x78, 0x7c, 0x7e, 0x7f, 0x3f, 0x1f, 0x0f};
static uint8 thrcols[1024];

int plycnt;
B8 l, r, columns, height;
uint8 *moves = msafe+1;
uint8 xthrcnt[64],othrcnt[64],colthr[128];

void inithrcols()     /* compute threat columns */
{
  int i,j,m,t;

  for (i = 0; i < 0x80; i++)
    for (j = 0x80 | i; j >= 0x10; j >>= 1)
      if ((j & 0xf) == 0xf) {
        four[i] = 1;
        break;
      }
  for (i = 0; i < 0x80; i++)
    if (!four[i])				/* no 4 connected */
      for (j = 1, m = 0x80; m >>= 1; j++)	/* try 4th stone */
        if (four[i | m])			/* threat */
          for (t = 1; t < 8; t++)		/* update relevant types */
            if (typemask[t] & m)
              thrcols[t << 7 | i] = thrcols[t << 7 | i] << 3 | j;
}

void initcolthr()     /* compute column threats */
{
  int i;

  for (i=0x8; i<0x40; i+=8) {
    colthr[i] = 1;		/* threat for player2?! */
    colthr[i+7] = 2;		/* threat for player1?! */
  }
}

void newgame()
{
  int i;

  plycnt = 0;
  for (i=0; i<21; i++)
    xdias[i] = odias[i] = initdias[i] << 7;
  for (i=0; i<8; i++) {
    height[i] = 7*8 + i;
    xrows[i] = orows[i] = 4 << 7;
    columns[i] = 1;
    l[i] = linit[i];
    r[i] = rinit[i];
  }
  for (i=0; i<64; i++) {
    othrcnt[i] = xthrcnt[i] = 0;
  }
}

void initplay()
{
  inithrcols();
  initcolthr();
  newgame();
}

int depth(n)	/* returns 0 (full) through 6 (empty) */
int n;
{
  return (height[n]>>3) - 1;
}

int haswon()
{
  if (plycnt < 7)
    return 0;
  return ((plycnt & 1) ? othrcnt : xthrcnt)[height[LASTMOVE]+8];
}

#define BACKMOV	backmovx
#define MAKEMOV	makemovx
#define ROWS	xrows
#define DIAS	xdias
#define THRCNT	xthrcnt
#define BIT
#include "play.h"

#define BACKMOV	backmovo
#define MAKEMOV	makemovo
#define ROWS	orows
#define DIAS	odias
#define THRCNT	othrcnt
#define BIT	| 1
#include "play.h"
