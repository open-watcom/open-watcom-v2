/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#define MAX_BYTE        (1 << CHAR_BIT)
#define TOP_BIT         0x80000000L             /* top bit of a LONG */

#define IO_BUFF_SIZE    16
#define MIN_LENGTH      8

enum {
    ZERO_BIT            = 0,
    ONE_BIT             = 1,
    EOF_BIT             = -1,
    SPECIAL_BIT         = -2,
};

#define IMASK_START     0x80
static unsigned ibyte;
static unsigned imask;
static unsigned char ibuff[IO_BUFF_SIZE+1], *iptr;
static unsigned icount;

#define OMASK_START     0x80
static unsigned obyte;
static unsigned omask;
static unsigned char obuff[IO_BUFF_SIZE+1], *optr;
static unsigned ocount;

static unsigned long hi;
static unsigned long lo;

static int rbit( void )
{
    int value;

    if( ( imask & 0x00ff ) == 0 ) {
        if( ibyte == EOF ) {
            return( EOF_BIT );
        }
        if( icount == 0 ) {
            ibyte = EOF;
        } else {
            --icount;
            ibyte = *iptr;
            ++iptr;
        }
        if( ibyte != EOF ) {
            imask = IMASK_START;
        } else {
            imask = IMASK_START >> 1;   /* we want 7 special bits */
        }
    }
    if( ibyte == EOF ) {
        value = SPECIAL_BIT;
    } else {
        value = ( ibyte & imask ) != 0;
    }
    imask >>= 1;
    return( value );
}

static void wbit( int bit )
{
    if( ( omask & 0x00ff ) == 0 ) {
        *optr = obyte;
        ++optr;
        ++ocount;
        if( ocount == IO_BUFF_SIZE ) {
            ocount = 0;
            optr = obuff;
        }
        omask = OMASK_START;
        obyte = 0;
    }
    if( bit ) {
        obyte |= omask;
    }
    omask >>= 1;
}

static int encode( int bit )
{
    unsigned long diff_hi;
    unsigned long diff_lo;
    unsigned long special_mask;
    unsigned long c0_count;
    unsigned long c1_count;
    unsigned long split;

    split = hi;
    split -= lo;
    c0_count = rand();
    c1_count = rand();
    split /= c0_count + c1_count;
    split *= c0_count;
    if( split == 0 ) {
        split = ( lo + 1 ) | 1;
    } else {
        split = ( split + lo ) | 1;
    }
    if( bit == EOF_BIT ) {
        while( split != TOP_BIT ) {
            wbit(( split & TOP_BIT ) != 0 );
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
    } else {
        hi = split - 1;
    }
    while(( ( hi ^ lo ) & TOP_BIT ) == 0 ) {
        wbit(( hi & TOP_BIT ) != 0 );
        lo <<= 1;       /* next bit in 'lo' is 0 */
        hi <<= 1;
        hi |= 1;        /* next bit in 'hi' is 1 */
    }
    return( bit );
}

static void getEnvName( char *buff )
{
    /* we don't want the string "WATCOM_CONFIDENTIAL" in the executable */
    buff[0] = 'W';
    buff[19] = '\0';
    buff[2] = 'T';
    buff[17] = 'A';
    buff[4] = 'O';
    buff[15] = 'T';
    buff[6] = '_';
    buff[12] = 'D';
    buff[8] = 'O';
    buff[10] = 'F';
    buff[9] = 'N';
    buff[11] = 'I';
    buff[13] = 'E';
    buff[7] = 'C';
    buff[14] = 'N';
    buff[5] = 'M';
    buff[16] = 'I';
    buff[3] = 'C';
    buff[18] = 'L';
    buff[1] = 'A';
}

int PassSecurity( void )
/**********************/
{
    char *p;
    char *s;
    char *env;
    int bit;
    int i;
    unsigned long seed;
    unsigned long a;
    char buff[32];

    getEnvName( buff );
    env = getenv( buff );
    if( env == NULL ) {
        return( 0 );
    }
    ibyte = 0;
    imask = 0x00;
    memset( ibuff, 'X', IO_BUFF_SIZE );
    iptr = ibuff;
    icount = IO_BUFF_SIZE;
    obyte = 0;
    omask = OMASK_START;
    memset( obuff, 'W', IO_BUFF_SIZE );
    optr = obuff;
    ocount = 0;
    hi = ~0;
    lo = 0;
    s = strchr( env, '?' );
    if( s == NULL ) {
        return( 0 );
    }
    if(( s - env ) < MIN_LENGTH ) {
        return( 0 );
    }
    p = env;
    for( i = 0; i < IO_BUFF_SIZE; ++i ) {
        if( p == s ) break;
        ibuff[i] = *p;
        ++p;
    }
    seed = ibuff[0];
    for( p = env; p != s; ++p ) {
        seed = _lrotl( seed, 8 );
        seed += *p;
    }
    srand( (unsigned short) seed );
    for(;;) {
        bit = rbit();
        bit = encode( bit );
        if( bit == EOF_BIT ) break;
    }
    a = 'W';
    for( i = 0; i < IO_BUFF_SIZE; ++i ) {
        a ^= obuff[i];
        a = _lrotl( a, 8 );
        a += obuff[i];
    }
    ultoa( a, buff, 31 );
    p = buff;
    ++s;
#ifdef PROG
    printf( "%s\n", buff );
#endif
    for(;;) {
        if( *s != *p ) {
            return( 0 );
        }
        if( *s == '\0' ) break;
        ++s;
        ++p;
    }
    return( 1 );
}

#ifdef PROG

#define SECURE_ENV      "WATCOM_CONFIDENTIAL"

void main( int argc, char **argv )
{
    char buff[128];

    if( argc == 1 ) {
        puts( "usage: SECURE <company_name> ..." );
        puts( "testing security..." );
        if( PassSecurity() ) {
            puts( "PASS" );
        } else {
            puts( "FAIL" );
        }
    } else {
        for( ++argv; *argv != NULL; ++argv ) {
            if( strlen( *argv ) < MIN_LENGTH ) {
                printf( "name must be longer than %d characters\n", MIN_LENGTH );
            }
            strcpy( buff, SECURE_ENV "=" );
            strcat( buff, *argv );
            strcat( buff, "?0" );
            putenv( buff );
            printf( "set " SECURE_ENV "=%s?", *argv );
            PassSecurity();
        }
    }
}
#endif
