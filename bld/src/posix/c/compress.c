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
* Description:  LZW file compression ala IEEE Computer, June 1984.
*
****************************************************************************/


/*
 * Authors: Spencer W. Thomas, Jim McKie, Steve Davies, Ken Turkowski
 *          James A. Woods, Joe Orost, Dave Mack        
 */


#include <ctype.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <utime.h>
#include <fcntl.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "util.h"


static const char *usage_text[] = {
    "Usage:\tcompress [-?cdfv] [-b bits] [file ...]",
    "\tfile        : files to compress or uncompress, stdin",
    "\t\t      if none specified.",
    "\tOptions:",
    "\t\t -? : print this list",
    "\t\t -b : limit max number of bits/code to `bits'",
    "\t\t -c : write output on stdout, don't remove original",
    "\t\t -d : decompress input (compresses by default)",
    "\t\t -f : force output file to be generated, even if one",
    "\t\t      already exists, or if no space is saved",
    "\t\t -v : write verbose compression statistics to stderr",
    NULL
};

#ifdef min
#undef min
#endif

#define min(a,b)    ((a>b) ? b : a)

/* MAXPATHLEN - maximum length of a pathname we allow */
#define MAXPATHLEN 1024

/* Setup for 16-bit machines */
#if !(INT_MAX > 65536L)
    #define BITS            12
    #define IO_BUF_SIZE     4096
#endif

#ifndef BITS
    #define BITS 16
#endif

#ifndef IO_BUF_SIZE
    #define IO_BUF_SIZE     65536
#endif

#if BITS == 16
  #define HSIZE 69001       /* 95% occupancy */
#endif
#if BITS == 15
  #define HSIZE 35023       /* 94% occupancy */
#endif
#if BITS == 14
  #define HSIZE 18013       /* 91% occupancy */
#endif
#if BITS == 13
  #define HSIZE 9001        /* 91% occupancy */
#endif
#if BITS <= 12
  #define HSIZE 5003        /* 80% occupancy */
#endif

/*
 * a code_int must be able to hold 2**BITS values of type int, and also -1
 */
#if BITS > 15
typedef long int        code_int;
#else
typedef int             code_int;
#endif

typedef long int        count_int;
typedef unsigned char   char_type;

/* Defines for third byte of header */
#define BIT_MASK    0x1f
#define BLOCK_MASK  0x80

/* Masks 0x40 and 0x20 are free.  0x20 could mean that there is
 * a fourth header byte (for expansion).
 */

#define INIT_BITS   9                   /* initial number of bits/code */

static char_type    magic_header[] = { "\037\235" };    /* 1F 9D */
static int n_bits;                      /* number of bits/code */
static int maxbits = BITS;              /* user settable max # bits/code */
static code_int maxcode;                /* maximum code, given n_bits */
static code_int maxmaxcode = 1 << BITS; /* should NEVER generate this code */
static code_int free_ent = 0;           /* first unused entry */
static int exit_stat = 0;
static int nomagic = 0;     /* use a 3-byte magic number header, unless old file */
static int zcat_flg = 0;    /* write output on stdout, suppress messages */
static int quiet = 1;       /* don't tell me about compression */
static int do_decomp = 0;
static int overwrite = 0;   /* do not overwrite unless given -f flag */

#define MAXCODE(n_bits) ((1 << (n_bits)) - 1)

static count_int        htab[HSIZE];
static unsigned short   codetab[HSIZE];

#define htabof(i)       htab[i]
#define codetabof(i)    codetab[i]

static code_int         hsize = HSIZE;  /* for dynamic table sizing */
static count_int        fsize;

/*
 * To save much memory, we overlay the table used by compress() with those
 * used by decompress().  The tab_prefix table is the same size and type
 * as the codetab.  The tab_suffix table needs 2**BITS characters.  We
 * get this from the beginning of htab.  The output stack uses the rest
 * of htab, and contains characters.  There is plenty of room for any
 * possible stack (stack used to be 8000 characters).
 */

#define tab_prefixof(i) codetabof(i)
#define tab_suffixof(i) ((char_type *)(htab))[i]
#define de_stack        ((char_type *)&tab_suffixof(1<<BITS))

/*
 * block compression parameters -- after all codes are used up,
 * and compression rate changes, start over.
 */
static int          block_compress = BLOCK_MASK;
static int          clear_flg = 0;
static long int     ratio = 0;

/* ratio check interval */
#if BITS == 16
    #define CHECK_GAP 50000 
#else
    #define CHECK_GAP 10000 /* ratio check interval */
#endif
static count_int checkpoint = CHECK_GAP;

/*
 * the next two codes should not be changed lightly, as they must not
 * lie within the contiguous general code space.
 */
#define FIRST   257 /* first free entry */
#define CLEAR   256 /* table clear output code */

static int force = 0;
static int valid = 0;       /* set when signal can remove ofname */
static char ofname[MAXPATHLEN];
static void (*bgnd_flag)();
static struct stat  statbuf, insbuf;

static int      offset;
static long     in_count = 1;       /* length of input */
static long     bytes_out;          /* length of compressed output */
static long     out_count = 0;      /* # of codes output (for debugging) */

static char buf[BITS];

static char_type lmask[9] = { 0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80, 0x00 };
static char_type rmask[9] = { 0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff };


/*
 * This routine returns 1 if we are running in the foreground and stderr
 * is a tty.
 */
static int foreground( void )
/***************************/
{
    if( bgnd_flag ) {           /* background? */
        return( 0 );
    } else {                    /* foreground */
        if( isatty( 2 ) ) {     /* and stderr is a tty */
            return( 1 );
        } else {
            return( 0 );
        }
    }
}

static void onintr( int sig )
/***************************/
{
    if( valid )
        unlink( ofname );
    exit( 1 );
}

static void oops( int sig ) /* wild pointer -- assume bad input */
/*************************/
{
    if( do_decomp == 1 )
        fprintf( stderr, "uncompress: corrupt input\n" );
    if( valid )
        unlink( ofname );
    exit( 1 );
}

static void writeerr( void )
/**************************/
{
    perror( ofname );
    if( valid )
        unlink( ofname );
    exit( 1 );
}

static void print_ratio( FILE *stream, long num, long den )
/*********************************************************/
{
    int     q;          /* Doesn't need to be long */

    if( num > 214748L ) {       /* 2147483647/10000 */
        q = num / (den / 10000L);
    } else {
        q = 10000L * num / den;     /* Long calculations, though */
    }
    if( q < 0 ) {
        putc( '-', stream );
        q = -q;
    }
    fprintf( stream, "%d.%02d%%", q / 100, q % 100 );
}

/*****************************************************************
 * Output the given code.
 * Inputs:
 *  code:   A n_bits-bit integer.  If == -1, then EOF.  This assumes
 *          that n_bits =< (long)wordsize - 1.
 * Outputs:
 *  Outputs code to the file.
 * Assumptions:
 *  Chars are 8 bits long.
 * Algorithm:
 *  Maintain a BITS character long buffer (so that 8 codes will
 *  fit in it exactly). When the buffer fills up empty it and start over.
 */

static void output( code_int code )
/*********************************/
{
    int     r_off = offset, bits = n_bits;
    char    *bp = buf;

    if( code >= 0 ) {
        /*
         * Get to the first byte.
         */
        bp += (r_off >> 3);
        r_off &= 7;
        /*
         * Since code is always >= 8 bits, only need to mask the first
         * hunk on the left.
         */
        *bp = (*bp & rmask[r_off]) | (code << r_off) & lmask[r_off];
        bp++;
        bits -= (8 - r_off);
        code >>= 8 - r_off;
        /* Get any 8 bit parts in the middle (<=1 for up to 16 bits). */
        if( bits >= 8 ) {
            *bp++ = code;
            code >>= 8;
            bits -= 8;
        }
        /* Last bits. */
        if( bits )
            *bp = code;

        offset += n_bits;
        if( offset == (n_bits << 3) ) {
            bp = buf;
            bits = n_bits;
            bytes_out += bits;
            do {
                putchar( *bp++ );
            } while( --bits );
            offset = 0;
        }

        /*
         * If the next entry is going to be too big for the code size,
         * then increase it, if possible.
         */
        if( free_ent > maxcode || (clear_flg > 0) ) {
            /*
             * Write the whole buffer, because the input side won't
             * discover the size increase until after it has read it.
             */
            if( offset > 0 ) {
                if( fwrite( buf, 1, n_bits, stdout ) != n_bits )
                    writeerr();
                bytes_out += n_bits;
            }
            offset = 0;

            if( clear_flg ) {
                maxcode = MAXCODE (n_bits = INIT_BITS);
                clear_flg = 0;
            }
            else {
                n_bits++;
                if ( n_bits == maxbits )
                    maxcode = maxmaxcode;
                else
                    maxcode = MAXCODE(n_bits);
            }
        }
    } else {
        /* At EOF, write the rest of the buffer */
        if( offset > 0 )
            fwrite( buf, 1, (offset + 7) / 8, stdout );
        bytes_out += (offset + 7) / 8;
        offset = 0;
        fflush( stdout );
        if( ferror( stdout ) )
            writeerr();
    }
}

static void cl_hash( count_int hsize )      /* reset code table */
/************************************/
{
    memset( htab, -1, hsize * sizeof( count_int ) );
}

static void cl_block( void )        /* table clear for block compress */
/**************************/
{
    long int    rat;

    checkpoint = in_count + CHECK_GAP;

    if( in_count > 0x007fffff ) {   /* shift will overflow */
        rat = bytes_out >> 8;
        if( rat == 0 ) {        /* Don't divide by zero */
            rat = 0x7fffffff;
        } else {
            rat = in_count / rat;
        }
    } else {
        rat = (in_count << 8) / bytes_out;  /* 8 fractional bits */
    }
    if( rat > ratio ) {
        ratio = rat;
    } else {
        ratio = 0;
        cl_hash( (count_int)hsize );
        free_ent = FIRST;
        clear_flg = 1;
        output( (code_int)CLEAR );
    }
}

/*
 * compress stdin to stdout
 *
 * Algorithm:  use open addressing double hashing (no chaining) on the
 * prefix code / next character combination.  We do a variant of Knuth's
 * algorithm D (vol. 3, sec. 6.4) along with G. Knott's relatively-prime
 * secondary probe.  Here, the modular division first probe is gives way
 * to a faster exclusive-or manipulation.  Also do block compression with
 * an adaptive reset, whereby the code table is cleared when the compression
 * ratio decreases, but after the table fills.  The variable-length output
 * codes are re-sized at this point, and a special CLEAR code is generated
 * for the decompressor.  Late addition:  construct the table according to
 * file size for noticeable speed improvement on small files.
 */

static void compress( void )
/**************************/
{
    long        fcode;
    code_int    i = 0;
    int         c;
    code_int    ent;
    int         disp;
    code_int    hsize_reg;
    int         hshift;

    if( nomagic == 0 ) {
        putchar( magic_header[0] );
        putchar( magic_header[1] );
        putchar( (char)(maxbits | block_compress) );
        if( ferror( stdout ) )
            writeerr();
    }

    offset = 0;
    bytes_out = 3;      /* includes 3-byte header mojo */
    out_count = 0;
    clear_flg = 0;
    ratio = 0;
    in_count = 1;
    checkpoint = CHECK_GAP;
    maxcode = MAXCODE(n_bits = INIT_BITS);
    free_ent = ((block_compress) ? FIRST : 256 );

    ent = getchar();

    hshift = 0;
    for( fcode = (long)hsize;  fcode < 65536L; fcode *= 2L )
        hshift++;
    hshift = 8 - hshift;        /* set hash code range bound */

    hsize_reg = hsize;
    cl_hash( (count_int) hsize_reg);        /* clear hash table */

    while( (c = getchar()) != EOF ) {
        in_count++;
        fcode = ((long)c << maxbits) + ent;
        i = (c << hshift) ^ ent;            /* xor hashing */

        if( htabof (i) == fcode ) {
            ent = codetabof (i);
            continue;
        } else if( (long)htabof (i) < 0 )   /* empty slot */
            goto nomatch;
        disp = hsize_reg - i;               /* secondary hash (after G. Knott) */
        if( i == 0 )
            disp = 1;
probe:
        if( (i -= disp) < 0 )
            i += hsize_reg;

        if( htabof (i) == fcode ) {
            ent = codetabof (i);
            continue;
        }
        if( (long)htabof (i) > 0 )
            goto probe;
nomatch:
        output ( (code_int) ent );
        out_count++;
        ent = c;
        if( free_ent < maxmaxcode ) {
            codetabof( i ) = free_ent++;    /* code -> hashtable */
            htabof( i ) = fcode;
        } else if( (count_int)in_count >= checkpoint && block_compress )
            cl_block ();
    }
    /* Output the final code */
    output( (code_int)ent );
    out_count++;
    output( (code_int)-1 );

    /* Print out stats on stderr */
    if( zcat_flg == 0 && !quiet ) {
        fprintf( stderr, "Compression: " );
        print_ratio( stderr, in_count-bytes_out, in_count );
    }
    if( bytes_out > in_count )  /* exit(2) if no savings */
        exit_stat = 2;
}

/*****************************************************************
 * Read one code from the standard input.  If EOF, return -1.
 * Inputs:
 *  stdin
 * Outputs:
 *  code or -1 is returned.
 */

static code_int getcode( void )
/*****************************/
{
    code_int            code;
    static int          offset = 0, size = 0;
    static char_type    buf[BITS];
    int                 r_off, bits;
    char_type           *bp = buf;

    if( clear_flg > 0 || offset >= size || free_ent > maxcode ) {
    /*
     * If the next entry will be too big for the current code
     * size, then we must increase the size.  This implies reading
     * a new buffer full, too.
     */
    if( free_ent > maxcode ) {
        n_bits++;
        if( n_bits == maxbits )
            maxcode = maxmaxcode;   /* won't get any bigger now */
        else
            maxcode = MAXCODE(n_bits);
    }
    if( clear_flg > 0) {
        maxcode = MAXCODE(n_bits = INIT_BITS);
        clear_flg = 0;
    }
    size = fread( buf, 1, n_bits, stdin );
    if( size <= 0 )
        return -1;          /* end of file */
    offset = 0;
    /* Round size down to integral number of codes */
    size = (size << 3) - (n_bits - 1);
    }
    r_off = offset;
    bits = n_bits;
    /*
     * Get to the first byte.
     */
    bp += (r_off >> 3);
    r_off &= 7;
    /* Get first part (low order bits) */
    code = (*bp++ >> r_off);
    bits -= (8 - r_off);
    r_off = 8 - r_off;      /* now, offset into code word */
    /* Get any 8 bit parts in the middle (<=1 for up to 16 bits). */
    if( bits >= 8 ) {
        code |= (*bp++ & 0xff) << r_off;
        r_off += 8;
        bits -= 8;
    }
    /* high order bits. */
    code |= (*bp & rmask[bits]) << r_off;
    offset += n_bits;

    return( code );
}

/*
 * Decompress stdin to stdout.  This routine adapts to the codes in the
 * file building the "string" table on-the-fly; requiring no table to
 * be stored in the compressed file.  The tables used herein are shared
 * with those of the compress() routine.  See the definitions above.
 */

static void decompress( void )
/****************************/
{
    char_type   *stackp;
    int         finchar;
    code_int    code, oldcode, incode;

    /* As above, initialize the first 256 entries in the table. */
    maxcode = MAXCODE(n_bits = INIT_BITS);
    for( code = 255; code >= 0; code-- ) {
        tab_prefixof(code) = 0;
        tab_suffixof(code) = (char_type)code;
    }
    free_ent = ((block_compress) ? FIRST : 256 );

    finchar = oldcode = getcode();
    if( oldcode == -1 )         /* EOF already? */
        return;                 /* Get out of here */
    putchar( (char)finchar );   /* first code must be 8 bits = char */
    if( ferror( stdout ) )      /* Crash if can't write */
        writeerr();
    stackp = de_stack;

    while( (code = getcode()) > -1 ) {
        if( (code == CLEAR) && block_compress ) {
            for ( code = 255; code >= 0; code-- )
                tab_prefixof(code) = 0;
            clear_flg = 1;
            free_ent = FIRST - 1;
            if( (code = getcode()) == -1 )  /* O, untimely death! */
                break;
        }
        incode = code;
        /* Special case for KwKwK string */
        if ( code >= free_ent ) {
                *stackp++ = finchar;
            code = oldcode;
        }

        /* Generate output characters in reverse order */
        while( code >= 256 ) {
            *stackp++ = tab_suffixof(code);
            code = tab_prefixof(code);
        }
        *stackp++ = finchar = tab_suffixof(code);

        /* And put them out in forward order */
        do {
            putchar ( *--stackp );
        } while( stackp > de_stack );

        /* Generate the new entry */
        if ( (code=free_ent) < maxmaxcode ) {
            tab_prefixof(code) = (unsigned short)oldcode;
            tab_suffixof(code) = finchar;
            free_ent = code + 1;
        }
        /* Remember previous code */
        oldcode = incode;
    }
    fflush( stdout );
    if( ferror( stdout ) )
        writeerr();
}

static void copystat( char *ifname, char *ofname )
/************************************************/
{
    int             mode;
    struct utimbuf  times;

    fclose( stdin );
    fclose( stdout );
    if( exit_stat == 2 && (!force) ) {  /* No compression: remove file.Z */
    if( !quiet )
        fprintf( stderr, "No compression -- %s unchanged", ifname );
    } else {            /* ***** Successful Compression ***** */
        exit_stat = 0;
        mode = insbuf.st_mode & 07777;
        if( chmod( ofname, mode ) )     /* Copy modes */
            perror( ofname );
#ifdef __UNIX__
        chown( ofname, insbuf.st_uid, insbuf.st_gid );  /* Copy ownership */
#endif
        times.actime  = insbuf.st_atime;
        times.modtime = insbuf.st_mtime;
        utime( ofname, &times );    /* Update last accessed and modified times */
        valid = 0;                  /* prevent latent ofname removal */
        if( unlink( ifname ) )      /* Remove input file */
            perror( ifname );
        if( !quiet )
            fprintf( stderr, " -- replaced with %s", ofname );
        return;     /* Successful return */
    }

    /* Unsuccessful return -- one of the tests failed */
    if( unlink( ofname ) )
    perror( ofname );
}

static void compress_file( char **fileptr )
/*****************************************/
{
    char    tempname[MAXPATHLEN];
#ifdef SHORTNAMES
    char    *cp;
#endif

    strcpy( tempname, *fileptr );
    errno = 0;
    if( stat( tempname, &insbuf ) == -1 ) {
        if( do_decomp ) {
            switch( errno ) {
            case ENOENT:    /* file doesn't exist */
                /*
                ** if the given name doesn't end with .Z, try appending one
                ** This is obviously the wrong thing to do if it's a
                ** directory, but it shouldn't do any harm.
                */
                if( stricmp( tempname + strlen( tempname ) - 2, ".Z" ) != 0 ) {
#ifdef SHORTNAMES
                    if( (cp = strrchr( tempname, DIR_SEP_CHAR ) ) != NULL )
                        cp++;
                    else                    
                        cp = tempname;
                    if( strlen( cp ) > 12 ) {
                        fprintf( stderr, "%s.Z: No such file or directory\n", tempname );
                        return;
                    }
#endif  /* SHORTNAMES */
                    strcat( tempname, ".Z" );
                    errno = 0;
                    if( stat( tempname, &insbuf ) == -1 ) {
                        perror( tempname );
                        return;
                    }
                } else {
                    perror( tempname );
                    return;
                }
                break;
            default:
                perror( tempname );
                return;
            }
        } else {
            /* we can't stat the file, ignore it */
            perror( tempname );
            return;
        }
    }

    switch( insbuf.st_mode & S_IFMT ) {
    case S_IFDIR:   /* directory */
        if( !quiet )
            fprintf( stderr, "%s is a directory -- ignored\n", tempname );
        break;
    case S_IFREG:   /* regular file */
        exit_stat = 0;
        if( do_decomp != 0 ) {
            /* DECOMPRESSION */
            if( !zcat_flg ) {
                if( stricmp( tempname + strlen( tempname ) - 2, ".Z" ) != 0 ) {
                    if( !quiet ) {
                        fprintf( stderr, "%s - no .Z suffix\n", tempname );
                    }
                    return;
                }
            }
            /* Open input file */
            if( freopen( tempname, "rb", stdin ) == NULL ) {
                perror( tempname );
                return;
            }
            setvbuf( stdin, NULL, _IOFBF, IO_BUF_SIZE );
            /* Check the magic number */
            if( nomagic == 0 ) {
                if ((getchar() != (magic_header[0] & 0xFF))
                 || (getchar() != (magic_header[1] & 0xFF))) {
                    fprintf( stderr, "%s: not in compressed format\n", tempname );
                    return;
                }
                maxbits = getchar();    /* set -b from file */
                block_compress = maxbits & BLOCK_MASK;
                maxbits &= BIT_MASK;
                maxmaxcode = 1 << maxbits;
                if( maxbits > BITS ) {
                    fprintf( stderr,
                         "%s: compressed with %d bits, can only handle %d bits\n",
                         tempname, maxbits, BITS );
                    return;
                }
            }
            /* we have to ignore SIGINT for a while, otherwise
             * a ^C can nuke an existing file with ofname
             */
            signal( SIGINT, SIG_IGN );
            /* Generate output filename */
            strcpy( ofname, tempname );
            /* Check for .Z suffix */
            if( stricmp( tempname + strlen( tempname ) - 2, ".Z" ) == 0 ) {
                ofname[strlen( tempname ) - 2] = '\0';  /* Strip off .Z */
            }
        } else {
            /* COMPRESSION */
            if( stricmp( tempname + strlen( tempname ) - 2, ".Z" ) == 0 ) {
                fprintf( stderr, "%s: already has .Z suffix -- no change\n",
                         tempname);
                return;
            }
            if( insbuf.st_nlink > 1 && (!force) ) {
                fprintf( stderr, "%s has %d other links: unchanged\n",
                         tempname, insbuf.st_nlink - 1 );
                return;
            }
            /* Open input file */
            if( freopen( tempname, "rb", stdin ) == NULL ) {
                perror( tempname );
                return;
            }
            fsize = (long)insbuf.st_size;
            /*
             * tune hash table size for small files -- ad hoc,
             * but the sizes match earlier #defines, which
             * serve as upper bounds on the number of output codes.
             */
            hsize = HSIZE;
            if( fsize < (1 << 12) )
                hsize = min( 5003, HSIZE );
            else if( fsize < (1 << 13) )
                hsize = min( 9001, HSIZE );
            else if( fsize < (1 << 14) )
                hsize = min( 18013, HSIZE );
            else if( fsize < (1 << 15) )
                hsize = min( 35023, HSIZE );
            else if( fsize < 47000 )
                hsize = min( 50021, HSIZE );

            /* we have to ignore SIGINT for a while, otherwise
             * a ^C can nuke an existing file with ofname
             */
            signal( SIGINT, SIG_IGN );
            /* Generate output filename */
            strcpy( ofname, tempname );
#ifdef SHORTNAMES   /* Short filenames */
            if( (cp = strrchr( ofname, DIR_SEP_CHAR ) ) != NULL )
                cp++;
            else
                cp = ofname;
            if( strlen( cp ) > 12 ) {
                fprintf( stderr, "%s: filename too long to tack on .Z\n", ofname );
                signal( SIGINT, onintr );
                return;
            }
#endif  /* SHORTNAMES */
            strcat( ofname, ".Z" );
        }
        /* Check for overwrite of existing file */
        if( overwrite == 0 && zcat_flg == 0 ) {
            if( stat( ofname, &statbuf ) == 0 ) {
                char    response[2];
            
                response[0] = 'n';
                fprintf( stderr, "%s already exists;", ofname );
                if( foreground() ) {
                    fprintf( stderr, " OK to overwrite (y or n)? " );
                    fflush( stderr );
                    read( 2, response, 2 );
                    while( response[1] != '\n' ) {
                        if( read( 2, response + 1, 1 ) < 0 ) {  /* Ack! */
                            perror( "stderr" );
                            break;
                        }
                    }
                }
                if( response[0] != 'y' ) {
                    fprintf( stderr, "\tnot overwritten\n" );
                    signal( SIGINT, onintr );
                    return;
                }
            }
        }
        signal( SIGINT, onintr );
        if( zcat_flg == 0 ) {       /* Open output file */
            valid = 1;
        if( freopen( ofname, "wb", stdout ) == NULL ) {
            perror( ofname );
            return;
        }
        if( !quiet )
            fprintf( stderr, "%s: ", tempname );
        } else {
            setmode( fileno( stdout ), O_BINARY );
        }
        setvbuf( stdout, NULL, _IOFBF, IO_BUF_SIZE );

        /* Actually do the compression/decompression */
        if( do_decomp == 0 )    
            compress();
        else            
            decompress();
        if( zcat_flg == 0 ) {
            copystat( tempname, ofname );   /* Copy stats */
        if( (exit_stat == 1) || (!quiet) )
            putc( '\n', stderr );
        }
        break;
    default:
        fprintf( stderr,"%s is not a directory or a regular file - ignored\n",
                 tempname );
        break;
    }
}

static int is_this( char *pathname, const char *name )
/****************************************************/
{
    const char  *base = basename( pathname );

#ifdef __UNIX__
    return( !strcmp( base, name ) );
#else
    return( !strnicmp( base, name, strlen( name ) ) );
#endif
}

int main( int argc, char **argv )
/*******************************/
{
    int     ch;

    if( (bgnd_flag = signal( SIGINT, SIG_IGN )) != SIG_IGN ) {
        signal( SIGINT, onintr );
    }
    signal( SIGSEGV, oops );

    if( is_this( argv[0], "uncompress" ) ) {
        do_decomp = 1;
    } else if( is_this( argv[0], "zcat" ) ) {
        do_decomp = 1;
        zcat_flg = 1;
    }

    while( (ch = getopt( argc, argv, ":b:cdfv" )) != -1 ) {
        switch( ch ) {
        case 'b':
            maxbits = atoi( optarg );
            break;
        case 'c':
            zcat_flg = 1;
            break;
        case 'C':
            block_compress = 0;
            break;
        case 'd':
            do_decomp = 1;
            break;
        case 'f':
            overwrite = 1;
            force = 1;
            break;
        case 'n':
            nomagic = 1;
            break;
        case 'v':
            quiet = 0;
            break;
        case 'q':
            quiet = 1;
            break;
        case ':':
            util_quit( usage_text, "missing maxbits\n" );
        case '?':
            util_quit( usage_text, NULL );
        }
    }

    /* Skip option arguments and argv[0] */
    argc = argc - optind;
    argv += optind;

    if( maxbits < INIT_BITS )
        maxbits = INIT_BITS;
    if( maxbits > BITS )
        maxbits = BITS;
    maxmaxcode = 1 << maxbits;

    if( *argv != NULL ) {
        for( ; *argv; ++argv ) {
            compress_file( argv );
        }
    } else {        /* Standard input */
        setmode( fileno( stdin ), O_BINARY );
        setmode( fileno( stdout ), O_BINARY );
        setvbuf( stdin, NULL, _IOFBF, IO_BUF_SIZE );
        setvbuf( stdout, NULL, _IOFBF, IO_BUF_SIZE );
        if( do_decomp == 0 ) {
            compress();
            if( !quiet )
                putc( '\n', stderr );
        } else {
            /* Check the magic number */
            if( nomagic == 0 ) {
                if( (getchar() != (magic_header[0] & 0xFF))
                 || (getchar() != (magic_header[1] & 0xFF)) ) {
                    fprintf( stderr, "stdin: not in compressed format\n" );
                    return( 1 );
                }
                maxbits = getchar();    /* set -b from file */
                block_compress = maxbits & BLOCK_MASK;
                maxbits &= BIT_MASK;
                maxmaxcode = 1 << maxbits;
                fsize = 100000;     /* assume stdin large for USERMEM */
                if( maxbits > BITS ) {
                    fprintf( stderr,
                        "stdin: compressed with %d bits, can only handle %d bits\n",
                        maxbits, BITS );
                    return( 1 );
                }
            }
            decompress();
        }
    }
    return( exit_stat );
}
