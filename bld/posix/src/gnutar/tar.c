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
* Description:  A public domain tar(1) program.
*
****************************************************************************/


/*
 * A public domain tar(1) program.
 *
 * Written by John Gilmore, ihnp4!hoptoad!gnu, starting 25 Aug 85.
 * MS-DOS port 2/87 by Eric Roskos.
 * Minix  port 3/88 by Eric Roskos.
 *
 * @(#)tar.c 1.21 10/29/86 Public Domain - gnu
 */

#include <stdio.h>
#include <sys/types.h>                  /* Needed for typedefs in tar.h */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef MSDOS
#include <conio.h>
#include <fcntl.h>
#endif
#ifdef V7
FILE *fopen();
char *fgets();
#endif

/*
 * The following causes "tar.h" to produce definitions of all the
 * global variables, rather than just "extern" declarations of them.
 */
#define TAR_EXTERN                              /**/
#include "tar.h"
#include "port.h"
#include "list.h"
#include "create.h"
#include "extract.h"
#include "buffer.h"
#include "getopt.h"

/*
 * We should use a conversion routine that does reasonable error
 * checking -- atoi doesn't.  For now, punt.  FIXME.
 */
#define intconv atoi

static FILE    *namef;                  /* File to read names from */
static char   **n_argv;                 /* Argv used by name routines */
static int      n_argc;                 /* Argc used by name routines */

 /* They also use "optind" from getopt(). */

#ifdef MSDOS
/*
 * see convmode, below.  This list is the list of files that should be
 * opened with mode O_BINARY to prevent CR/LF conversions while they
 * are being read in.  FIXME: it is my intent to eventually add an
 * option to the command line that lets you add arbitrarily many new
 * extensions to this list, so people won't have problems with the
 * list being inadequate for them.  I wish there was an easier way, but
 * this one is fairly consistent if you think about it.
 */
#define NBINEXTS        20

static char    *binexts[NBINEXTS] =             /* extensions for O_BINARY files */
{
        "com",
        "exe",
        "obj",
        0                                                       /* required */
};

#endif

static void name_init( int argc, char **argv );
static void addbinext( char *s );

/* Forward declarations */
void options( int argc, char **argv );
void describe( void );
static void addname( char *name );



/*
 * Main routine for tar.
 */
int main( int argc, char **argv )
{

        /*
         * Uncomment this message in particularly buggy versions...
         * fprintf(stderr, "tar: You are running an experimental PD tar, maybe
         * use /bin/tar.\n");
         */

        tar = "tar";                            /* Set program name */
#ifdef MSDOS
        physdrv = 0;                            /* set default drive */
        devsize = 720;                          /* default drive size */
        ftty = open("CON", O_RDWR);             /* open console */
#else /* !MSDOS */
        ftty = open("/dev/tty", 2);
#endif /* !MSDOS */
        if (ftty < 0) {
                fprintf(stderr, "Can't open %s for I/O\n",
#ifdef MSDOS
                "console"
#else
                "/dev/tty"
#endif
                );
                exit(EX_SYSTEM);
        }

        options(argc, argv);

        name_init(argc, argv);

#if defined(MSDOS) && !defined(__NO_PHYS__)
        if (f_phys) {
                uprintf(ftty,"tar: archive on %dK drive %c\n",
                        devsize/2, 'A' +  physdrv);
                uprintf(ftty,"tar: insert %s disk in drive '%c' and press [Enter]: ",
                        f_create? "formatted" : "first",
                        'A' + physdrv);
                while (ugetc(ftty)!='\n') ;
        }
#endif

        if (f_create) {
                if (f_extract || f_list)
                        goto dupflags;
                create_archive();
        } else if (f_extract) {
                if (f_list)
                        goto dupflags;
                read_and(extract_archive);
        } else if (f_list) {
                read_and(list_archive);
        } else {
dupflags:
                fprintf(stderr,
                        "tar: you must specify exactly one of the c, t, or x options\n");
                describe();
                exit(EX_ARGSBAD);
        }
        putchar('\n');
        fflush(stdout);
#ifndef MSDOS
        sync(); /* insure all floppy buffers are written out */
#endif
        return( 0 );
}


/*
 * Parse the options for tar.
 */
void options( int argc, char **argv )
{
        int    c;                      /* Option letter */

        /* Set default option values */
        blocking = DEFBLOCKING;         /* From Makefile */
        ar_file = DEF_AR_FILE;          /* From Makefile */

        /* Parse options */
        while ((c = getoldopt(argc, argv, "b:BcdDf:hikmopsS:tT:u:vV:xzZ")
                ) != EOF)
        {
                switch (c)
                {

                case 'b':
                        blocking = intconv(optarg);
                        break;

                case 'B':
                        f_reblock++;            /* For reading 4.2BSD pipes */
                        break;

                case 'c':
                        f_create++;
                        break;

                case 'd':
                        f_debug++;                      /* Debugging code */
                        break;                          /* Yes, even with dbx */

                case 'D':
                        f_sayblock++;           /* Print block #s for debug */
                        break;                          /* of bad tar archives */

                case 'f':
                        ar_file = optarg;
                        break;

                case 'h':
                        f_follow_links++;       /* follow symbolic links */
                        break;

                case 'i':
                        f_ignorez++;            /* Ignore zero records (eofs) */

                        /*
                         * This can't be the default, because Unix tar writes two records
                         * of zeros, then pads out the block with garbage.
                         */
                        break;

                case 'k':                               /* Don't overwrite files */
                        f_keep++;
                        break;

                case 'm':
                        f_modified++;
                        break;

                case 'o':                               /* Generate old archive */
                        f_oldarch++;
                        break;

                case 'p':
                        f_use_protection++;
                        (void) umask(0);        /* Turn off kernel "help" */
                        break;

                case 's':
                        f_sorted_names++;       /* Names to extr are sorted */
                        break;
#ifdef MSDOS
                case 'S':
                        devsize = atoi(optarg); /* size of DOS disk drive */
                        devsize <<= 1;          /* convert K to blocks */
                        break;
#endif
                case 't':
                        f_list++;
                        break;

                case 'T':
                        name_file = optarg;
                        f_namefile++;
                        break;
#ifdef MSDOS
                case 'u':
                        addbinext(optarg);
                        break;
#endif
                case 'v':
                        f_verbose++;
                        break;
#if defined(MSDOS) && !defined(__NO_PHYS__)
                case 'V':
                        f_phys++;
                        physdrv = toupper(*optarg) - 'A';
                        if (physdrv > 4 || physdrv < 0)
                        {
                                fprintf(stderr, "tar: drive letter for -V must be A-D\n");
                                exit(EX_ARGSBAD);
                        }
                        break;
#endif /* MSDOS */

                case 'x':
                        f_extract++;
                        break;

                case 'z':                               /* Easy to type */
                case 'Z':                               /* Like the filename extension .Z */
#ifndef MSDOS
                        f_compress++;
#else
                        fprintf(stderr, "Running compress as a subprocess is not supported under DOS.\n");
                        fprintf(stderr, "Run compress separately instead, for same effect.\n");
#endif
                        break;

                default:
                case '?':
                        describe();
                        exit(EX_ARGSBAD);

                }
        }

        blocksize = blocking * RECORDSIZE;
}


/* FIXME, describe tar options here */
void describe( void )
{

        fputs("tar: valid options:\n\
-b N    blocking factor N (block size = Nx512 bytes)\n\
-B      reblock as we read (for reading 4.2BSD pipes)\n\
-c      create an archive\n\
-D      dump record number within archive with each message\n\
-f F    read/write archive from file or device F\n", stderr);
        fputs("-h       don't dump symbolic links; dump the files they point to\n\
-i      ignore blocks of zeros in the archive, which normally mean EOF\n\
-k      keep existing files, don't overwrite them from the archive\n\
-m      don't extract file modified time\n\
-o      write an old V7 format archive, rather than ANSI [draft 6] format\n\
-p      do extract all protection information\n", stderr);
#ifdef MSDOS
        fputs("-S X     device for -V option is X Kbyte drive\n", stderr);
#endif
        fputs("-s       list of names to extract is sorted to match the archive\n\
-t      list a table of contents of an archive\n\
-T F    get names to extract or create from file F\n", stderr);
#ifdef MSDOS
        fputs("\
-u X    add X to list of file extensions to be opened in BINARY mode\n\
        (use '.' to denote 'files with no extension')\n\
-V X    use drive X (X=A..D) in multivolume mode; ignore -f if present\n",
                stderr);
#endif
        fputs("\
-v      verbosely list what files we process\n\
-x      extract files from an archive\n", stderr);
#ifndef MSDOS

        /*
         * regrettably, DOS doesn't have real pipes, just artificial shell-level
         * ones.  It is better to just use those.
         */
        fputs("\
-z or Z run the archive through compress(1)\n", stderr);
#endif
}


/*
 * Set up to gather file names for tar.
 *
 * They can either come from stdin or from argv.
 */
static void name_init( int argc, char **argv )
{
    if (f_namefile) {
        if (optind < argc) {
            fprintf(stderr, "tar: too many args with -T option\n");
            exit(EX_ARGSBAD);
        }
        if (!strcmp(name_file, "-")) {
            namef = stdin;
        } else {
            namef = fopen(name_file, "r");
            if (namef == NULL) {
                fprintf(stderr, "tar: ");
                perror(name_file);
                exit(EX_BADFILE);
            }
        }
    } else {
        /* Get file names from argv, after options. */
        n_argc = argc;
        n_argv = argv;
    }
}

/*
 * Name translation function for MS-DOS support; can also be
 * extended via #ifdef for other os's.
 *
 * Convert a name to one suitable to this OS.  If this can't be done
 * automatically, let the user choose a name.
 *
 * The user prompting is done only if stdin isatty.
 *
 * It is important to understand the name translation, which occurs
 * in two steps.  First, the actual name string passed in s is modified
 * in place to change case and direction of slashes, because DOS's
 * directory routines return uppercase names with backslashes, which
 * are not suitable for Unix.  This changes the string into a unix-like
 * filename.  Second, this string is copied into a local buffer and
 * transformed, if necessary, into a filename that is syntactically
 * acceptable to DOS.  The routine returns a pointer to this string.
 * The former step fixes names that come from DOS to be Unix-compatible;
 * it will never change Unix filenames, except to make uppercase letters
 * lowercase, because they are already Unix-compatible.  The latter step
 * fixes names that come from Unix to be DOS-compatible; it will never
 * change DOS filenames, because they are already DOS-compatible.
 *
 * The translation of uppercase letters to lowercase ones in unix filenames
 * that appeared in a tar file is a side-effect of the dual purpose of
 * fixname; its only effect is that listings of filenames in a tar file
 * will always be lowercase.  An improvement might be to separate fixname
 * into one routine to fix DOS names, and another to fix Unix names,
 * but this is left for a future enhancement.
 *
 * Even in non-DOS environments, fixname() must at least return a pointer
 * to a *copy* of the original filename, even if it is an unmodified
 * copy.  This is because this name string is used by tar at times after
 * the string which was pointed to by 's' has been overwritten by other
 * data.  The present code does this properly.
 *
 * FIXME: This code is embarassingly complex and needs to be rewritten.
 */

char* fixname( char *s )
{
        char  *q;
        char  *prd;
        char  *lsl;
        int   name_cnt;
        static char     buf[256];       /* where the copy of the name is stored */

#ifdef MSDOS

        /*
         * CODE TO FIX DOS NAMES: DOS's filenames are always uppercase, though
         * DOS maps lowercase characters to uppercase in filenames automatically.
         * If we create the archive with these uppercase names, the files if
         * un-tarred under Unix all have uppercase names. So we map the names to
         * lowercase under DOS so that it works best for both.  The same for \ vs
         * /: DOS takes either, Unix needs /, so we use /. This first
         * transformation occurs on the actual string we were passed, rather than
         * a copy of it.
         */
        strcpy(buf, s);
        q = buf;
        strlwr(q);
        for (; *q; q++)
                if (*q == '\\')
                        *q = '/';

        /*
         * CODE TO FIX UNIX NAMES: if more than one '.' in name, DOS won't create
         * file.  Delete all but last '.', then see if name longer than DOS
         * allows.  If so, prompt user for new name.  (It might be better to
         * always do the length check, but in this case it is more likely to be a
         * problem since names with multiple dots often have fellow files with
         * common left substrings which the part after the dot qualifies.)
         */
        --q;
        if( *q != '/' ) {
            lsl = rindex(buf, '/');
            if (lsl == NULL) {
                lsl = buf;
            } else {
                lsl++;
            }

            prd = 0;
            for (q = lsl; *q; q++)
            {
                    if( *q == '.' ) {
                        if( prd != 0 ) break;
                        prd = q;
                    }
            }
            if( prd == 0 ) {
                *q++ = '.';
            }
            *q = '\0';

            q = index(lsl, '.');
            if( q - lsl > 8 ) {
                uprintf( ftty, "tar: the file name of %s is too long\n", buf );
                strcpy( lsl+8, q );
                uprintf( ftty, "tar: truncating to %s\n", buf );
                q = index(lsl, '.');
            }
            if( strlen(q) > 4 ) {
                uprintf( ftty, "tar: the file extension of %s is too long\n", buf );
                q += 4;
                *q = '\0';
                uprintf( ftty, "tar: truncating to %s\n", buf );
            }


            /*
             * prompt user for valid name, & check file existence, only if doing
             * an "extract" operation with stdin not redirected.
             */
            if( f_extract && !access(buf, 0) ) {
                uprintf( ftty, "tar: %s already exists\n", buf );
                name_cnt = 3;
                if( prd == 0 ) {
                    strcat( buf, "002" );
                } else if( strlen( prd ) == 2 ) {
                    strcat( buf, "02" );
                } else if( strlen( prd ) == 3 ) {
                    strcat( buf, "2" );
                } else {
                    name_cnt = 2;
                }
                q = buf + strlen( buf ) - 1;
                while( !access(buf, 0) ) {
                    itoa( name_cnt, q, 10 );
                    if( name_cnt == 9 ) {
                        q--;
                    } else if( name_cnt == 99 ) {
                        q--;
                    }
                    name_cnt++;
                }
                uprintf( ftty, "tar: written as %s\n", buf );
            }
            q = buf + strlen(buf) - 1;
            if( *q == '.' ) {
                *q = '\0';
            }
        }
#endif

        return (buf);
}

/*
 * convmode(s) - return conversion mode bits for file with name s.
 *
 * This routine assumes filenames have the file type encoded in them
 * in a standard way (e.g., MS/DOS extensions).  It examines the
 * name of the file, and returns any mode bits that need to be or'ed
 * into the mode bits for the open (O_RDWR, etc. bits) for that particular
 * file to be read such that it looks like a normal Unix file.
 *
 * Obviously, your OS has to meet all the above implied constraints, but at
 * least this is a head start, I hope...
 */

int convmode( char * s )
{
        char          **p;

#ifdef MSDOS
        while (*s)
        {
                if (*s == '.')
                        break;
                s++;
        }

        if (*s == '\0')
                s = " .";       /* special string for "no extension" */
                                /* it has space in front because of  */
        s++;                    /* <- that increment */

        for (p = binexts; *p; p++)
        {
                if (strcmp(s, *p) == 0)
                {
                        return (O_BINARY);
                }
        }

        return (O_TEXT);
#else
        /* for a Unix-like OS, always return 0 */
        return (0);
#endif
}

#ifdef MSDOS
/*
 * add an extension to the "binexts" list of file extensions that
 * won't get O_BINARY translation (see convmode(), above)
 */

static void addbinext( char *s )
{
        char **exts;
        int             n;

        for (exts = binexts, n = 0; *exts; exts++, n++);        /* find end */

        if (n >= NBINEXTS - 1)
        {
                annofile(stderr, tar);
                fprintf(stderr, "%s: too many extensions added (max=%d)\n",
                        s, NBINEXTS - 1);
                exit(EX_ARGSBAD);
        }

        /* optional "." on front unless string is just "." */
        if (s[0] == '.' && s[1] != '\0')
                s++;

        *exts++ = s;
        *exts = 0;
}

#endif

/*
 * Get the next name from argv or the name file.
 *
 * Result is in static storage and can't be relied upon across two calls.
 */
char *name_next( void )
{
        static char     buffer[NAMSIZ + 2];     /* Holding pattern */
        char  *p;
        char  *q;

        if (namef == NULL)
        {
                /* Names come from argv, after options */
                if (optind < n_argc)
                {
                        return fixname(n_argv[optind++]);
                }
                return (char *) NULL;
        }
        p = fgets(buffer, NAMSIZ + 1 /* nl */ , namef);
        if (p == NULL)
                return p;                               /* End of file */
        q = p + strlen(p) - 1;          /* Find the newline */
        *q-- = '\0';                            /* Zap the newline */
        while (*q == '/')
                *q-- = '\0';                    /* Zap trailing slashes too */
        return fixname(p);
}


/*
 * Close the name file, if any.
 * BartoszP: used only in create.c
 */
void name_close( void )
{

        if (namef != NULL && namef != stdin)
                fclose(namef);
}


/*
 * Gather names in a list for scanning.
 * Could hash them later if we really care.
 *
 * If the names are already sorted to match the archive, we just
 * read them one by one.  name_gather reads the first one, and it
 * is called by name_match as appropriate to read the next ones.
 * At EOF, the last name read is just left in the buffer.
 * This option lets users of small machines extract an arbitrary
 * number of files by doing "tar t" and editing down the list of files.
 */
void name_gather( void )
{
        char  *p;
        static struct name namebuff[1];         /* One-name buffer */
        struct name *namebuf = namebuff;

        if (f_sorted_names)
        {
                p = name_next();
                if (p)
                {
                        namebuf->length = strlen(p);
                        if (namebuf->length >= sizeof namebuf->name)
                        {
                                fprintf(stderr, "Argument name too long: %s\n",
                                        p);
                                namebuf->length = (sizeof namebuf->name) - 1;
                        }
                        strncpy(namebuf->name, p, namebuf->length);
                        namebuf->next = (struct name *) NULL;
                        namebuf->found = 0;
                        namelist = namebuf;
                        namelast = namelist;
                }
                return;
        }

        /* Non sorted names -- read them all in */
        while (NULL != (p = name_next()))
        {
                addname(p);
        }
}


/*
 * Add a name to the namelist.
 */
static void addname( char *name )
{
        int    i;                      /* Length of string */
        struct name *p;        /* Current struct pointer */

        i = strlen(name);
        /* NOSTRICT */
        p = (struct name *)
                malloc((unsigned) (i + sizeof(struct name) - NAMSIZ));
        p->next = (struct name *) NULL;
        p->length = i;
        p->found = 0;
        strncpy(p->name, name, i);
        p->name[i] = '\0';                      /* Null term */
        if (namelast)
                namelast->next = p;
        namelast = p;
        if (!namelist)
                namelist = p;
}


/*
 * Match a name from an archive, p, with a name from the namelist.
 *
 * FIXME: Allow regular expressions in the name list.
 */
int name_match( char *p )
{
        struct name *nlp;
        int    len;

again:
        if (0 == (nlp = namelist))      /* Empty namelist is easy */
                return 1;
        len = strlen(p);
        for (; nlp != 0; nlp = nlp->next)
        {
                if (nlp->name[0] == p[0]/* First chars match */
                        && nlp->length <= len           /* Archive len >= specified */
                        && (p[nlp->length] == '\0' || p[nlp->length] == '/')
                /* Full match on file/dirname */
                        && strncmp(p, nlp->name, nlp->length) == 0)     /* Name compare */
                {
                        nlp->found = 1;         /* Remember it matched */
                        return 1;                       /* We got a match */
                }
        }

        /*
         * Filename from archive not found in namelist. If we have the whole
         * namelist here, just return 0. Otherwise, read the next name in and
         * compare it. If this was the last name, namelist->found will remain on.
         * If not, we loop to compare the newly read name.
         */
        if (f_sorted_names && namelist->found)
        {
                name_gather();                  /* Read one more */
                if (!namelist->found)
                        goto again;
        }
        return 0;
}


/*
 * Print the names of things in the namelist that were not matched.
 */
void names_notfound( void )
{
        struct name *nlp;
        char  *p;

        for (nlp = namelist; nlp != 0; nlp = nlp->next)
        {
                if (!nlp->found)
                {
                        fprintf(stderr, "tar: %s not found in archive\n",
                                nlp->name);
                }

                /*
                 * We could free() the list, but the process is about to die anyway,
                 * so save some CPU time.  Amigas and other similarly broken software
                 * will need to waste the time, though.
                 */
#ifndef unix
                if (!f_sorted_names)
                        free(nlp);
#endif /* unix */
        }
        namelist = (struct name *) NULL;
        namelast = (struct name *) NULL;

        if (f_sorted_names)
        {
                while (0 != (p = name_next()))
                        fprintf(stderr, "tar: %s not found in archive\n", p);
        }
}
