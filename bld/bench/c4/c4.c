/*
 * Fhourstones connect-4 solver
 *
 * implementation of the well-known game
 * played on a vertical board of 7 columns by 6 rows,
 * where 2 players take turns in dropping counters in a column.
 * the first player to get four of his counters
 * in a horizontal, vertical or diagonal row, wins the game.
 * if neither player has won after 42 moves, then the game is drawn.
 *
 * This software is copyright (c) 1992 by
 *      John Tromp
 *      Lindenlaan 33
 *      1701 GT Heerhugowaard
 *      Netherlands
 * E-mail: tromp@cwi.nl
 *
 * This notice must not be removed.
 * This software must not be sold for profit.
 * You may redistribute if your distributees have the
 * same rights and restrictions.
 */


#include "timer.h"
#include "report.h"
#include "c4.h"

static
int history[43],	/* history of current game */
    movenr;		/* number of counters played in current game */

static
int error,		/* invalid user input */
    gameover;		/* game has been drawn or won */
extern int window;	/* search window */

void initall()
{
  extern void initbest(),inittrans(),initplay();

  initbest();
  inittrans();
  initplay();
}

void game()
{
  extern void newgame();

  movenr = gameover = 0;
  newgame();
}

void play(n)
int n;
{
  extern int depth();
  extern void makemovx(),makemovo();
  extern int haswon(void);

   if (error = n < 1 || n > 7 || !depth(n) || gameover)
      return;
   if (history[movenr] != n)
   {  history[movenr] = n;
      history[movenr+1] = 0;     /* end of variation */
   }
   if (++movenr & 1)
     makemovo(n);
   else
     makemovx(n);
   gameover = haswon() || movenr == 42;
}

int main( int argc, char *argv[] )
{
  int c,i,result;
  extern int best();
  extern void loadbook(),htstat();
  extern unsigned nodes;
  double secs;
  extern double dtime();
  FILE	 *fp;
  int	 needs_close = 0;

  fp = stdin;
  if( argc > 1 ) {
    fp = fopen( argv[ 1 ], "rt" );
    if( fp == NULL ) {
      printf( "Error opening input file '%s'\n", argv[ 1 ] );
      fp = stdin;
    } else {
      needs_close = 1;
    }
  }
  initall();
#ifdef BOOK
  loadbook();
#endif
  (void)printf("Fhourstones 1.0\noptions:");
#ifdef SMALL
  (void)printf("    small ints");
#endif
#ifdef MOD64
  (void)printf("    64-bit modulo");
#endif
  (void)printf("\n");
  for (;;) {
    game();
    while ((c = fgetc( fp )) != EOF) {
      if (c >= '1' && c <= '7')
        c -= '0';
      else if (c >= 'A' && c <= 'G')
        c -= ('A' - 1);
      else if (c >= 'a' && c <= 'g')
        c -= ('a' - 1);
      else if (c == '=' || c == '+' || c == '-' ||
               (error = c != ' ' && c != '\t' && c != '\n'))
        break;
      else continue;
      play(c);
      if (error) {
        (void)printf("illegal move.\n");
        break;
      }
    }
    if (c == EOF)
      break;
    if (error) {
      error = 0;
      continue;
    }
    if (gameover) {
      (void)printf("Game is over.\n");
      continue;
    }

    switch (c) {
    case '-':
      window = 2;
      (void)printf("Looking for 2nd player win in");
      break;
    case '=':
      window = 0;
      (void)printf("Solving");
      break;
    case '+':
      window = 1;
      (void)printf("Looking for 1st player win in");
      break;
    }
    (void)printf(" %d-ply position after ", movenr);
    for (i = 0; i < movenr; i++)
      (void)printf("%d", history[i]);
    (void)printf(" . . .\n");

    secs = dtime();
    TimerOn();
    result = best();		/* expect work << 3 | score */
    TimerOff();
    secs = dtime() - secs;
    Report( "C4", TimerElapsed() );
    if (secs < 1.0e-6)		/* prevent division by 0 */
      secs = 1.0e-6;
    (void)printf("score = %d (%c)  work = %d\n", result&7,
           "   -<=>+"[result&7], result>>3);
    (void)printf("%u pos / %.1f sec = %.1f Kpos/sec\n",
             nodes, secs, 0.001*(double)nodes/secs);
    htstat();
    if( needs_close ) {
	fclose( fp );
    }
  }
  return 0;
}
