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


/*@H************************ < COMPRESS utility> ****************************
*                                                                           *
*   compress : compress.c                                                   *
*              Main and Operating System Independent support functions      *
*                                                                           *
*   port by  : Donald J. Gloistein                                          *
*                                                                           *
*   Source, Documentation, Object Code:                                     *
*   released to Public Domain. This code is ported from compress v4.0       *
*   release joe.                                                            *
*---------------------------  Module Description  --------------------------*
*   The compress program is compatible with the compression/decompression   *
*   used on the Unix systems compress programs.  This is version 4 and      *
*   supports up to 16 bits compression. The porting retained the Unix       *
*   meanings of all options, added a couple for MsDos and modified the      *
*   file name conventions to make more sense.                               *
*                                                                           *
*--------------------------- Implementation Notes --------------------------*
*                                                                           *
*   compiled with : compress.h compress.fns                                 *
*   linked with   : compapi.obj  compusi.obj                                *
*   problems:                                                               *
*              See notes in compress.h for defines needed.                  *
*              It should work now with Xenix                                *
*                                                                           *
*              Check the signal() handler functions in your compiler        *
*              documentation. This code assumes ANSI SYS V compatible       *
*              header and return values. Change as appropriate for your     *
*              compiler and operating system.                               *
*                                                                           *
*              This source compiles properly with Microsoft C compiler      *
*              version 5.1.                                                 *
*                                                                           *
*   CAUTION:   because the program is in modules, make sure you recompile   *
*              all modules if you change the header or a define in the      *
*              compress.c file                                              *
*                                                                           *
* Algorithm from "A Technique for High Performance Data Compression",       *
* Terry A. Welch, IEEE Computer Vol 17, No 6 (June 1984), pp 8-19.          *
*                                                                           *
* Assumptions:                                                              *
*  When filenames are given, replaces with the compressed version           *
*  (.Z suffix) only if the file decreases in size.                          *
* Algorithm:                                                                *
*  Modified Lempel-Ziv method (LZW).  Basically finds common                *
* substrings and replaces them with a variable size code.  This is          *
* deterministic, and can be done on the fly.  Thus, the decompression       *
* procedure needs no input table, but tracks the way the table was built.   *
*                                                                           *
*                                                                           *
*---------------------------      Author(s)        -------------------------*
*     Initials ---- Name ---------------------------------                  *
*      DjG          Donald J. Gloistein                                     *
*                   Plus many others, see rev.hst file for full list        *
*      LvR          Lyle V. Rains, many thanks for improved implementation  *
*************************************************************************@H*/

/*@R************************< Revision History >*****************************
*                                                                           *
*   version -- date -- init ---Notes----------------------                  *
*    4.01    08-29-88  DjG    first cut  for 16 bit MsDos version           *
*            09-04-88  DjG    fixed unlink on zcat if interupted.           *
*                             added msdos filename logic and functions      *
*    4.10    10-27-88  DjG  revised API with coding changes by LvR.         *
*    4.10a   10-30-88  DjG  cleaned up code and fixed bug in freeing ptr.   *
*    4.10b   11-01-88  DjG  cleaned up the logic for inpath/outpath         *
*                           Changed the logic to finding the file name      *
*                           Fixed the allocation bug in the api             *
*                           Added some more portability macros              *
*    4.10c   11-04-88  DjG  Changed maxcode from global to static in api.   *
*                           Supplied some library functions for those who   *
*                           don't have them, changed dos usi to use the     *
*                           strrpbrk(). Checked casts in api again. Compiles*
*                           without warnings at pick level 3.               *
*    4.10d   11-25-88  DjG  revised some memory allocation, put more in the *
*                           header file. Corrected some typos.              *
*                           Changed prog_name() to force lower case         *
*                           Corrected bug, no longer unlinks existing file  *
*                           if not enough memory to compress or decompress  *
*            12-06-88  DjG  VERY minor changes for casts and header defines *
*            12-08-88  DjG  Adjusted path separator check in main function  *
*                           Amiga uses split seg because of compiler        *
*            12-09-88  DjG  Debugging done, all defaults now Unix compress  *
*                           defaults, including unlinking input file and    *
*                           acting as a filter. Must use -h option to get   *
*                           help screen.                                    *
*    4.10e   12-11-88  DjG  Fixed more casts, prototypes and header file.   *
*    4.10f   12-12-88  DjG  Fixed unlinking open files on error. This fails *
*                           on shared or os/2 platforms.                    *
*            12-15-88  DjG  Fixed SIGTYPE for function passed to signal     *
*                           Fixed problems with Xenix 2.2.1                 *
*    4.2     12-19-88  DjG  Replaced adaptive reset as an option.           *
*    4.3     12-26-88  DjG  Fixed long file name bug, fixed bug with        *
*                           compressdir. -B option added, same as -b option *
*            05-06-89  Dal  Ported to Sozobon/Alcyon C for Atari ST.  Also, *
*                           created get_one() for console prompting.        *
*            05-08-89  Dal  Ported to Minix-ST                              *
*************************************************************************@R*/

#include <stdio.h>

#define MAIN        /* header has defining instances of globals */
#include "compress.h" /* contains the rest of the include file declarations */

#define ARGVAL() (*++(*argv) || (--argc && *++argv))
char suffix[] = SUFFIX ;          /* only used in this file */

void main( argc, argv )
register int argc; char **argv;
{
    char **filelist, **fileptr,*temp;
    struct stat statbuf;

#ifndef NOSIGNAL
    if ( (bgnd_flag = signal ( SIGINT, SIG_IGN )) != SIG_IGN ) {
        /* ANSI/SYS V compatible */
        /* the following test checks for error on setting signals */
        /* check your documentation on the value to test          */
        /* if your signal.h doesn't support the return, it is     */
        /* essentially a no-op test                               */
        if (bgnd_flag == SIG_ERR){
            exit_stat = SIGNAL_ERROR;
            check_error();
        }
        if( (signal(SIGINT,onintr) == SIG_ERR)
        || (signal(SIGSEGV,oops) == SIG_ERR)) {/* check your compiler docs. */
            exit_stat = SIGNAL_ERROR;
            check_error();
        }
    }
#endif

    /* set up array for files to be converted */
#ifdef ALLOC
    filelist = fileptr = (char **)(alloc(argc * sizeof(char *)));
#else
    filelist = fileptr = (char **)(malloc(argc * sizeof(char *)));
#endif
    *filelist = NULL;

    /* gets name, compares and sets defaults */
    prog_name = get_program_name(argv[0]);

    /* now parse command line and get file list */
    for (argc--, argv++; argc > 0; argc--, argv++) {
        if (**argv == '-') {        /* A flag argument */
            while (*++(*argv)) {    /* Process all flags in this arg */
                switch (**argv) {
#if !defined(NDEBUG)
                    case 'D':
                        debug = TRUE;
                        keep_error = TRUE;
                        break;
                    case 'V':
                        verbose = TRUE;
                        version();
                        break;
#else
                    case 'V':
                        version();
                        break;
#endif /*!NDEBUG */
                    case 'v':
                        quiet = !quiet;
                        break;
                    case 'd':
                        do_decomp = TRUE;
                        break;
                    case 'f':
                        force = overwrite = TRUE;
                        break;
                    case 'n':
                        nomagic = TRUE;
                        break;
                    case 'C':
                        block_compress = FALSE;
                        break;
                    case 'b': case 'B':
                        if (!ARGVAL()) {
                            fprintf(stderr, "Missing maxbits\n");
                            Usage(1);
                            exit(ERROR);
                        }
                        maxbits = atoi(*argv);
                        goto nextarg;
                    case 'I':
                        if (!ARGVAL()) {
                            fprintf(stderr, "Missing in_path name\n");
                            Usage(1);
                            exit(ERROR);
                        }
                        strcpy(inpath,*argv);
                        temp = &inpath[strlen(inpath)-1];
#ifdef MSDOS
                        if (*temp != '\\' && *temp != '/')
#else
                        if (*temp != separator[0])
#endif
                            strcat(inpath,separator);
                        goto nextarg;
                    case 'O':
                        if (!ARGVAL()){
                            fprintf(stderr, "Missing out_path name\n");
                            Usage(1);
                            exit(ERROR);
                        }
                        strcpy(outpath,*argv);
                        temp = &outpath[strlen(outpath)-1];
#ifdef MSDOS
                        if (*temp != '\\' && *temp != '/')
#else
                        if (*temp != separator[0])
#endif
                            strcat(outpath,separator);
                        goto nextarg;
                    case 'c':
                        keep = zcat_flg = TRUE;
                        break;
                    case 'K':
                        keep_error = TRUE;
                        break;
                    case 'k':
                        keep = !keep;
                        break;
                    case '?':case 'h':case 'H':
                        Usage(0);
                        exit(NORMAL);
                        break;
                    case 'q':
                        quiet = TRUE;
                        break;
                    default:
                        fprintf(stderr, "%s : Unknown flag: '%c'\n",prog_name, **argv);
                        Usage(1);
                        exit(ERROR);
                } /* end switch */
            } /* end while processing this argument */
        }  /* end if option parameter */
        else {                                  /* must be input file name */
            *fileptr++ = *argv;                 /* Build input file list */
            *fileptr = NULL;
        } /* end else */
nextarg:        continue;                          /* process nextarg */
    } /* end command line processing */

    /* adjust for possible errors or conflicts */
    if(maxbits < MINBITS || maxbits > MAXBITS){
        fprintf(stderr,"\n%s: illegal bit value, range = %d to %d\n",prog_name,MINBITS,MAXBITS);
        exit(NORMAL);
    }
    if (zcat_flg && *outpath)         /* can't have an out path and zcat */
        *outpath = '\0';

    /* to make the error messages make sense */
    strcpy(ifname,"stdin");
    strcpy(ofname,"stdout");

    if (*filelist) {         /* Check if there are files specified */
                             /* *fileptr must continue to specify  */
                             /* command line in/out file name      */
        is_list = TRUE;
        for (fileptr = filelist; *fileptr; fileptr++) {
            exit_stat = 0;
            endchar[0] = '\0';
            if (do_decomp) {                /* DECOMPRESSION          */
                if (*inpath){               /* adjust for inpath name */
                    strcpy(ifname,inpath);  /* and copy into ifname   */
                    strcat(ifname,name_index(*fileptr));
                }
                else
                    strcpy(ifname,*fileptr);
                if(!is_z_name(ifname))         /* Check for .Z suffix    */
                    if(!(make_z_name(ifname))) /* No .Z: tack one on     */
                        continue;
                                               /* Open input file        */
                if ((freopen(ifname, READ_FILE_TYPE, stdin)) == NULL) {
                    perror(ifname);
                    continue;
                }
                else
                    setvbuf(stdin,zbuf,_IOFBF,ZBUFSIZE);
                if (!nomagic) {             /* Check the magic number */
                    if ((getchar() != (magic_header[0] & 0xFF))
                      || (getchar() != (magic_header[1] & 0xFF))) {
                        fprintf(stderr, "%s: not in compressed format\n",
                            *ifname);
                        continue;
                    }
                    maxbits = getchar();    /* set -b from file */
                    block_compress = maxbits & BLOCK_MASK;
                    maxbits &= BIT_MASK;
                    if(maxbits > MAXBITS) {
                        fprintf(stderr,
                        "%s: compressed with %d bits, can only handle %d bits\n",
                        ifname, maxbits, MAXBITS);
                        continue;
                    }
                }  /* end if nomagic */
                                             /* Generate output filename */
                if (*outpath){               /* adjust for outpath name */
                    strcpy(ofname,outpath);  /* and copy into ofname   */
                    strcat(ofname,name_index(ifname));
                }
                else
                    strcpy(ofname,ifname); /* DjG may screw up the placement */
                                           /* of the outfile */
                unmake_z_name(ofname);     /* strip off Z or .Z */
            }
            else {            /* COMPRESSION */
                if (*inpath){               /* adjust for inpath name */
                    strcpy(ifname,inpath);  /* and copy into ifname   */
                    strcat(ifname,name_index(*fileptr));
                }
                else
                    strcpy(ifname,*fileptr);
                if (is_z_name(ifname)) {
                    fprintf(stderr, "%s: already has %s suffix -- no change\n",
                        ifname,suffix);
                    continue;
                }
                /* Open input file */
                if ((freopen(ifname,READ_FILE_TYPE, stdin)) == NULL) {
                    perror(ifname);
                    continue;
                }
                else
                    setvbuf(stdin,xbuf,_IOFBF,XBUFSIZE);
                /* Generate output filename */
                if (*outpath){               /* adjust for outpath name */
                    strcpy(ofname,outpath);  /* and copy into ofname   */
                    strcat(ofname,name_index(ifname));
                }
                else  /* place it in directory of input file */
                    strcpy(ofname,ifname); /* DjG may screw up the placement */
                                           /* of the outfile */
                if (!(make_z_name(ofname)))
                    continue;
            } /* end else compression  we now have the files set up */

            /* Check for overwrite of existing file */
            if (!overwrite && !zcat_flg) {
                if (!stat(ofname, &statbuf)) {
                    char response, get_one();

                    response = 'n';
                    fprintf(stderr, "%s already exists;", ofname);
#ifndef NOSIGNAL
                    if (foreground()) {
#else
                    if (TRUE) {
#endif
                        fprintf(stderr, "\ndo you wish to overwrite %s (y or n)? ",
                        ofname);
                        fflush(stderr);
                        response = get_one();
                    }
                    if ((response != 'y') && (response != 'Y')) {
                        fprintf(stderr, "\tnot overwritten\n");
                        continue;
                    }
                } /* end if stat */
            } /* end if overwrite */
            /* Output file  is opened in compress/decompress routines */

            /* Actually do the compression/decompression  on files */
            if (!do_decomp){
                compress();
                check_error();
            }
            else{
                decompress();
                check_error();
            }
            if(!zcat_flg) {
                copystat(ifname, ofname); /* Copy stats */
                if((exit_stat ) || (!quiet))
                    putc('\n', stderr);
            }       /* end if zcat */
        }           /*end for  loop */
    }               /* end if filelist */
    else {          /* it is standard input to standard output*/
#if (FILTER == FALSE)     /* filter is defined as true or false */
    /* DjG added to make more sense.  The following tests for standard
       input being a character device. If so, there is no use in MsDos
       for the program, as that will compress from the keyboard to the
       console. Sure not what is needed. Instead, the usage function
       is called. In Xenix/Unix systems, there is a need for this type
       of pipe as the input may be from a char dev, remote station.
     */

        /* check if input is unredirected */
        if ( isatty(fileno(stdin)) ){
            Usage(1);
            exit(NORMAL);
        }
#endif
        /* filter */
        if (do_decomp){
            setvbuf(stdin,zbuf,_IOFBF,ZBUFSIZE);  /* make the buffers larger */
            setvbuf(stdout,xbuf,_IOFBF,XBUFSIZE);
        }
        else{
            setvbuf(stdin,xbuf,_IOFBF,XBUFSIZE);  /* make the buffers larger */
            setvbuf(stdout,zbuf,_IOFBF,ZBUFSIZE);
        }
        if (!do_decomp) {   /* compress stdin to stdout */
            compress();
            check_error();
            if(!quiet)
                putc('\n', stderr);
        } /* end compress stdio */
        else {   /* decompress stdin to stdout */
            /* Check the magic number */
            if (!nomagic) {
                if ((getchar()!=(magic_header[0] & 0xFF))
                 || (getchar()!=(magic_header[1] & 0xFF))) {
                    fprintf(stderr, "stdin: not in compressed format\n");
                    exit(ERROR);
                }
                maxbits = getchar();    /* set -b from file */
                block_compress = maxbits & BLOCK_MASK;
                maxbits &= BIT_MASK;
                if(maxbits > MAXBITS) {
                    fprintf(stderr,
                    "stdin: compressed with %d bits, can only handle %d bits\n",
                    maxbits, MAXBITS);
                    exit(ERROR);
                }
            }
            decompress();
            check_error();
        } /* end else decomp stdio */
    } /* end else standard input */
    exit(exit_stat);
}

void Usage(flag)
int flag;
{
static char *keep2 =  "keep";
static char *keep3 =  "kill (erase)";
static char *on = "on";
static char *off = "off";

#ifndef NDEBUG
    fprintf(stderr,"Usage: %s [-cCdDf?hkKvV][-b maxbits][-Iinpath][-Ooutpath][filenames...]\n",
        prog_name);
#else
    fprintf(stderr,"Usage: %s [-cCdf?hkKvV][-b maxbits][-Iinpath][-Ooutpath][filenames...]\n",
        prog_name);
#endif
    if (flag)
        return;
    fprintf(stderr,"Argument Processing..case is significant:\n");
    fprintf(stderr,"     MUST use '-' for switch character\nAll flags are optional.\n");
#ifndef NDEBUG
    fprintf(stderr,"     -D => debug; Keep file on error.\n");
    fprintf(stderr,"     -V => print Version; debug verbose\n");
#else
    fprintf(stderr,"     -V => print Version\n");
#endif
    fprintf(stderr,"     -d => do_decomp default = %s\n",(do_decomp)?on:off);
    fprintf(stderr,"     -v => verbose default = %s\n", (quiet)?off:on);
    fprintf(stderr,"     -f => force overwrite of output file default = %s\n",
        (force)?on:off);
    fprintf(stderr,"     -n => no header: useful to uncompress old files\n");
    fprintf(stderr,"     -c => cat all output to stdout default = %s\n",
        (zcat_flg)?on:off);
    fprintf(stderr,"     -C => generate output compatible with compress 2.0.\n");
    fprintf(stderr,"     -k => %s input file, default = %s\n",(keep)?keep3:keep2,
            (keep)?keep2:keep3);
    fprintf(stderr,"     -K => %s output file on error, default = %s\n",
            (keep_error)?keep3:keep2,(keep_error)?keep2:keep3);
    fprintf(stderr,"     -b maxbits  => default = %d bits, max = %d bits\n",maxbits,MAXBITS);
    fprintf(stderr,"     -I pathname => infile path  = %s\n",inpath);
    fprintf(stderr,"     -O pathname => outfile path = %s\n",outpath);
    fprintf(stderr,"     -? -h => help usage.\n");
}

char get_one()
/*
 * get a single character, with echo.
 */
{
        char tmp[2];
        int fd;

#ifdef SOZOBON
        return(0x7F & getche());
#endif
#ifdef MSC
        return(getche());
#endif
/*
 * All previous #ifdef'ed code should return() a value.
 * If no other option is available, the following is the original code.
 * It not only reads from stderr (not a defined operation)
 * but it does so via an explicit read() call on file descriptor 2!
 * So much for portability.                    -Dal
 */
#ifdef MINIX
        fd = open("/dev/tty", 0);       /* open the tty directly */
#else
        fd = 2;                         /* read from stderr */
#endif
        read(fd, tmp, 2);
        while (tmp[1] != '\n') {
                if (read(fd, tmp+1, 1) < 0) {   /* Ack! */
                        perror("stderr");
                        break;
                }
        }
        return(tmp[0]);
}

void writeerr()
{
    perror ( ofname );
    if (!zcat_flg && !keep_error){
        fclose(stdout);
        unlink ( ofname );
    }
    exit ( 1 );
}

#ifndef NOSIGNAL
/*
 * This routine returns 1 if we are running in the foreground and stderr
 * is a tty.
 */
int foreground()
{
    if(bgnd_flag) { /* background? */
        return(0);
    }
    else {            /* foreground */
        if(isatty(2)) {     /* and stderr is a tty */
            return(1);
        } else {
            return(0);
        }
    }
}
#endif

void prratio(stream, num, den)
FILE *stream;
long int num, den;
{
    register int q;         /* Doesn't need to be long */

    if(num > 214748L) {     /* 2147483647/10000 */
        q = (int) (num / (den / 10000L));
    }
    else {
        q = (int) (10000L * num / den);     /* Long calculations, though */
    }
    if (q < 0) {
        putc('-', stream);
        q = -q;
    }
    fprintf(stream, "%d.%02d%%", q / 100, q % 100);
}


int check_error()     /* returning OK continues with processing next file */
{
    switch(exit_stat) {
  case OK:
    return (OK);
  case NOMEM:
    if (do_decomp)
        fprintf(stderr,"%s: not enough memory to decompress '%s'.\n", prog_name, ifname);
    else
        fprintf(stderr,"%s: not enough memory to compress '%s'.\n", prog_name, ifname);
    return(OK);
  case SIGNAL_ERROR:
    fprintf(stderr,"%s: error setting signal interupt.\n",prog_name);
    exit(ERROR);
    break;
  case READERR:
    fprintf(stderr,"%s: read error on input '%s'.\n", prog_name, ifname);
    break;
  case WRITEERR:
    fprintf(stderr,"%s: write error on output '%s'.\n", prog_name, ofname);
    break;
   case TOKTOOBIG:
    fprintf(stderr,"%s: token too long in '%s'.\n", prog_name, ifname);
    break;
  case INFILEBAD:
    fprintf(stderr, "%s: '%s' in unknown compressed format.\n", prog_name, ifname);
    break;
 case CODEBAD:
    fprintf(stderr,"%s: file token bad in '%s'.\n", prog_name,ifname);
    break;
 case TABLEBAD:
    fprintf(stderr,"%s: internal error -- tables corrupted.\n", prog_name);
    break;
  case NOTOPENED:
    fprintf(stderr,"%s: could not open output file %s\n",prog_name,ofname);
    exit(ERROR);
    break;
  case NOSAVING:
    if (force)
        exit_stat = OK;
    return (OK);
  default:
    fprintf(stderr,"%s: internal error -- illegal return value = %d.\n", prog_name,exit_stat);
  }
  if (!zcat_flg && !keep_error){
        fclose(stdout);         /* won't get here without an error */
        unlink ( ofname );
    }
  exit(exit_stat);
#ifndef __WATCOMC__
  return(ERROR);
#endif
}
