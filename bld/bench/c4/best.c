/* alpha beta search
   (c) 1992 John Tromp
*/

#include "c4.h"

#define BONUS		1

int window;	/* 0 = [-,+]    1 = [=,+]   2 = [-,=] */
static uint8 alp[] = {WIN2, DRAW, WIN2},
             bet[] = {WIN1, WIN1, DRAW};
static uint8 xkiller[64], okiller[64];

extern B8 r, columns, height;
extern uint8 xthrcnt[], othrcnt[], colthr[], *moves;
extern int plycnt;
extern u_int posed;
extern void makemovx(),makemovo(),backmovx(),backmovo();
extern int transpose();
extern void transtore(),transrestore(),emptyht();

u_int nodes;

void initbest()
{
}

static FILE *BOOKFILE;

int bookin()
{
  static int buf5 = 1;
  static int nrle;
  int score;

  if (buf5 == 1) {
    if (nrle) {
      buf5 = 242;
      nrle--;
    } else if ((buf5 = getc(BOOKFILE)) > 242) {
      nrle = buf5 - 242;
      buf5 = 242;
    }
    buf5 += 243;
  }
  score = 3 + 2 * (buf5 % 3);
  buf5 /= 3;
  return score;
}

#define ME	1
#define OPP	2
#define BOOKME	bookx
#define BOOKOPP booko
#define ABME	abx
#define ABOPP	abo
#define THRME	xthrcnt
#define THROPP	othrcnt
#define IWIN	WIN2
#define ILOSE	WIN1
#define MAKEMOV	makemovx
#define BACKMOV	backmovx
#define WORST	8
#define KILLER	xkiller
#define WRSTBND	beta
#define BESTBND	alpha
#define IMPRVS	<
#define NOWORSE	<=
#define NOLOSE	DRIN2
#include "best.h"

#define ME	2
#define OPP	1
#define BOOKME	booko
#define BOOKOPP	bookx
#define ABME	abo
#define ABOPP	abx
#define THRME	othrcnt
#define THROPP	xthrcnt
#define IWIN	WIN1
#define ILOSE	WIN2
#define MAKEMOV	makemovo
#define BACKMOV	backmovo
#define WORST	0
#define KILLER	okiller
#define WRSTBND	alpha
#define BESTBND	beta
#define IMPRVS	>
#define NOWORSE	>=
#define NOLOSE	DRIN1
#include "best.h"

void loadbook()
{
  BOOKFILE = fopen("book.8", "r");
  if (BOOKFILE == NULL) {
    printf("File book.8 not found.\n");
    exit(0);
  }
  nodes = 0;
  booko();
  if (getc(BOOKFILE) != EOF) {
    printf("File book.8 corrupt.\n");
    exit(0);
  }
  fclose(BOOKFILE);
}

int best()
{
  int i,(*myab)();
  uint8 *mythrcnt;
  int me,x,work,score;
  u_int poscnt;

  nodes = 0;
  if (plycnt & 1) {
    mythrcnt = xthrcnt;
    myab = abx;
    me = 1;
  } else {
    mythrcnt = othrcnt;
    myab = abo;
    me = 2;
  }
  for (i = 0; i = r[i]; ) {
    if (mythrcnt[height[i]] || colthr[columns[i]] == me)
      return (plycnt&1) ? WIN2 : WIN1;
  }
  if (x = transpose(columns)) {
    score = SCORE(x);
    if (EXACT(score))
      return score;
  }
  for (i=0; i<64; i++)
     okiller[i] = xkiller[i] = 0x80;
  emptyht();
  score = myab(alp[window], bet[window]);
  poscnt = posed;
  for (work=1; poscnt>>=1; work++) ;	/* work = log of #positions stored */
  return work << 3 | score;		/* normally bestofall */
}
