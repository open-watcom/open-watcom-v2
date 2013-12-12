.func _bios_timeofday for NEC PC98
.synop begin
#include <bios.h>
unsigned _bios_timeofday( unsigned service,
                          char *timeval );
.ixfunc2 '&BiosFunc' &func
.synop end
.desc begin
The
.id &func.
function gets or sets the current system clock value.
The values for service are:
.begterm 12
.termhd1 Value
.termhd2 Meaning
.term _TIME_GETCLOCK
Places the current system clock value in the location pointed to by
.arg timeval
.ct .li .
.term _TIME_SETCLOCK
Sets the system clock to the value in the location pointed to by
.arg timeval
.ct .li .
.endterm
.np
The area pointed to by the argument
.arg timeval
has the following structure.
.begnote
.note timeval+0
Year (BCD 00-99)
.note timeval+1
The upper 4 bits is the month (in the range 1-12).
The lower 4 bits is the day (in the range 0-6).
.note timeval+2
date (BCD 1-31)
.note timeval+3
hours (BCD 0-23)
.note timeval+4
minutes (BCD 0-59)
.note timeval+5
seconds (BCD 0-59)
.endnote
.desc end
.return begin
It returns zero if no errors, non-zero if errors.
.return end
.exmp begin
#include <time.h>
#include <stdio.h>
#include <bios.h>
#define TICKPERSEC      18.2

int     mark[ 10000 ];

void main()
  {
    time_t   tstart, tend;    /* For difftime         */
    clock_t  cstart, cend;    /* For clock            */
    long     bstart, bend;    /* For _bios_timeofday  */
    register int i, loop;
    int      n, num, step;

    /* Start timing. */
    printf( "Working...\n" );
    time( &tstart );  /* Use time and difftime for timing to seconds   */
    cstart = clock(); /* Use clock for timing to hundredths of seconds */
#if defined( DOS )    /* Define DOS to use _bios_timeofday             */
    _bios_timeofday( _TIME_GETCLOCK, &bstart );
#endif
    /* Do timed Sieve of Erotosthenes. */
    for ( loop = 0; loop < 250; ++loop )
        for ( num = 0, n = 3; n < 10000; n += 2 )
            if ( !mark[ n ] ) {
                step = 2 * n;
                for ( i = 3 * n; i < 10000; i += step )
                    mark[ i ] = -1;
                ++num;
            }
    /*
     * End timing and print results. Note that _bios_timeofday doesn't
     * handle midnight rollover.
     */
    time( &tend );
    printf( "\ndifftime:\t\t%4.2f seconds to find %d primes 50 times\n",
             difftime( tend, tstart ), num );
    cend = clock();
    printf( "\nclock:\t\t\t%4.2f seconds to find %d primes 50 times\n",
             ( ( float )cend - cstart ) / CLK_TCK, num );

#if defined( DOS )
    _bios_timeofday( _TIME_GETCLOCK, &bend );
    printf( "\n_bios_timeofday:\t%4.2f seconds",
             ( ( float )bend - bstart ) / TICKPERSEC );
    printf( " to find %d primes 50 times\n", num );

#endif
  }
.exmp end
.class BIOS
.system
