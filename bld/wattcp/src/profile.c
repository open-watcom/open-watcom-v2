#include <stdio.h>
#include <stdlib.h>

#include "wattcp.h"
#include "misc.h"
#include "pcdbug.h"
#include "profile.h"

#if defined(__386__)

static FILE *fout;
static char  fname[] = "wattcp.pro";

static uint64 cpu_speed;   /* clocks per second */
static uint64 overhead;
static uint64 start_time;

/*
 * Return estimated CPU speed in MHz
 */
static uint64 get_cpu_speed( void )
{
    #define NUM_MSEC     100  /* 100msec should be enough */
    #define NUM_SAMPLES  3

    uint64 speed;
    uint64 sample [NUM_SAMPLES];
    int    i;

    for( i = 0; i < NUM_SAMPLES; i++ ) {
        DWORD  t = set_timeout( NUM_MSEC );
        uint64 start, stop;

        start = get_rdtsc();
        while( !chk_timeout( t ) )
            ;
        stop = get_rdtsc();
        sample[i] = 1000 * ( stop - start ) / NUM_MSEC;
    }

    speed = 0;
    for( i = 0; i < NUM_SAMPLES; i++ )
        speed += sample[i];
    return( speed / NUM_SAMPLES );
}


void profile_start( const char *str )
{
    if( fout ) {
        fprintf( fout, "profiling %s: ", str );
        start_time = get_rdtsc();
    }
}

void profile_stop (void)
{
    if( fout ) {
        uint64 elapsed, now = get_rdtsc();
        uint64 delta_ms;

        if( now > start_time ) {
            elapsed = now - start_time;
        } else {
            elapsed = start_time - now;
        }
        elapsed -= overhead;
        delta_ms = ( 10000 * elapsed ) / cpu_speed;
        fprintf( fout, "%llu.%u msec (%Lu clocks)\n", delta_ms / 10, delta_ms % 10, elapsed );
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
    unsigned    reg_edx;

    if( has_cpuid() ) {   /* is CPUID supported */
        get_cpuid( 1, &reg_edx, &reg_edx, &reg_edx, &reg_edx );
        if( reg_edx & 0x10 ) {
            has_rdtsc = 1;
            cpu_speed = get_cpu_speed();
            fout = fopen( fname, "wt" );
            if( fout != NULL ) {
                uint64 start = get_rdtsc();

                profile_start( "dummy" );
                profile_stop();
                overhead = get_rdtsc() - start;
//                rewind( fout );
                fprintf( fout, "CPU-speed %Lu, overhead %Lu clocks\n",
                        cpu_speed, overhead );
                atexit( profile_exit );
            }
        }
    }
}

#endif  /* __386__ */
