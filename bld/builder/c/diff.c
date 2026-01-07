/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  diff.c - public domain context diff program
*                   (32/64-bit code only, no 16-bit)
*
****************************************************************************/


#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include "bool.h"
#include "wio.h"
#include "watcom.h"
#include "pathgrp2.h"

#include "clibext.h"


#define EOS             0
#define TEMPFILE        "diff.tmp"

#define MYALLOC(t,s,w)      (t *)myalloc((s) * sizeof( t ), w)
#define MYCOMPACT(t,p,s,w)  (t *)compact((p), (s) * sizeof( t ), w)
#define MYFREE(p)           if(p != NULL) {free(p); p = NULL;}

typedef enum {
    DIFF_NO_DIFFS = 0,
    DIFF_HAVE_DIFFS,
    DIFF_NOT_COMPARED,
    DIFF_NO_MEMORY,
}       return_code;

typedef enum {
    FIL_A = 0,
    FIL_B,
    FIL_SIZE,
}       entry_file;

typedef unsigned long   ULONG;
typedef long            SLONG;
typedef unsigned short  USHORT;
typedef short           SSHORT;

typedef struct candidate {
    SLONG       b;          /* Line in fileB      */
    SLONG       a;          /* Line in fileA      */
    SLONG       link;       /* Previous candidate */
}               CANDIDATE;

typedef struct line {
    USHORT      hash;       /* Hash value etc.    */
    SSHORT      serial;     /* Line number        */
}               LINE;

static LINE         *file[FIL_SIZE];    /* Hash/line for total file  */
static LINE         *sfile[FIL_SIZE];   /* Hash/line after prefix    */
static SLONG        len[FIL_SIZE];      /* Actual lines in each file */
static SLONG        slen[FIL_SIZE];     /* Squished lengths          */

static SLONG        prefix;             /* Identical lines at start  */
static SLONG        suffix;             /* Identical lenes at end    */

/*
 * The following vectors overlay the area defined by fileA
 */

static SSHORT       *class;             /* Unsorted line numbers        */
static SLONG        *klist;             /* Index of element in clist    */
static CANDIDATE    *clist;             /* Storage pool for candidates  */
static SLONG        clength = 0;        /* Number of active candidates  */
#define CSIZE_INC   50                  /* How many to allocate each time we have to */
static SLONG        csize = CSIZE_INC;  /* Current size of storage pool */

static SLONG        *match;             /* Longest subsequence          */

/*
 * The following vectors overlay the area defined by fileB
 */

static SSHORT       *member;            /* Concatenated equiv. classes  */

/*
 * Files IO related variables
 */
static long         *oldseek;           /* Seek position in file A      */
static long         *newseek;           /* Seek position in file B      */

static int          symbol[FIL_SIZE] = { 'A', 'B' };
static FILE         *infd[FIL_SIZE];    /* Input files A and B handles  */
static FILE         *tempfd;            /* Temp file for input redirection handle */

/*
 * Global variables
 */

static char         *Dflag = NULL;      /* output #ifdef code           */
static bool         Hflag = false;      /* half hearted algorithm       */
static bool         nflag = false;      /* Edit script requested        */
static bool         eflag = false;      /* Edit script requested        */
static bool         bflag = false;      /* Blank supress requested      */
static int          cflag = 0;          /* Context printout             */
static bool         iflag = false;      /* Ignore case requested        */
static bool         tflag = false;      /* Test for enough memory flag  */
static int          diff_rc = DIFF_NO_DIFFS;

#define BUFSIZE     1025

static char         text[FIL_SIZE][BUFSIZE]; /* Input line from file */

static const char   *cmdusage =
"usage:\n"
"        diff [options] file1 file2\n"
"\n"
"options:\n"
"        -H           Use half hearted algorithm if memory runs out\n"
"        -n           generate an edit script for RCS\n"
"        -e           generate an edit script\n"
"        -b           ignore blanks\n"
"        -c[n]        print n context line (defaults to 3)\n"
"        -i           ignore case\n"
"        -t           quiet mode. return 3 if not enough memory\n"
;

#ifdef DEVBUILD

static void dump_sfile_data( int which )
/***************************************
 * Dump file sorted data
 */
{
    SLONG   i;

    for( i = 1; i <= slen[which]; i++ ) {
        printf( "sfile%c[%d] = %6d %06o\n", symbol[which], i, sfile[which][i].serial, sfile[which][i].hash );
    }
}

static void dumpklist( SLONG kmax, const char *why )
/***************************************************
 * Dump klist
 */
{
    SLONG    i;
    CANDIDATE *cp;
    SLONG    count;

    printf( "\nklist[0..%d] %s, clength = %d\n", kmax, why, clength );
    for( i = 0; i <= kmax; i++ ) {
        cp = &clist[klist[i]];
        printf( "%2d %2d", i, klist[i] );
        if( cp >= &clist[0]
          && cp < &clist[clength] ) {
            printf( " (%2d %2d -> %2d)\n", cp->a, cp->b, cp->link );
        } else if( klist[i] == -1 ) {
            printf( " End of chain\n" );
        } else {
            printf( " illegal klist element\n" );
        }
    }
    for( i = 0; i <= kmax; i++ ) {
        count = -1;
        for( cp = (CANDIDATE *)&klist[i]; cp > &clist[0];
             cp = (CANDIDATE *)&cp->link ) {
            if( ++count >= 6 ) {
                printf( "\n    " );
                count = 0;
            }
            printf( " (%2d: %2d,%2d -> %d)",
                    (int)( cp - clist ), cp->a, cp->b, cp->link );
        }
        printf( "\n" );
    }
    printf( "*\n" );
}

#if 0
static void rdump( SLONG *pointer, const char *why )
/***************************************************
 * Dump memory block
 */
{
    SLONG       *last;
    SLONG       count;

    last = ( (SLONG **)pointer )[-1];
    fprintf( stderr, "dump %s of %06o -> %06o, %d words",
        why, (unsigned)(pointer_int)pointer, (unsigned)(pointer_int)last, (int)( last - pointer ) );
    last = (SLONG *)( ((pointer_int)last) & ~1 );
    for( count = 0; pointer < last; ++count ) {
        if( (count & 07) == 0 ) {
            fprintf( stderr, "\n%06o", (unsigned)(pointer_int)pointer );
        }
        fprintf( stderr, "\t%06o", *pointer );
        pointer++;
    }
    fprintf( stderr, "\n" );
}

static void dump( int which )
{
    SLONG       i;

    printf( "Dump of file%c, %d elements\n", symbol[which], len[which] );
    printf( "linep @ %06o\n", (unsigned)(pointer_int)file[which] );
    for( i = 0; i <= len[which]; i++ ) {
        printf( "%3d  %6d  %06o\n", i, file[which][i].serial, file[which][i].hash );
    }
}
#endif

#endif

/*
 * Sort hash entries
 */

static void sort( int which )
{
    SLONG       j;
    LINE        *aim;
    LINE        *ai;
    SLONG       mid;
    SLONG       k;
    LINE        work;

    for( j = 1; j <= slen[which]; j *= 2 )
        ;
    mid = ( j - 1 );
    while( ( mid /= 2 ) != 0 ) {
        k = slen[which] - mid;
        for( j = 1; j <= k; j++ ) {
            for( ai = &sfile[which][j]; ai > sfile[which]; ai -= mid ) {
                aim = &ai[mid];
                if( aim < ai )
                    break;      /* ?? Why ??     */
                if( aim->hash > ai->hash
                  || ( aim->hash == ai->hash
                  && aim->serial > ai->serial ) ) {
                    break;
                }
                work.hash = ai->hash;
                ai->hash = aim->hash;
                aim->hash = work.hash;
                work.serial = ai->serial;
                ai->serial = aim->serial;
                aim->serial = work.serial;
            }
        }
    }
}

/*
 * Build equivalence class vector
 */

static void equiv( void )
{
    LINE        *ap;
    union {
        LINE    *bp;
        SSHORT  *mp;
    }           r;
    SLONG       j;
    LINE        *atop;

#ifdef DEVBUILD
    printf( "equiv entry\n" );
    dump_sfile_data( FIL_A );
    dump_sfile_data( FIL_B );
#endif
    j = 1;
    ap = &sfile[FIL_A][1];
    r.bp = &sfile[FIL_B][1];
    atop = &sfile[FIL_A][slen[FIL_A]];
    while( ap <= atop && j <= slen[FIL_B] ) {
        if( ap->hash < r.bp->hash ) {
            ap->hash = 0;
            ap++;
        } else if( ap->hash == r.bp->hash ) {
            ap->hash = j;
            ap++;
        } else {
            r.bp++;
            j++;
        }
    }
    while( ap <= atop ) {
        ap->hash = 0;
        ap++;
    }
    sfile[FIL_B][slen[FIL_B] + 1].hash = 0;
#ifdef DEVBUILD
    printf( "equiv exit\n" );
    dump_sfile_data( FIL_A );
    dump_sfile_data( FIL_B );
#endif
    ap = &sfile[FIL_B][0];
    atop = &sfile[FIL_B][slen[FIL_B]];
    r.mp = &member[0];
    while( ++ap <= atop ) {
        r.mp++;
        *r.mp = -( ap->serial );
        while( ap[1].hash == ap->hash ) {
            ap++;
            r.mp++;
            *r.mp = ap->serial;
        }
    }
    r.mp[1] = -1;
#ifdef DEVBUILD
    for( j = 0; j <= slen[FIL_B]; j++ ) {
        printf( "member[%d] = %d\n", j, member[j] );
    }
#endif
}

/*
 * my_fgets() is like fgets() except that the terminating newline
 * is removed.
 */

static char *my_fgets( char *s, int max_len, FILE *iop )
{
    char    *cs;
    size_t  len1;

    if( fgets( s, max_len, iop ) == NULL )
        return( NULL );
    len1 = strlen( s );
    cs = s + len1 - 1;
    if( *cs == '\n' ) {
        *cs = '\0';
    }
    --cs;
    if( len1 > 1
      && *cs == '\r' ) {
        *cs = '\0';
    }
    return( s );
}

/*
 * Error message before retiring.
 */

static void error( const char *format, ... )
{
    va_list args;

    va_start( args, format );
    vfprintf( stderr, format, args );
    va_end( args );
    putc( '\n', stderr );
    fflush( stderr );
}

static void noroom( const char *why )
{
    if( tflag ) {
        exit( DIFF_NO_MEMORY );
    } else if( Hflag ) {
        MYFREE( klist );
        MYFREE( clist );
        MYFREE( match );
        MYFREE( oldseek );
        MYFREE( newseek );
        MYFREE( file[FIL_A] );
        MYFREE( file[FIL_B] );
        printf( "d1 %ld\n", len[FIL_A] );
        printf( "a1 %ld\n", len[FIL_B] );
        fseek( infd[FIL_B], 0, 0 );
        while( my_fgets( text[FIL_B], sizeof( text[FIL_B] ), infd[FIL_B] ) != NULL )
            printf( "%s\n", text[FIL_B] );
        exit( DIFF_HAVE_DIFFS );
    } else {
        error( "Out of memory when %s\n", why );
        exit( DIFF_NOT_COMPARED );
    }
}

/*
 * Allocate or crash.
 */

static void *myalloc( size_t size, const char *why )
{
    void    *ptr;

    ptr = malloc( size );
    if( ptr == NULL )
        noroom( why );
    return( ptr );
}

/*
 * Build class vector
 */

static void unsort( void )
{
    SLONG       *temp;
    SLONG       *tp;
    union {
        LINE    *ap;
        SSHORT  *cp;
    }           u;
    LINE        *evec;
    SSHORT      *eclass;
#ifdef DEVBUILD
    SLONG       i;
#endif

    temp = MYALLOC( SLONG, slen[FIL_A] + 1, "unsort scratch" );
    u.ap = &sfile[FIL_A][1];
    evec = &sfile[FIL_A][slen[FIL_A]];
    while( u.ap <= evec ) {
#ifdef DEVBUILD
        printf( "temp[%2d] := %06o\n", u.ap->serial, u.ap->hash );
#endif
        temp[u.ap->serial] = u.ap->hash;
        u.ap++;
    }
    /*
     * Copy into class vector and free work space
     */
    u.cp = &class[1];
    eclass = &class[slen[FIL_A]];
    tp = &temp[1];
    while( u.cp <= eclass ) {
        *u.cp++ = *tp++;
    }
    MYFREE( temp );
#ifdef DEVBUILD
    printf( "unsort exit\n" );
    for( i = 1; i <= slen[FIL_A]; i++ ) {
        printf( "class[%d] = %d %06o\n", i, class[i], class[i] );
    }
#endif
}

static void *compact( void *ptr, size_t size, const char *why )
{
    char        *new_ptr;

    new_ptr = realloc( ptr, size );
    if( new_ptr == NULL )
        noroom( why );

#ifdef DEVBUILD
    if( new_ptr != ptr ) {
        fprintf( stderr, "moved from %06o to %06o\n", (unsigned)(pointer_int)ptr, (unsigned)(pointer_int)new_ptr );
    }
#endif
    return( new_ptr );
}


static SLONG newcand( SLONG a, SLONG b, SLONG pred )
/***************************************************
 * Generate maximum common subsequence chain in clist[]
 *   a    .. Line in file[FIL_A]
 *   b    .. Line in file[FIL_B]
 *   pred .. Line in file[FIL_B]
 */
{
    CANDIDATE   *new;

    clength++;
    clength++;
    if( clength >= csize ) {
        csize += CSIZE_INC;
        clist = MYCOMPACT( CANDIDATE, clist, csize, "extending clist" );
    }
    new = &clist[clength - 1];
    new->a = a;
    new->b = b;
    new->link = pred;
    return( clength - 1 );
}


/*
 * Search klist[low..top] (inclusive) for b.  If klist[low]->b >= b,
 * return zero.  Else return s such that klist[s-1]->b < b and
 * klist[s]->b >= b.  Note that the algorithm presupposes the two
 * preset "fence" elements, (0, 0) and (slen[FIL_A], slen[FIL_B]).
 */

static ULONG search( ULONG low, ULONG high, SLONG b )
{
    SLONG       temp;
    ULONG       mid;

    if( clist[klist[low]].b >= b )
        return( 0 );
    while( (mid = ( low + high ) / 2) > low ) {
        temp = clist[klist[mid]].b;
        if( temp > b ) {
            high = mid;
        } else if( temp < b ) {
            low = mid;
        } else {
            return( mid );
        }
    }
    return( mid + 1 );
}

static SLONG subseq( void )
{
    SLONG       a;
    ULONG       ktop;
    SLONG       b;
    ULONG       s;
    ULONG       r;
    SLONG       i;
    SLONG       cand;

    klist[0] = newcand( 0, 0, -1 );
    klist[1] = newcand( slen[FIL_A] + 1, slen[FIL_B] + 1, -1 );
    ktop = 1;                   /* -> guard entry  */
    for( a = 1; a <= slen[FIL_A]; a++ ) {
        /*
         * For each non-zero element in fileA ...
         */
        i = class[a];
        if( i == 0 )
            continue;
        cand = klist[0];        /* No candidate now  */
        r = 0;                  /* Current r-candidate  */
        do {
            /*
             * Perform the merge algorithm
             */
            b = member[i];
#ifdef DEVBUILD
            printf( "a = %d, i = %d, b = %d\n", a, i, b );
#endif
            if( b < 0 ) {
                b = -b;
            }
#ifdef DEVBUILD
            printf( "search(%d, %d, %d) -> %d\n", r, ktop, b, search( r, ktop, b ) );
#endif
            s = search( r, ktop, b );
            if( s != 0 ) {
                if( clist[klist[s]].b > b ) {
                    klist[r] = cand;
                    r = s;
                    cand = newcand( a, b, klist[s - 1] );
#ifdef DEVBUILD
                    dumpklist( ktop, "klist[s-1]->b > b" );
#endif
                }
                if( s >= ktop ) {
                    klist[ktop + 1] = klist[ktop];
                    ktop++;
#ifdef DEVBUILD
                    klist[r] = cand;
                    dumpklist( ktop, "extend" );
#endif
                    break;
                }
            }
        } while( member[++i] > 0 );
        klist[r] = cand;
    }
#ifdef DEVBUILD
    printf( "last entry = %d\n", ktop - 1 );
#endif
    return( ktop - 1 );          /* Last entry found  */
}


static void unravel( SLONG k )
{
    SLONG       i;
    CANDIDATE   *cp;
    SLONG       first_trailer;
    SLONG       difference;

    first_trailer = len[FIL_A] - suffix;
    difference = len[FIL_B] - len[FIL_A];
#ifdef DEVBUILD
    printf( "first trailer = %d, difference = %d\n", first_trailer, difference );
#endif
    for( i = 0; i <= len[FIL_A]; i++ ) {
        match[i] = ( i <= prefix ) ? i : ( ( i > first_trailer ) ? i + difference : 0 );
    }
#ifdef DEVBUILD
    printf( "unravel\n" );
#endif
    while( k != -1 ) {
        cp = &clist[k];
#ifdef DEVBUILD
        if( k < 0
          || k >= clength )
            error( "Illegal link -> %d", k );
        printf( "match[%d] := %d\n", cp->a + prefix, cp->b + prefix );
#endif
        match[cp->a + prefix] = cp->b + prefix;
        k = cp->link;
    }
}

/*
 * true if strings are identical
 */

static bool streq( const char *s1, const char *s2 )
{
    while( *s1++ == *s2 ) {
        if( *s2++ == EOS ) {
            return( true );
        }
    }
    return( false );
}

/*
 * Like fput() except that it puts a newline at the end of the line.
 */

static void fputss( char *s, FILE *iop )
{
    fputs( s, iop );
    putc( '\n', iop );
}

/*
 * Input routine, read one line to buffer[], return true on eof, else false.
 * The terminating newline is always removed.  If "-b" was given, trailing
 * whitespace (blanks and tabs) are removed and strings of blanks and
 * tabs are replaced by a single blank.  getinpline() does all hacking for
 * redirected input files.
 */

static bool getinpline( FILE *fd, char *buffer, int max_len )
{
    char     *top;
    char     *fromp;
    char      c;

    if( my_fgets( buffer, max_len, fd ) == NULL ) {
        *buffer = EOS;
        return( true );
    }
    if( fd == stdin ) {
        fputss( buffer, tempfd );
    }
    if( bflag
      || iflag ) {
        top = buffer;
        fromp = buffer;
        while( (c = *fromp++) != EOS ) {
            if( bflag
              && ( c == ' '
              || c == '\t' ) ) {
                c = ' ';
                while( *fromp == ' ' || *fromp == '\t' ) {
                    fromp++;
                }
            }
            if( iflag ) {
                c = tolower( c );
            }
            *top++ = c;
        }
        if( bflag
          && top[-1] == ' ' ) {
            top--;
        }
        *top = EOS;
    }
    return( false );
}

/*
 * Check for hash matches (jackpots) and collect random access indices to
 * the two files.
 *
 * It should be possible to avoid doing most of the ftell's by noticing
 * that we are not doing a context diff and noticing that if a line
 * compares equal to the other file, we will not ever need to know its
 * file position.  FIXME.
 */

static int check( const char *fileAname, const char *fileBname )
{
    SLONG       a;          /* Current line in file A  */
    SLONG       b;          /* Current line in file B  */
    SLONG       jackpot;

    /* unused parameters */ (void)fileAname; (void)fileBname;

    /*
     * The VAX C ftell() returns the address of the CURRENT record, not the
     * next one (as in DECUS C or, effectively, other C's).  Hence, the values
     * are "off by one" in the array.  OFFSET compensates for this.
     */

#define OFFSET 0

    b = 1;
    rewind( infd[FIL_A] );
    rewind( infd[FIL_B] );
    /*
     * See above; these would be over-written on VMS anyway.
     */

    oldseek[0] = ftell( infd[FIL_A] );
    newseek[0] = ftell( infd[FIL_B] );

    jackpot = 0;
#ifdef DEVBUILD
    printf( "match vector\n" );
    for( a = 0; a <= len[FIL_A]; a++ )
        printf( "match[%d] = %d\n", a, match[a] );
#endif
    for( a = 1; a <= len[FIL_A]; a++ ) {
        if( match[a] == 0 ) {
            /* Unique line in A */
            oldseek[a + OFFSET] = ftell( infd[FIL_A] );
            getinpline( infd[FIL_A], text[FIL_A], sizeof( text[FIL_A] ) );
            continue;
        }
        while( b < match[a] ) {
            /* Skip over unique lines in B */
            newseek[b + OFFSET] = ftell( infd[FIL_B] );
            getinpline( infd[FIL_B], text[FIL_B], sizeof( text[FIL_B] ) );
            b++;
        }

        /*
         * Compare the two, supposedly matching, lines. Unless we are going
         * to print these lines, don't bother to remember where they are.  We
         * only print matching lines if a context diff is happening, or if a
         * jackpot occurs.
         */
        //      if (cflag) {
        oldseek[a + OFFSET] = ftell( infd[FIL_A] );
        newseek[b + OFFSET] = ftell( infd[FIL_B] );
        //      }
        getinpline( infd[FIL_A], text[FIL_A], sizeof( text[FIL_A] ) );
        getinpline( infd[FIL_B], text[FIL_B], sizeof( text[FIL_B] ) );
        if( !streq( text[FIL_A], text[FIL_B] ) ) {
#ifdef DEVBUILD
            fprintf( stderr, "Spurious match:\n" );
            fprintf( stderr, "line %d in %s, \"%s\"\n", a, fileAname, text[FIL_A] );
            fprintf( stderr, "line %d in %s, \"%s\"\n", b, fileBname, text[FIL_B] );
#endif
            match[a] = 0;
            jackpot++;
        }
        b++;
    }
    for( ; b <= len[FIL_B]; b++ ) {
        newseek[b + OFFSET] = ftell( infd[FIL_B] );
        getinpline( infd[FIL_B], text[FIL_B], sizeof( text[FIL_B] ) );
    }
    /*
     * The logical converse to the code up above, for NON-VMS systems, to
     * store away an fseek() pointer at the beginning of the file.  For VMS,
     * we need one at EOF...
     */

    return( jackpot );
}

/*
 * Print a range
 */

static void range( SLONG from, SLONG to, int which )
{
    if( cflag ) {
        from -= cflag;
        if( from <= 0 ) {
            from = 1;
        }
        to += cflag;
        if( to > len[which] ) {
            to = len[which];
        }
    }
    if( to > from ) {
        printf( "%ld,%ld", from, to );
    } else if( to < from ) {
        printf( "%ld,%ld", to, from );
    } else {
        printf( "%ld", from );
    }
}

static void internal_error( const char *format, ... )
{
    va_list args;

    va_start( args, format );
    fprintf( stderr, "Internal error: " );
    vfprintf( stderr, format, args );
    va_end( args );
    putc( '\n', stderr );
}

/*
 * Print the appropriate text
 */

static void fetch( long *seekvec, SLONG start, SLONG end, int which, const char *pfx )
{
    SLONG       i;
    SLONG       first;
    SLONG       last;

    if( cflag ) {
        first = start - cflag;
        if( first <= 0 ) {
            first = 1;
        }
        last = end + cflag;
        if( last > len[which] ) {
            last = len[which];
        }
    } else {
        first = start;
        last = end;
    }
    if( fseek( infd[which], seekvec[first], 0 ) != 0 ) {
        internal_error( "?Can't read line %d at %08lx (hex) in file%c\n", start,
            seekvec[first], symbol[which] );
        exit( DIFF_NOT_COMPARED );
    } else {
        for( i = first; i <= last; i++ ) {
            if( my_fgets( text[which], sizeof( text[which] ), infd[which] ) == NULL ) {
                internal_error( "** Unexpected end of file\n" );
                exit( DIFF_NOT_COMPARED );
            }
#ifdef DEVBUILD
            printf( "%5d: %s%s\n", i, pfx, text[which] );
#else
            fputs( ( cflag && ( i < start || i > end ) ) ? "  " : pfx, stdout );
            fputs( text[which], stdout );
            putchar( '\n' );
#endif
        }
    }
}


static void change( SLONG astart, SLONG aend, SLONG bstart, SLONG bend )
/***********************************************************************
 * Output a change entry: fileA[astart..aend] changed to fileB[bstart..bend]
 */
{
    char        c;

    /*
     * This catches a "dummy" last entry
     */
    if( astart > aend
      && bstart > bend )
        return;
    diff_rc = DIFF_HAVE_DIFFS;
    c = ( astart > aend ) ? 'a' : ( ( bstart > bend ) ? 'd' : 'c' );
    if( nflag ) {
        if( c == 'a' ) {
            printf( "a%ld %ld\n", astart - 1, bend - bstart + 1 );
            fetch( newseek, bstart, bend, FIL_B, "" );
        } else if( c == 'd' ) {
            printf( "d%ld %ld\n", astart, aend - astart + 1 );
        } else {
            printf( "d%ld %ld\n", astart, aend - astart + 1 );
            printf( "a%ld %ld\n", aend, bend - bstart + 1 );
            fetch( newseek, bstart, bend, FIL_B, "" );
        }
        return;
    }
    if( cflag ) {
        fputs( "**************\n*** ", stdout );
    }

    if( c == 'a'
      && cflag == 0 ) {
        /*
         * Addition: just print one odd #
         */
        range( astart - 1, astart - 1, FIL_A );
    } else {
        /*
         * Print both, if different
         */
        range( astart, aend, FIL_A );
    }
    if( cflag == 0 ) {
        putchar( c );
        if( !eflag ) {
            if( c == 'd' ) {
                /*
                 * Deletion: just print one odd #
                 */
                range( bstart - 1, bstart - 1, FIL_B );
            } else {
                /*
                 * Print both, if different
                 */
                range( bstart, bend, FIL_B );
            }
        }
    }
    putchar( '\n' );
    if( ( !eflag
      && c != 'a' )
      || cflag ) {
        fetch( oldseek, astart, aend, FIL_A, ( cflag ) ? ( ( c == 'd' ) ? "- " : "! " ) : "< " );
        if( cflag ) {
            fputs( "--- ", stdout );
            range( bstart, bend, FIL_B );
            fputs( " -----\n", stdout );
        } else if( astart <= aend
          && bstart <= bend ) {
            printf( "---\n" );
        }
    }
    if( bstart <= bend ) {
        fetch( newseek, bstart, bend, FIL_B, ( cflag ) ? ( ( c == 'a' ) ? "+ " : "! " ) : ( ( eflag ) ? "" : "> " ) );
        if( eflag ) {
            printf( ".\n" );
        }
    }
}

static void output( const char *fileAname, const char *fileBname )
{
    SLONG       astart;
    SLONG       aend = 0;
    SLONG       bstart;
    SLONG       bend;

    rewind( infd[FIL_A] );
    rewind( infd[FIL_B] );
    match[0] = 0;
    match[len[FIL_A] + 1] = len[FIL_B] + 1;
    if( !eflag ) {
        if( cflag ) {
            /*
             * Should include ctime style dates after the file names, but
             * this would be non-trivial on OSK.  Perhaps there should be a
             * special case for stdin.
             */
            printf( "*** %s\n--- %s\n", fileAname, fileBname );
        }
        /*
         * Normal printout
         */
        for( astart = 1; astart <= len[FIL_A]; astart = aend + 1 ) {
            /*
             * New subsequence, skip over matching stuff
             */
            while( astart <= len[FIL_A] && match[astart] == ( match[astart - 1] + 1 ) ) {
                astart++;
            }
            /*
             * Found a difference, setup range and print it
             */
            bstart = match[astart - 1] + 1;
            aend = astart - 1;
            while( aend < len[FIL_A] && match[aend + 1] == 0 ) {
                aend++;
            }
            bend = match[aend + 1] - 1;
            match[aend] = bend;
            change( astart, aend, bstart, bend );
        }
    } else {
        /*
         * Edit script output -- differences are output "backwards" for the
         * benefit of a line-oriented editor.
         */
        for( aend = len[FIL_A]; aend >= 1; aend = astart - 1 ) {
            while( aend >= 1 && match[aend] == ( match[aend + 1] - 1 )
                   && match[aend] != 0 ) {
                aend--;
            }
            bend = match[aend + 1] - 1;
            astart = aend + 1;
            while( astart > 1 && match[astart - 1] == 0 ) {
                astart--;
            }
            bstart = match[astart - 1] + 1;
            match[astart] = bstart;
            change( astart, aend, bstart, bend );
        }
    }
    if( len[FIL_A] == 0 ) {
        change( 1, 0, 1, len[FIL_B] );
    }
}

static USHORT    crc16a[] = {
    0000000, 0140301, 0140601, 0000500,
    0141401, 0001700, 0001200, 0141101,
    0143001, 0003300, 0003600, 0143501,
    0002400, 0142701, 0142201, 0002100,
};

static USHORT    crc16b[] = {
    0000000, 0146001, 0154001, 0012000,
    0170001, 0036000, 0024000, 0162001,
    0120001, 0066000, 0074000, 0132001,
    0050000, 0116001, 0104001, 0043000,
};

/*
 * Return the CRC16 hash code for the buffer
 * Algorithm from Stu Wecker (Digital memo 130-959-002-00).
 */

static USHORT hash( const char *buffer )
{
    USHORT      crc;
    const char  *tp;
    SSHORT      temp;

    crc = 0;
    for( tp = buffer; *tp != EOS; ) {
        temp = *tp++ ^ crc;     /* XOR crc with new char  */
        crc = ( crc >> 8 ) ^ crc16a[temp & 0017]
            ^ crc16b[(temp & 0360) >> 4];
    }
    return( ( crc == 0 ) ? (USHORT)1 : crc );
}

/*
 * Can't open file message
 */

static void cant( const char *filename, const char *what )
{
    fprintf( stderr, "Can't open %s file \"%s\": ", what, filename );
    perror( NULL );
}

/*
 * Look for initial and trailing sequences that have identical hash values.
 * Don't bother building them into the candidate vector.
 */

static void squish( void )
{
    SLONG       i;
    LINE        *ap;
    LINE        *bp;
    SLONG       j;
    SLONG       k;
    int         which;

    /*
     * prefix -> first line (from start) that doesn't hash identically
     */
    i = 0;
    ap = &file[FIL_A][1];
    bp = &file[FIL_B][1];
    while( i < len[FIL_A] && i < len[FIL_B] && ap->hash == bp->hash ) {
        i++;
        ap++;
        bp++;
    }
    prefix = i;

    /*
     * suffix -> first line (from end) that doesn't hash identically
     */
    j = len[FIL_A] - i;
    k = len[FIL_B] - i;
    ap = &file[FIL_A][len[FIL_A]];
    bp = &file[FIL_B][len[FIL_B]];
    i = 0;
    while( i < j && i < k && ap->hash == bp->hash ) {
        i++;
        ap--;
        bp--;
    }
    suffix = i;

    /*
     * Tuck the counts away
     */
    for( which = 0; which < FIL_SIZE; which++ ) {
        ap = sfile[which] = file[which] + prefix;
        j = slen[which] = len[which] - prefix - suffix;
        for( i = 0; i <= j; i++, ap++ ) {
            ap->serial = i;
        }
    }
}

/*
 * Read the file, building hash table
 */

static void input( int which )
    /* FIL_A or FIL_B to redefine infd[]  */
{
    LINE        *lentry;
    SLONG       linect = 0;
    FILE        *fd;
#define LSIZE_INC 200           /* # of line entries to alloc at once */
    SLONG       lsize = LSIZE_INC;

    lentry = MYALLOC( LINE, lsize + 3, "line" );
    fd = infd[which];
    while( !getinpline( fd, text[which], sizeof( text[which] ) ) ) {
        if( ++linect >= lsize ) {
            lsize += 200;
            lentry = MYCOMPACT( LINE, lentry, lsize + 3, "extending line vector" );
        }
        lentry[linect].hash = hash( text[which] );
    }

    /*
     * If input was from stdin ("-" command), finish off the temp file.
     */
    if( fd == stdin ) {
        fclose( tempfd );
        tempfd = infd[which] = fopen( TEMPFILE, "r" );
    }

    /*
     * If we wanted to be stingy with memory, we could realloc lentry down to
     * its exact size (+3 for some odd reason) here.  No need?
     */
    len[which] = linect;
    file[which] = lentry;
}

/*
 * Diff main program
 */

int main( int argc, char **argv )
{
    SLONG           i;
    char            *ap;
    struct stat     st;
    char            path[_MAX_PATH];
    pgroup2         pg;
    int             which;

    while( argc > 1 && *( ap = argv[1] ) == '-' && *++ap != EOS ) {
        while( *ap != EOS ) {
            switch( ( *ap++ ) ) {
            case 'b':
                bflag = true;
                break;
            case 'c':
                if( *ap > '0'
                  && *ap <= '9' ) {
                    cflag = *ap++ - '0';
                } else {
                    cflag = 3;
                }
                break;
            case 'e':
                eflag++;
                break;
            case 'd':
                Dflag = ap;
                while( *ap != EOS )
                    ++ap;
                break;
            case 'H':
                Hflag++;
                break;
            case 'n':
                nflag++;
                break;
            case 'i':
                iflag++;
                break;
            case 't':
                tflag++;
                break;
            default:
                error( "bad option '-%c'\n", ap[-1] );
                return( DIFF_NOT_COMPARED );
            }
        }
        argc--;
        argv++;
    }

    if( argc != 3 ) {
        error( cmdusage );
        return( DIFF_NOT_COMPARED );
    }
    if( nflag + ( ( cflag ) ? 1 : 0 ) + eflag > 1 ) {
        error( " -c, -n and -e are incompatible.\n" );
        return( DIFF_NOT_COMPARED );
    }
    argv++;
    for( which = 0; which < FIL_SIZE; which++ ) {
        if( argv[which][0] == '-'
          && argv[which][1] == EOS ) {
            infd[which] = stdin;
            tempfd = fopen( TEMPFILE, "w" );
            if( tempfd == NULL ) {
                cant( TEMPFILE, "work" );
                exit( DIFF_NOT_COMPARED );
            }
        } else {
            strcpy( path, argv[which] );
            if( which == FIL_B
              && stat( argv[which], &st ) == 0
              && S_ISDIR( st.st_mode ) ) {
                _splitpath2( argv[which - 1], pg.buffer, NULL, NULL, &pg.fname, &pg.ext );
                _makepath( path, NULL, argv[which], pg.fname, pg.ext );
            }
            infd[which] = fopen( path, "r" );
            if( infd[which] == NULL ) {
                cant( path, "input" );
                exit( DIFF_NOT_COMPARED );
            }
        }
    }

    if( infd[FIL_A] == stdin
      && infd[FIL_B] == stdin ) {
        error( "Can't diff two things both on standard input." );
        return( DIFF_NOT_COMPARED );
    }
    if( infd[FIL_A] == NULL
      && infd[FIL_B] == NULL ) {
        cant( argv[FIL_A], "input" );
        cant( argv[FIL_B], "input" );
        exit( DIFF_NOT_COMPARED );
    }

    /*
     * Read input, building hash tables.
     */
    input( FIL_A );
    input( FIL_B );
    squish();
#ifdef DEVBUILD
    printf( "before sort\n" );
    dump_sfile_data( FIL_A );
    dump_sfile_data( FIL_B );
#endif
    sort( FIL_A );
    sort( FIL_B );
#ifdef DEVBUILD
    printf( "after sort\n" );
    dump_sfile_data( FIL_A );
    dump_sfile_data( FIL_B );
#endif

    /*
     * Build equivalence classes.
     */
    member = (SSHORT *)file[FIL_B];
    equiv();
    member = (SSHORT *)MYCOMPACT( SLONG, member, slen[FIL_B] + 2, "squeezing member vector" );
    file[FIL_B] = (LINE *)member;

    /*
     * Reorder equivalence classes into array class[]
     */
    class = (SSHORT *)file[FIL_A];
    unsort();
    class = (SSHORT *)MYCOMPACT( SLONG, class, slen[FIL_A] + 2, "compacting class vector" );
    file[FIL_A] = (LINE *)class;
    /*
     * Find longest subsequences
     */
    klist = MYALLOC( SLONG, slen[FIL_A] + 2, "klist" );
    clist = MYALLOC( CANDIDATE, csize, "clist" );
    i = subseq();
    MYFREE( member );
    file[FIL_B] = NULL;
    MYFREE( class );
    file[FIL_A] = NULL;
    match = MYALLOC( SLONG, len[FIL_A] + 2, "match" );
    unravel( klist[i] );
    MYFREE( clist );
    MYFREE( klist );

    /*
     * Check for fortuitous matches and output differences
     */
    oldseek = MYALLOC( long, len[FIL_A] + 2, "oldseek" );
    newseek = MYALLOC( long, len[FIL_B] + 2, "newseek" );
    if( check( argv[FIL_A], argv[FIL_B] ) ) {
#ifdef DEVBUILD
        fprintf( stderr, "Spurious match, output is not optimal\n" );
#else
        ;
#endif
    }
    output( argv[FIL_A], argv[FIL_B] );
    if( tempfd != NULL ) {
        fclose( tempfd );
        remove( TEMPFILE );
    }
    MYFREE( oldseek );
    MYFREE( newseek );
    MYFREE( file[FIL_A] );
    MYFREE( file[FIL_B] );

    return( diff_rc );
}
