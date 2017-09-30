/*
  Dynamic Markov Chain compression program

  written by:   Anthony Scian
                WATCOM Systems Inc.

  Reference:    G.V.Cormack and R.N.S.Horspool
                The Computer Journal
                Vol. 30, No. 6, 1987
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>

#define MAX_BYTE        (1 << CHAR_BIT)
#define TOP_BIT         (UBIG(1) << (sizeof(UBIG)*CHAR_BIT-1))

#define TRUST_LEVEL     1                       /* DMC parameters */
#define THRESHOLD       10
#define MAX_STATES    32750

#if ( CHAR_BIT * MAX_BYTE - 1 ) > MAX_STATES
#error MAX_STATES is too small
#endif

#define IO_BUFF_SIZE    16384

enum {
    ZERO_BIT            = 0,
    ONE_BIT             = 1,
    EOF_BIT             = -1,
    SPECIAL_BIT         = -2,
};

typedef UBIG bigcount_t;
typedef UBIG guazzo_t;
typedef UBIG state_t;
typedef UBIG count_t;

unsigned char compress;

state_t state;
state_t last;
state_t *t0;
state_t *t1;
count_t *c0;
count_t *c1;

long success = 0;

#define IMASK_START     0x80
int ifd;
UBIG ibytes;
unsigned ibyte;
unsigned imask = 0x00;
unsigned char *ibuff, *iptr;
unsigned icount;

#define OMASK_START     0x80
int ofd;
UBIG obytes;
unsigned obyte;
unsigned omask = OMASK_START;
unsigned char *obuff, *optr;
unsigned ocount;

guazzo_t hi = ~(guazzo_t)0;
guazzo_t lo = 0;

#define NL      "\n"

void outs( char *p )
{
    fputs( p, stdout );
}

void outl( unsigned long val )
{
    char buff[16];

    ultoa( val, buff, 10 );
    fputs( buff, stdout );
}

void fatal( char *msg )
{
    puts( msg );
    exit( EXIT_FAILURE );
}

void printStats( void )
{
#if 0   // seems to differ slightly if using __int64
    outs( "input size=" );
    outl( ibytes );
    outs( " output size=" );
    outl( obytes );
    outs( NL );
#endif
    if( compress ) {
        if( obytes < ibytes ) {
            outs( "compression ratio: " );
            outl( ( obytes * 100 ) / ibytes );
            outs( "% (PKZIP ratio=" );
            outl( (( ibytes - obytes ) * 100 ) / ibytes );
            outs( "%)" NL );
        } else {
            outs( "no compression!" NL );
        }
    } else {
        if( obytes > ibytes ) {
            outs( "decompression ratio: " );
            outl( ( ibytes * 100 ) / obytes );
            outs( "% (PKZIP ratio=" );
            outl( (( obytes - ibytes ) * 100 ) / obytes );
            outs( "%)" NL );
        } else {
            outs( "no decompression!" NL );
        }
    }
}

int ReadBit( void )
{
    int value;
    size_t amt_read;

    if( ( imask & 0x00ff ) == 0 ) {
        if( ibyte == EOF ) {
            return( EOF_BIT );
        }
        if( icount == 0 ) {
            amt_read = read( ifd, ibuff, IO_BUFF_SIZE );
            if( amt_read == 0 || amt_read == -1 ) {
                ibyte = EOF;
            } else {
                icount = amt_read - 1;
                ibyte = ibuff[0];
                iptr = &ibuff[1];
            }
        } else {
            --icount;
            ibyte = *iptr;
            ++iptr;
        }
        if( ibyte != EOF ) {
            ++ibytes;
            imask = IMASK_START;
        } else {
            imask = IMASK_START >> 1;   /* we want 7 special bits */
        }
    }
    if( ibyte == EOF ) {
        if( compress ) {
            value = SPECIAL_BIT;
        } else {
            value = EOF_BIT;
        }
    } else {
        value = ( ibyte & imask ) != 0;
    }
    imask >>= 1;
    return( value );
}

void FlushOutput( void )
{
    if( ocount ) {
        write( ofd, obuff, ocount );
        ocount = 0;
        optr = obuff;
    }
}

void WriteBit( int bit )
{
    if( ( omask & 0x00ff ) == 0 ) {
        *optr = obyte;
        ++optr;
        ++ocount;
        if( ocount == IO_BUFF_SIZE ) {
            FlushOutput();
        }
        ++obytes;
        omask = OMASK_START;
        obyte = 0;
    }
    if( bit ) {
        obyte |= omask;
    }
    omask >>= 1;
}

void AllocMem( void )
{
    t0 = (state_t *) malloc( sizeof( state_t ) * MAX_STATES );
    t1 = (state_t *) malloc( sizeof( state_t ) * MAX_STATES );
    c0 = (count_t *) malloc( sizeof( count_t ) * MAX_STATES );
    c1 = (count_t *) malloc( sizeof( count_t ) * MAX_STATES );
    if( t0 == NULL || t1 == NULL || c0 == NULL || c1 == NULL ) {
        fatal( "cannot initialize" );
    }
    ibuff = (unsigned char *) malloc( IO_BUFF_SIZE );
    obuff = (unsigned char *) malloc( IO_BUFF_SIZE );
    if( ibuff == NULL || obuff == NULL ) {
        fatal( "cannot initialize" );
    }
    icount = 0;
    iptr = ibuff;
    ocount = 0;
    optr = obuff;
}

void MakeBraid( void )
{
    unsigned i;
    unsigned j;
    unsigned k;
    state_t state;

    for( i = 0; i < CHAR_BIT; ++i ) {
        for( j = 0; j < MAX_BYTE; ++j ) {
            state = i + CHAR_BIT * j;
            k = ( i + 1 ) % CHAR_BIT;
            t0[state] = k + (( 2 * j     ) % MAX_BYTE ) * CHAR_BIT;
            t1[state] = k + (( 2 * j + 1 ) % MAX_BYTE ) * CHAR_BIT;
        }
    }
    last = CHAR_BIT * MAX_BYTE - 1;
}

void InitCounts( void )
{
    state_t i;

    for( i = 0; i <= last; ++i ) {
        c0[i] = 2;
        c1[i] = 1;
    }
}

state_t CloneState( state_t curr, int bit )
{
    bigcount_t next_count;
    state_t next;
    state_t new_state;
    state_t *tb;
    count_t *cb;

    if( bit ) {
        tb = t1;
        cb = c1;
    } else {
        tb = t0;
        cb = c0;
    }
    ++cb[curr];
    next = tb[curr];
    next_count = (bigcount_t)c0[next] + c1[next];
    if(( next_count + 1 ) < cb[curr] ) {
        return( next );
    }
    if( cb[curr] > THRESHOLD && (( next_count + 1 ) - cb[curr] ) > THRESHOLD ) {
        if( ++last >= MAX_STATES ) {
            printStats();
            MakeBraid();
            InitCounts();
            return( 0 );
        }
        new_state = last;
        /* build new state */
        tb[curr] = new_state;
        /* 0 bit */
        t0[new_state] = t0[next];
        c0[new_state] = ( (bigcount_t)cb[curr] * c0[next] ) / next_count;
        c0[next] -= c0[new_state];
        /* 1 bit */
        t1[new_state] = t1[next];
        c1[new_state] = ( (bigcount_t)cb[curr] * c1[next] ) / next_count;
        c1[next] -= c1[new_state];
        return( new_state );
    }
    return( next );
}

int GuazzoEncode( state_t curr, int bit )
{
    UBIG diff_hi;
    UBIG diff_lo;
    UBIG special_mask;
    guazzo_t split;

    split = hi;
    split -= lo;
    if( success > 0 ) {
        bigcount_t c0_count;

        c0_count = c0[curr] + TRUST_LEVEL;
        split /= c0_count + ( (bigcount_t)c1[curr] + TRUST_LEVEL );
        split *= c0_count;
    } else {
        /* bad predictions so far, don't use them */
        split /= 2;
    }
    if( split == 0 ) {
        split = ( lo + 1 ) | 1;
    } else {
        split = ( split + lo ) | 1;
    }

    if( bit == EOF_BIT ) {
        while( split != TOP_BIT ) {
            WriteBit(( split & TOP_BIT ) != 0 );
            split <<= 1;
        }
        return( bit );
    }
    if( bit == SPECIAL_BIT ) {
        special_mask = TOP_BIT;
        diff_hi = hi ^ special_mask;
        diff_lo = lo ^ special_mask;
        do {
            if( diff_hi & special_mask ) {
                bit = 1;
                break;
            }
            if( diff_lo & special_mask ) {
                bit = 0;
                break;
            }
            special_mask >>= 1;
        } while( special_mask != 0 );
    }
    if( bit ) {
        lo = split;
        if( c1[curr] > c0[curr] ) {
            ++success;
        } else {
            --success;
        }
    } else {
        hi = split - 1;
        if( c0[curr] > c1[curr] ) {
            ++success;
        } else {
            --success;
        }
    }
    while(( ( hi ^ lo ) & TOP_BIT ) == 0 ) {
        WriteBit(( hi & TOP_BIT ) != 0 );
        lo <<= 1;       /* next bit in 'lo' is 0 */
        hi <<= 1;
        hi |= 1;        /* next bit in 'hi' is 1 */
    }
    return( bit );
}

void GuazzoDecode( void )
{
    UBIG in_msg;
    UBIG last_bit;
    int bit;
    int out_bit;
    guazzo_t split;

    in_msg = 0;
    last_bit = 0;
    for(;;) {
        split = hi;
        split -= lo;
        if( success > 0 ) {
            split /= (bigcount_t)c0[state] + (bigcount_t)c1[state] +
                     2 * TRUST_LEVEL;
            split *= ( (bigcount_t)c0[state] + TRUST_LEVEL );
        } else {
            /* bad predictions so far, don't use them */
            split /= 2;
        }
        split += lo;
        if( split == lo ) {
            split += 1;
        }
        split |= 1;

        for(;;) {
            if( ( in_msg | ( last_bit - 1 ) ) < split ) {
                hi = split - 1;
                if( c0[state] > c1[state] ) {
                    ++success;
                } else {
                    --success;
                }
                out_bit = 0;
                break;
            }
            if( in_msg >= split ) {
                lo = split;
                if( c1[state] > c0[state] ) {
                    ++success;
                } else {
                    --success;
                }
                out_bit = 1;
                break;
            }
            bit = ReadBit();
            if( bit == EOF_BIT ) {
                return;
            }
            if( last_bit == 0 ) {
                last_bit = TOP_BIT;
            } else {
                last_bit >>= 1;
            }
            if( last_bit == 0 ) {
                fatal( "guazzo encoding overflow" );
            }
            if( bit ) {
                in_msg |= last_bit;
            }
        }
        WriteBit( out_bit );
        state = CloneState( state, out_bit );
        while((( hi ^ lo ) & TOP_BIT ) == 0 ) {
            lo <<= 1;
            hi <<= 1;
            hi |= 1;
            in_msg <<= 1;
            last_bit <<= 1;
        }
    }
}

int main( int argc, char **argv )
{
    int bit;
    clock_t start;
    clock_t stop;

    if( argc != 4 ) {
        fatal( "usage: DMC {c|d} <input_file> <output_file>" );
    }
    switch( tolower( argv[1][0] ) ) {
    case 'c':
        compress = 1;
        break;
    case 'd':
        compress = 0;
        break;
    default:
        fatal( "must specify 'c' or 'd' as first command line argument" );
    }
    ifd = open( argv[2], O_RDONLY|O_BINARY );
    if( ifd == -1 ) {
        fatal( "unable to open input file" );
    }
    ofd = open( argv[3], O_WRONLY|O_TRUNC|O_CREAT|O_BINARY, S_IWRITE|S_IREAD );
    if( ofd == -1 ) {
        fatal( "unable to open output file" );
    }

    AllocMem();
    MakeBraid();
    InitCounts();
    state = 0;
    if( compress ) {
        for(;;) {
            bit = ReadBit();
            bit = GuazzoEncode( state, bit );   /* calls WriteBit() */
            if( bit == EOF_BIT ) break;
            state = CloneState( state, bit );
        }
    } else {
        GuazzoDecode();
    }
    FlushOutput();
    printStats();
    return( EXIT_SUCCESS );
}
