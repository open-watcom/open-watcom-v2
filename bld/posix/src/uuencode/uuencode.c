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
 * Microsoft C and Turbo C.  Standard input problem fixed 29 April 1990
 * as per suggestion by Steve Harrold.
 *
 * Modifed 13 February 1991 by Greg Roelofs for use on VMS systems.
 * Compile and link normally (but note that the shared-image link option
 * produces a binary only 6 blocks long, as opposed to the 152-block one
 * produced by an ordinary link).  To set up the VMS symbol to run the
 * program ("run uuencode filename1 filename2 filename3" won't work), do:
 *              uuencode :== "$disk:[directory]uuencode.exe"
 * and don't forget the leading "$" or it still won't work.  The syntax
 * differs slightly from the Unix and MS-DOS versions since VMS has such
 * an awkward approach to redirection; run the program with no arguments
 * for the usage (or see USAGE below).  The output file is in VMS "stream-
 * LF" format but should be readable by MAIL, ftp, or anything else.
 *
 * Modifed 31 August 1992 by David S. McKee for use with the WATCOM compiler.
 * Enhanced the command line parsing so that people find it easier to use
 * while keeping the redirection and piping capability.
 */

#if !defined( lint ) && !defined( __WATCOMC__ )
static char sccsid[] = "@(#)uuencode.c  5.6 (Berkeley) 7/6/88";
#endif /* not lint */

#ifdef __WATCOMC__
#define __MSDOS__
#endif
#ifdef __MSDOS__
#ifndef MSDOS
#define MSDOS 1
#endif
#endif

/*
 * uuencode [input] output
 *
 * Encode a file so it can be mailed to a remote system.
 */
#include <stdio.h>

#ifdef VMS
#  define OUT out       /* force user to specify output file */
#  define NUM_ARGS 3
#  define USAGE "Usage: uuencode [infile] remotefile uufile\n"
#  include <types.h>
#  include <stat.h>
#else
#  define OUT stdout    /* Unix, MS-DOS:  anybody with decent redirection */
#  define NUM_ARGS 2
#  define USAGE "Usage: uuencode [infile] remotefile\n"
#ifdef __WATCOMC__
static char * usage_data[] = {
    "Usage: uuencode [-h|?] [infile] remotefile [outfile]\n\n",
    "          remotefile - the name of the file when the\n",
    "                       file is later uudecoded\n",
    "          infile     - when more than one parameter is specified,\n",
    "                       the first is the input file name\n",
    "          outfile    - when more than one parameter is specified,\n",
    "                       the third is the output file name\n\n",
    "       When indirection or piping is used for either input or output,\n",
    "       the respective command line parameters are ignored.  The default\n",
    "       output is a name constructed from the remotefile name.\n\n",
    "       Examples: \n",
    "         uuencode test.zip\n",
    "           infile=test.zip, remotefile=test.zip, outfile=test.uu\n",
    "         uuencode test.zip test.dat\n"
    "           infile=test.zip, remotefile=test.dat, outfile=test.uu\n",
    "         uuencode test.out test.dat <test.zip\n",
    "           infile=test.zip, remotefile=test.out, outfile=test.dat\n",
    NULL
};
#  undef OUT
#  define OUT out       /* force user to specify output file */
#  include <stdlib.h>
#endif
#  include <sys/types.h>
#  include <sys/stat.h>
#endif

#ifdef MSDOS
#include <unistd.h>
#include <fcntl.h>
#endif

/* ENC is the basic 1-character encoding function to make a char printing */
#define ENC(c) ((c) ? ((c) & 077) + ' ': '`')

main(argc, argv)
char **argv;
{
#if defined( VMS ) || defined( __WATCOMC__ )
        FILE        *out;
#endif
        FILE        *in;
        int     mode;
        int     cnt;
        char *  file_name;
        char    fname[_MAX_FNAME];
        char    full_name[_MAX_PATH];
        struct stat sbuf;

#ifdef __WATCOMC__
        argc--;
        if( argc > 3 || argc == 0 || argv[1] == '?'
         || argv[1] == '-?' || argv[1] == '-h' ) {
            cnt = 0;
            for(;;) {
                if( usage_data[cnt] == NULL ) break;
                fprintf(stderr, usage_data[cnt++] );
            }
            exit(2);
        }

        if( !isatty(fileno(stdin)) ) {
            in = stdin;
            if( argc == 3 ) {
                argv++;
                argc--;
            }
        } else {
            in = fopen( argv[1], "r" );
            if( in == NULL ) {
                perror(argv[1]);
                exit(1);
            }
            if( argc > 1 ) {
                argv++;
                argc--;
            }
        }

        if( !isatty(fileno(stdout)) ) {
            out = stdout;
        } else {
            if( argc == 2 ) {
                file_name = argv[2];
            } else {
                _splitpath( argv[1], NULL, NULL, fname, NULL );
                _makepath( full_name, NULL, NULL, fname, "uu" );
                file_name = full_name;
            }
            out = fopen( file_name, "w" );
            if( out == NULL ) {
                perror(file_name);
                exit(4);
            }
        }
#else
        /* optional 1st argument */
        if (argc > NUM_ARGS) {
                if ((in = fopen(argv[1], "r")) == NULL) {
                        perror(argv[1]);
                        exit(1);
                }
                argv++; argc--;
        } else {
                in = stdin;
        }
#endif

#ifdef MSDOS
        /* set input file mode to binary for MSDOS systems */
        setmode(fileno(in), O_BINARY);
#endif

#ifndef __WATCOMC__
        if (argc != NUM_ARGS) {
                fprintf(stderr, USAGE);
                exit(2);
        }
#endif

#ifdef VMS   /* mandatory 3rd argument is name of uuencoded file */
        if ((out = fopen(argv[2], "w")) == NULL) {
                perror(argv[2]);
                exit(4);
        }
#endif

        /* figure out the input file mode */
        if (fstat(fileno(in), &sbuf) < 0 || !isatty(fileno(in)))
                mode = 0666 & ~umask(0666);
        else
                mode = sbuf.st_mode & 0777;
        fprintf(OUT, "begin %o %s\n", mode, argv[1]);

        encode(in, OUT);

        fprintf(OUT, "end\n");
        exit(0);
}

/*
 * copy from in to out, encoding as you go along.
 */
encode(in, out)
register FILE *in;
register FILE *out;
{
        char buf[80];
        register int i, n;

        for (;;) {
                /* 1 (up to) 45 character line */
                n = fread(buf, 1, 45, in);
                putc(ENC(n), out);

                for (i=0; i<n; i += 3)
                        outdec(&buf[i], out);

                putc('\n', out);
                if (n <= 0)
                        break;
        }
}

/*
 * output one group of 3 bytes, pointed at by p, on file f.
 */
outdec(p, f)
register char *p;
register FILE *f;
{
        register int c1, c2, c3, c4;

        c1 = *p >> 2;
        c2 = (*p << 4) & 060 | (p[1] >> 4) & 017;
        c3 = (p[1] << 2) & 074 | (p[2] >> 6) & 03;
        c4 = p[2] & 077;
        putc(ENC(c1), f);
        putc(ENC(c2), f);
        putc(ENC(c3), f);
        putc(ENC(c4), f);
}
