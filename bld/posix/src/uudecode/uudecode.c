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
* Description:  POSIX uudecode utility
*               Decodes uuencoded files - converts from 7-bit to 8-bit chars
*
****************************************************************************/


/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

/*
 * Modified 12 April 1990 by Mark Adler for use on MSDOS systems with
 * Microsoft C and Turbo C.
 *
 * Modifed 13 February 1991 by Greg Roelofs for use on VMS systems.  As
 * with the MS-DOS version, the setting of the file mode has been disabled.
 * Compile and link normally (but note that the shared-image link option
 * produces a binary only 6 blocks long, as opposed to the 137-block one
 * produced by an ordinary link).  To set up the VMS symbol to run the
 * program ("run uudecode filename" won't work), do:
 *              uudecode :== "$disk:[directory]uudecode.exe"
 * and don't forget the leading "$" or it still won't work.  The binaries
 * produced by this program are in VMS "stream-LF" format; this makes no
 * difference to VMS when running decoded executables, nor to VMS unzip,
 * but other programs such as zoo or arc may or may not require the file
 * to be "BILFed" (or "unBILFed" or whatever).  Also, unlike the other
 * flavors, VMS files don't get overwritten (a higher version is created).
 *
 * Modified 13 April 1991 by Gary Mussar to be forgiving of systems that
 * appear to be stripping trailing blanks.
 *
 * Modifed 31 August 1992 by David S. McKee for use with the WATCOM compiler.
 *
 * Modifed 7 July 1993 by David S. McKee to handle more variants of the
 *         input file.
 */

#ifdef __WATCOMC__
#define __MSDOS__
#endif
#ifdef __MSDOS__        /* For Turbo C */
#ifndef MSDOS
#define MSDOS 1
#endif
#endif

/*
 * uudecode [input]
 *
 * create the specified file, decoding as you go.
 * used with uuencode.
 */
#include <stdio.h>

#ifdef VMS
#  include <types.h>
#  include <stat.h>
#else
#  ifndef MSDOS            /* i.e., UNIX */
#    include <pwd.h>
#  endif
#  ifdef __WATCOMC__
#  include <stdlib.h>
#  include <string.h>
#  endif
#  include <sys/types.h>   /* MSDOS or UNIX */
#  include <sys/stat.h>
#endif

/* single-character decode */
#define DEC(c)  (((c) - ' ') & 077)

/*Forward declarations */
void outdec( char *p, FILE *f, int n );
void decode( FILE *in, FILE *out );

int main(int argc, char **argv)
{
    FILE *in, *out;
    int mode;
    int begin_flag;
    char dest[128];
    char buf[80];

#ifdef __WATCOMC__
    if( argc == 1 || !strcmp(argv[1], "?") || !strcmp(argv[1], "-?")
         || !strcmp(argv[1], "-h") ) {
        printf( "Usage: uudecode [infile]\n" );
        exit(2);
    }
#endif
    /* optional input arg */
    if (argc > 1) {
        if ((in = fopen(argv[1], "r")) == NULL) {
            perror(argv[1]);
            exit(1);
        }
        argv++;
        argc--;
    } else {
        in = stdin;
    }

    if (argc != 1) {
        printf("Usage: uudecode [infile]\n");
        exit(2);
    }

    for(;;) {
        begin_flag = 1;
        /* search for header line */
        for(;;) {
            if (fgets(buf, sizeof buf, in) == NULL) {
                if( begin_flag == 0 ) {
                    fprintf(stderr, "No begin line\n");
                    exit(3);
                }
                begin_flag = 0;
                break;
            }
            if( strnicmp( buf, "begin-", 6 ) != 0
                    && strnicmp( buf, "begin -", 7 ) != 0
                    && strnicmp( buf, "start-", 6 ) != 0
                    && strnicmp( buf, "start -", 7 ) != 0 ) {
                if (strncmp(buf, "begin ", 6) == 0)
                    break;
                if (strnicmp(buf, "begin ", 6) == 0) {
                    fprintf( stderr, "begin line is mixed case, assuming lower case\n" );
                    break;
                }
            }
        }
        if( begin_flag == 0 )
            break;
        (void)sscanf(buf+6, "%o %s", &mode, dest);

#if !defined(MSDOS) && !defined(VMS)    /* i.e., UNIX */
        /* handle ~user/file format */
        if (dest[0] == '~') {
            char *sl;
            struct passwd *getpwnam();
            struct passwd *user;
            char dnbuf[100], *index(), *strcat(), *strcpy();

            sl = index(dest, '/');
            if (sl == NULL) {
                fprintf(stderr, "Illegal ~user\n");
                exit(3);
            }
            *sl++ = 0;
            user = getpwnam(dest+1);
            if (user == NULL) {
                fprintf(stderr, "No such user as %s\n", dest);
                exit(4);
            }
            strcpy(dnbuf, user->pw_dir);
            strcat(dnbuf, "/");
            strcat(dnbuf, sl);
            strcpy(dest, dnbuf);
        }
#endif  /* !defined(MSDOS) && !defined(VMS) */

        /* create output file */
#ifdef MSDOS
        out = fopen(dest, "wb");        /* Binary file */
#else
        out = fopen(dest, "w");
#endif
        if (out == NULL) {
            perror(dest);
            exit(4);
        }
#if !defined(MSDOS) && !defined(VMS)    /* i.e., UNIX */
        chmod(dest, mode);
#endif

        decode(in, out);

        if( fgets(buf, sizeof buf, in) == NULL || strcmp(buf, "end\n") ) {
            fprintf(stderr, "No end line\n");
            exit(5);
        }
    }
    return( 0 );
}

/*
 * copy from in to out, decoding as you go along.
 */
void decode( FILE *in, FILE *out ) {
    char buf[80];
    char *bp;
    int n, i, expected;
    int found_begin;
    int after_blank;
    int end_cut;

    end_cut = 0;
    for(;;) {
        /* for each input line */
        if (fgets(buf, sizeof buf, in) == NULL) {
            printf("Short file\n");
            exit(10);
        }
        bp = buf;
        while( *bp == ' ' ) {
            bp++;
        }
        if( !end_cut && ( *bp=='\n' || (buf[0]=='-' && buf[1]=='-')) ) {
            after_blank = 0;
            for(;;) {
                if( after_blank && buf[0] == 'M' )
                    break;
                if( buf[0] == ' ' || buf[0] == '\n' ) {
                    after_blank = 1;
                } else {
                    after_blank = 0;
                }
                if( fgets(buf, sizeof buf, in) == NULL) {
                    printf("Short file\n");
                    exit(10);
                }
            }
        }

        n = DEC(buf[0]);
        if( (n <= 0) || (buf[0] == '\n') )
            break;

        /* Calculate expected # of chars and pad if necessary */
        expected = ((n+2)/3)<<2;

        found_begin = 0;
        if( expected != strlen(buf)-1
            && ( strnicmp( buf, "end-", 4 ) == 0
                    || strnicmp( buf, "end -", 5 ) == 0
                    || strnicmp( buf, "--- end", 7 ) == 0) ) {
            end_cut = 1;
            for(;;) {
                if( strnicmp( buf, "--- begin", 9 ) == 0
                    || strnicmp( buf, "begin-", 6 ) == 0
                    || strnicmp( buf, "begin -", 7 ) == 0
                    || strnicmp( buf, "start-", 6 ) == 0
                    || strnicmp( buf, "start -", 7 ) == 0 ) {
                    found_begin = 1;
                    break;
                }
                if( fgets(buf, sizeof buf, in) == NULL ) {
                    printf("Short file\n");
                    exit(10);
                }
            }
        }
        if( !found_begin ) {
            for( i = strlen(buf)-1; i <= expected; i++ ) {
                buf[i] = ' ';
            }
            bp = &buf[1];
            while( n > 0 ) {
                outdec(bp, out, n);
                bp += 4;
                n -= 3;
            }
        }
    }
}

/*
 * output a group of 3 bytes (4 input characters).
 * the input chars are pointed to by p, they are to
 * be output to file f.  n is used to tell us not to
 * output all of them at the end of the file.
 */
void outdec( char *p, FILE *f, int n ) {
    int c1, c2, c3;

    c1 = DEC(*p) << 2 | DEC(p[1]) >> 4;
    c2 = DEC(p[1]) << 4 | DEC(p[2]) >> 2;
    c3 = DEC(p[2]) << 6 | DEC(p[3]);
    if (n >= 1)
        putc(c1, f);
    if (n >= 2)
        putc(c2, f);
    if (n >= 3)
        putc(c3, f);
}

/*
 * Return the ptr in sp at which the character c appears;
 * NULL if not found
 */

#ifndef __WATCOMC__
#define NULL    0
#endif

char* index( char *sp, int c ) {
    do {
        if (*sp == c)
            return(sp);
    } while (*sp++);
    return(NULL);
}
