#include <stdio.h>
#include <stdlib.h>

#include "wattcp.h"
#include "misc.h"
#include "pcdbug.h"
#include "profile.h"

#if defined(__DJGPP__) || defined(__WATCOM386__)

static FILE *fout;
static char  fname[] = "wattcp.pro";

static uint64 cpu_speed;   /* clocks per second */
static uint64 overhead;
static uint64 start_time;

/*
 * Return estimated CPU speed in MHz
 */
static uint64 get_cpu_speed (void)
{
  #define NUM_MSEC     100  /* 100msec should be enough */
  #define NUM_SAMPLES  3

  uint64 speed;
  uint64 sample [NUM_SAMPLES];
  int    i;

  for (i = 0; i < NUM_SAMPLES; i++)
  {
    DWORD  t = set_timeout (NUM_MSEC);
    uint64 start, stop;

    start = get_rdtsc();
    while (!chk_timeout(t))
          ;
    stop = get_rdtsc();
    sample[i] = 1000 * (stop - start) / NUM_MSEC;
  }

  speed = 0;
  for (i = 0; i < NUM_SAMPLES; i++)
      speed += sample[i];
  return (speed / NUM_SAMPLES);
}


void profile_start (const char *str)
{
  if (fout)
  {
    fprintf (fout, "profiling %s: ", str);
    start_time = get_rdtsc();
  }
}

void profile_stop (void)
{
  if (fout)
  {
    uint64 elapsed, now = get_rdtsc();
    double delta_ms;

    if (now > start_time)
         elapsed = now - start_time;
    else elapsed = start_time - now;
    elapsed -= overhead;
    delta_ms = 1000.0 * (double)elapsed / (double)cpu_speed;
    fprintf (fout, "%12.6f msec (%Lu clocks)\n", delta_ms, elapsed);
  }
}

static void profile_exit (void)
{
  if (fout)
  {
    fclose (fout);
    fout = NULL;
    fprintf (stderr, "profiling info written to %s\n", fname);
  }
}

/*
 * must be called after init_timers()
 */
void profile_init (void)
{
  CheckCpuType();
  if (x86_capability & 0x10) /* RDTSC supported */
  {
    cpu_speed = get_cpu_speed();
    has_rdtsc = 1;
    fout = fopen (fname, "wt");
    if (fout)
    {
      uint64 start = get_rdtsc();

      profile_start ("dummy");
      profile_stop();
      overhead = get_rdtsc() - start;
  //  rewind (fout);
      fprintf (fout, "CPU-speed %Lu, overhead %Lu clocks\n",
               cpu_speed, overhead);
      atexit (profile_exit);
    }
  }
}

#endif  /* __DJGPP__ || __WATCOM386__ */

