/* transposition table routines 
   (c) 1992 John Tromp
*/

#include "c4.h"

#define HTSIZE  1050011		/* not to be messed with:-) */
#define HTD	(HTSIZE & 0xfffff)
#define STRNG	(HTD/8)
#define STD	(0x100-STRNG)
#ifndef	PROBES			/* must be in range 1--8 */
#define PROBES	8		/* suggested by Robert Hyatt */
#endif

static u_int *ht;			/* hash locks */
static hashentry *he;			/* hash entries */
static u_int stride;
static u_int htindex, lock, hits;
static u_int works[32];
static u_int typecnt[8];                /* bound type stats */

#ifndef MOD64
typedef struct {
  unsigned htmod:24;
  unsigned stmod: 8;
} mods;

static mods mulmod[0x402];
#endif

u_int posed;				/* counts transtore calls */

void inittrans()
{
  int i, htm, stm;

  ht = allocate(HTSIZE,u_int);
  he = allocate(HTSIZE,hashentry);
  if (ht == NULL || he == NULL) {
    printf("Out of memory; failed to allocate %d bytes.\n",
	   HTSIZE * (sizeof(u_int) + sizeof(hashentry)));
    exit(0);
  }

#ifndef MOD64
  for (i = htm = stm = 0; i < 0x402; i++) {
    mulmod[i].htmod = htm;
    mulmod[i].stmod = stm;
    if ((htm -= HTD) < 0)
      htm += HTSIZE;
    if ((stm += STD) >= STRNG)
      stm -= STRNG;
  }
#endif
}

void emptyht()
{
  int i;

  for (i=0; i<HTSIZE; i++)
    if (he[i] < 0xf8)           /* leave book-entries intact */
      he[i] &= 0x07;
  posed = hits = 0;
}

double hitrate()
{
  return posed ? (double)hits/(double)posed : 0.0;
}

void hash(pos)
B8 pos;
{
  register u_int t1,t2;

#ifdef MOD64
  register uint64 htemp;
#else
  register u_int htemp,stemp,t;
#endif

  t1 = (pos[1] << 7 | pos[2]) << 7 | pos[3];
  t2 = (pos[7] << 7 | pos[6]) << 7 | pos[5];
  
#ifdef MOD64
  htemp = t1 > t2 ? (uint64)(t1 << 7 | pos[4]) << 21 | t2:
                    (uint64)(t2 << 7 | pos[4]) << 21 | t1;
  lock = htemp >> 17;
  htindex = htemp % HTSIZE;
  stride = 0x20000 + lock % STRNG;
#else
  if (t1 > t2) {
    lock = (t1 << 7 | pos[4]) << 4 | t2 >> 17;
    t = t2 & 0x7ffff;
  } else {
    lock = (t2 << 7 | pos[4]) << 4 | t1 >> 17;
    t = t1 & 0x7ffff;
  }
  htemp = (lock >> 2 & 0xfffff) + mulmod[lock >> 22].htmod;
  htemp = ((htemp & 0x7ff) << 9 | (t >> 10)) + mulmod[htemp >> 11].htmod;
  if (htemp >= HTSIZE)
    htemp -= HTSIZE;
  htemp = ((htemp & 0x3ff) << 10 | (t & 0x3ff)) + mulmod[htemp >> 10].htmod;
  htindex = htemp >= HTSIZE ? htemp - HTSIZE : htemp;
  
  stemp = (lock >> 16 & 0xff) + mulmod[lock >> 24].stmod;
  stemp = (lock >>  8 & 0xff) + mulmod[stemp].stmod;
  stemp = (lock       & 0xff) + mulmod[stemp].stmod;
  if (stemp >= STRNG && (stemp -= STRNG) >= STRNG)
    stemp -= STRNG;
  stride = stemp + 0x20000;
#endif
}

int transpose(pos)
B8 pos;
{
  int i,x;

  hash(pos);
  for (x=htindex, i=0; i < PROBES; i++) {
    if (ht[x] == lock)                /* CACHE MISS! */
      return he[x];
    if ((x += stride) >= HTSIZE)
      x -= HTSIZE;
  }
  return 0;
}

void transrestore(pos,score,work)
B8 pos;
int score,work;
{
  register int i,x;

  posed++;
  hash(pos);
  for (x=htindex, i=0; i < PROBES; i++) {
    if (ht[x] == lock) {               /* CACHE MISS! */
      hits++;
      he[x] = work << 3 | score;
      return;
    }
    if ((x += stride) >= HTSIZE)
      x -= HTSIZE;
  }
  for (x=htindex, i=0; i < PROBES; i++) {
    if (work > WORK(he[x])) {
      hits++;
      ht[x] = lock;
      he[x] = work << 3 | score;
      return;
    }
    if ((x += stride) >= HTSIZE)
      x -= HTSIZE;
  }
}

void transtore(pos,score,work)
B8 pos;
int score,work;
{
  register int i,x;

  posed++;
  hash(pos);
  for (x=htindex, i=0; i < PROBES; i++) {
    if (work > WORK(he[x])) {
      hits++;
      ht[x] = lock;
      he[x] = work << 3 | score;
      return;
    }
    if ((x += stride) >= HTSIZE)
      x -= HTSIZE;
  }
}

double rate(S)
int S;
{
  u_int total;
  int i;

  for (total=i=0; i<8; i++)
    total += typecnt[i];
  return total ? (double)typecnt[S]/(double)total : 0.0;
}

void htstat()      /* some statistics on hash table performance */
{
  int i;

  for (i=0; i<32; i++)
    works[i] = 0;
  for (i=0; i<8; i++)
    typecnt[i] = 0;
  for (i=0; i<HTSIZE; i++) {
    works[WORK(he[i])]++;
    if (WORK(he[i]))
      typecnt[SCORE(he[i])]++;
  }
  (void)printf("store rate = %.3f\n",hitrate());
  (void)printf("- %5.3f  < %5.3f  = %5.3f  > %5.3f  + %5.3f\n",
            rate(WIN2),rate(DRIN2),rate(DRAW),rate(DRIN1),rate(WIN1));
  for (i=0; i<32; i++)
    (void)printf("%6u%c",works[i],(i&7)==7?'\n':'\t');
}
