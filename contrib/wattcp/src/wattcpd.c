/*
 *  Waterloo "background" daemon processes.
 *  Called from tcp retransmitter at each timer tick.
 *
 *  E. Engelke
 */

#include <stdio.h>
#include <stdlib.h>

#include "wattcp.h"
#include "pctcp.h"

#define MAX_DAEMONS  50

void (*wattcpd) (void) = NULL;

static void (**backd)(void) = NULL;
static int   lastd;

static void dowattcpd (void)
{
  void (**p)(void) = backd;
  int   count = lastd;
  do
  {
    if (*p)
      (**p)();
  }
  while (count--);
}

int addwattcpd (void (*p)())
{
  int i;

  if (!wattcpd)
  {
    backd = (void(**)()) calloc (MAX_DAEMONS, sizeof(void(*)()));
    if (backd)
       wattcpd = dowattcpd;
  }
  for (i = 0; i < MAX_DAEMONS; i++)
      if (!backd[i])
      {
        backd[i] = p;
        break;
      }

  if (i < MAX_DAEMONS)
  {
    if (lastd <= i)
        lastd = ++i;
    return (0);
  }
  return (-1);
}

int delwattcpd (void (*p)())
{
  int i, j;

  for (i = 0; i < MAX_DAEMONS; i++)
      if (backd[i] == p)
      {
        backd[i] = NULL;
        break;
      }

  for (j = i + 1; j < lastd; j++)
      if (backd[j])
         i = j;
  lastd = i + 1;
  return (0);
}

