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
 * List a tar archive.
 *
 * Also includes support routines for reading a tar archive.
 *
 * Pubic Domain version written 26 Aug 1985 by John Gilmore (ihnp4!hoptoad!gnu).
 * MS-DOS port 2/87 by Eric Roskos.
 * Minix  port 3/88 by Eric Roskos.
 *
 * @(#)list.c 1.18 9/23/86 Public Domain - gnu
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifndef MSDOS
#ifndef V7
#include <sys/file.h>
#endif /* V7 */
#else                                                   /* ifdef MSDOS */
#include <fcntl.h>
#endif

#define isodigit(c)     ( ((c) >= '0') && ((c) <= '7') )

#include "tar.h"
#include "buffer.h"
#include "list.h"

union record   *head;                   /* Points to current archive header */
struct stat     hstat[1];               /* Stat struct corresponding */
struct stat    *phstat = hstat;         /* to overcome construct ACK C can't handle */

/* Forward declarations */
void demode( unsigned int mode, char *string );

/*
 * Print a header record, based on tar options.
 */
void list_archive( char *xname )
{

        /* Save the record */
        saverec(&head);

        /* Print the header record */
        print_header(xname);

        /* Skip past it in the archive */
        saverec((union record **) 0);           /* Unsave it */
        userec(head);

        /* Skip to the next header on the archive */
        if (head->header.linkflag != LF_BLK &&
            head->header.linkflag != LF_CHR) /* no skip if special file - JER */
                skip_file((long) phstat->st_size);
}


/*
 * Read a record that's supposed to be a header record.
 * Return its address in "head", and if it is good, the file's
 * size in hstat->st_size.
 *
 * Return 1 for success, 0 if the checksum is bad, EOF on eof,
 * 2 for a block full of zeros (EOF marker).
 *
 * You must always userec(head) to skip past the header which this
 * routine reads.
 */
static int read_header( void )
{
        int    i;
        long   sum, recsum;
        char  *p;
        union record *header;

        header = findrec();
        head = header;                          /* This is our current header */
        if (NULL == header)
                return EOF;

        recsum = from_oct(8, header->header.chksum);

        sum = 0;
        p = header->charptr;
        for (i = sizeof(*header); --i >= 0;)
        {

                /*
                 * We can't use unsigned char here because of old compilers, e.g. V7.
                 */
                sum += 0xFF & *p++;
        }

        /* Adjust checksum to count the "chksum" field as blanks. */
        for (i = sizeof(header->header.chksum); --i >= 0;)
                sum -= 0xFF & header->header.chksum[i];
        sum += ' ' * sizeof header->header.chksum;

        if (sum == recsum)
        {

                /*
                 * Good record.  Decode file size and return.
                 */
                if (header->header.linkflag == LF_LINK)
                        phstat->st_size = 0;    /* Links 0 size on tape */
                else
                        phstat->st_size = from_oct(1 + 12, header->header.size);
                return 1;
        }

        if (sum == 8 * ' ')
        {

                /*
                 * This is a zeroed block...whole block is 0's except for the 8
                 * blanks we faked for the checksum field.
                 */
                return 2;
        }

        return 0;
}


/*
 * Decode things from a file header record into a "struct stat".
 * Also set "*stdp" to !=0 or ==0 depending whether header record is "Unix
 * Standard" tar format or regular old tar format.
 *
 * read_header() has already decoded the checksum and length, so we don't.
 *
 * If wantug != 0, we want the uid/group info decoded from Unix Standard
 * tapes (for extraction).  If == 0, we are just printing anyway, so save time.
 */
void decode_header( union record *header, struct stat *st, 
                     int *stdp, int wantug)
{
        st->st_mode = from_oct(8, header->header.mode);
        st->st_mtime = from_oct(1 + 12, header->header.mtime);

        if (0 == strcmp(header->header.magic, TMAGIC))
        {
                /* Unix Standard tar archive */
                *stdp = 1;
                if (wantug)
                {
#ifndef MSDOS
#ifndef NONAMES /* if we have names, still support noname tapes - JER */
                        if (header->header.uname[0]) /* JER */
                                st->st_uid = finduid(header->header.uname);
                        else
#endif /* NONAMES */
                                st->st_uid = from_oct(8, header->header.uid);
#ifndef NONAMES
                        if (header->header.gname[0]) /* JER */
                                st->st_gid = findgid(header->header.gname);
                        else
#endif /* NONAMES */
                                st->st_gid = from_oct(8, header->header.gid);
#else /* MSDOS */
                        st->st_uid = st->st_gid = 0;            /* unsupported in DOS */
#endif /* MSDOS */
                }
#ifndef MSDOS
                switch (header->header.linkflag)
                case LF_BLK:
                case LF_CHR:
                        st->st_dev = makedev(from_oct(8, header->header.devmajor),
                                from_oct(8, header->header.devminor));
#endif
        }
        else
        {
                /* Old fashioned tar archive */
                *stdp = 0;
                st->st_uid = from_oct(8, header->header.uid);
                st->st_gid = from_oct(8, header->header.gid);
                st->st_dev = 0;
        }
}


/*
 * Quick and dirty octal conversion.
 *
 * Result is -1 if the field is invalid (all blank, or nonoctal).
 */
long from_oct( int digs, char * where)
{
        long   value;

        while (isspace(*where))
        {                                                       /* Skip spaces */
                where++;
                if (--digs <= 0)
                        return -1;                      /* All blank field */
        }
        value = 0;
        while (digs > 0 && isodigit(*where))
        {                                                       /* Scan til nonoctal */
                value = (value << 3) | (*where++ - '0');
                --digs;
        }

        if (digs > 0 && *where && !isspace(*where))
                return -1;                              /* Ended on non-space/nul */

        return value;
}


/*
 * Actually print it.
 */
#define UGSWIDTH        11                      /* min width of User, group, size */
#define DATEWIDTH       19                      /* Last mod date */
static int      ugswidth = UGSWIDTH;    /* Max width encountered so far */

void print_header(char * xname)
{
        char    modes[11];
        char    *timestamp;
        char    uform[11], gform[11];           /* These hold formatted ints */
        char    *user, *group;
        char    size[24];       /* Holds a formatted long or maj, min */
        long    longie;         /* To make ctime() call portable */
        int     pad;
        int     header_std;     /* Is header standard or not? */
        int     i;
#ifdef MSDOS
        char    blanks[26];
#endif

        annofile(stdout, (char *) NULL);

        if (f_verbose)
        {
                decode_header(head, hstat, &header_std, 0);

                /* File type and modes */
                modes[0] = '?';
                switch (head->header.linkflag)
                {
                case LF_NORMAL:
                case LF_OLDNORMAL:
                case LF_LINK:
                        modes[0] = '-';
                        if ('/' == head->header.name[strlen(head->header.name) - 1])
                                modes[0] = 'd';
                        break;
                case LF_DIR:
                        modes[0] = 'd';
                        break;
                case LF_SYMLINK:
                        modes[0] = 'l';
                        break;
                case LF_BLK:
                        modes[0] = 'b';
                        break;
                case LF_CHR:
                        modes[0] = 'c';
                        break;
                case LF_FIFO:
                        modes[0] = 'f';
                        break;
                case LF_CONTIG:
                        modes[0] = '=';
                        break;
                }
#ifdef MSDOS
                if (convmode(xname) & O_TEXT)
                        modes[0] = 'a';
#endif

                demode((unsigned) phstat->st_mode, modes + 1);

                /* Timestamp */
                longie = phstat->st_mtime;
#ifdef MSDOS
                /* following is due to a bug in MSDOS's ctime() */
                if (longie < 0x10000000L) {
                    memset(blanks, ' ', sizeof(blanks)-1);
                    blanks[sizeof(blanks)-1] = '\0';
                    timestamp = blanks;
                }
                else
#endif
                    timestamp = ctime((unsigned long*)(&longie));
                timestamp[16] = '\0';
                timestamp[24] = '\0';

                /* User and group names */
                if (*head->header.uname && header_std)
                {
                        user = head->header.uname;
                }
                else
                {
                        user = uform;
                        (void) sprintf(uform, "%d", (int) phstat->st_uid);
                }
                if (*head->header.gname && header_std)
                {
                        group = head->header.gname;
                }
                else
                {
                        group = gform;
                        (void) sprintf(gform, "%d", (int) phstat->st_gid);
                }

                /* Format the file size or major/minor device numbers */
                switch (head->header.linkflag)
                {
                case LF_CHR:
                case LF_BLK:
#ifdef V7
                        (void) sprintf(size, "(%d, %d) %D",
#else
                        (void) sprintf(size, "%d, %d",
#endif
                                major(phstat->st_dev),
                                minor(phstat->st_dev),
                                /* size has meaning for Minix - JER */
                                (long)phstat->st_size);
                        break;

                default:
#ifdef V7
                        (void) sprintf(size, "%D", (long) phstat->st_size);
#else
                        (void) sprintf(size, "%ld", (long) phstat->st_size);
#endif
                }

                /* Figure out padding and print the whole line. */
                pad = strlen(user) + strlen(group) + strlen(size) + 1;
                if (pad > ugswidth)
                        ugswidth = pad;

                        printf("%s %s/%s ", modes, user, group);
                        for (i = ugswidth - pad; i > 0; i--) putchar(' ');
                        printf("%s %s %s %s", size, timestamp+4,
                                timestamp+20, head->header.name);
        }
        else
        {
                printf("%s", head->header.name);
        }
        if (strcmp(head->header.name, xname))
                printf(" (%s)", xname);

        if (f_verbose)
                switch (head->header.linkflag)
                {
                case LF_SYMLINK:
                        printf(" -> %s\n", head->header.linkname);
                        break;

                case LF_LINK:
                        printf(" link to %s\n", head->header.linkname);
                        break;

                default:
                        printf(" unknown file type '%c'\n", head->header.linkflag);
                        break;

                case LF_OLDNORMAL:
                case LF_NORMAL:
                case LF_CHR:
                case LF_BLK:
                case LF_DIR:
                case LF_FIFO:
                case LF_CONTIG:
                        putc('\n', stdout);
                        break;
                }
        else
        {
                putc('\n', stdout);
        }

        /* FIXME: we don't print major/minor device numbers */
}

/*
 * Print a similar line when we make a directory automatically.
 */
void pr_mkdir( char *pathname, int length, int mode )
{
        char            modes[11];

        if (f_verbose)
        {
                /* File type and modes */
                modes[0] = 'd';
                demode((unsigned) mode, modes + 1);

                annofile(stdout, (char *) NULL);
                printf("%s %*s %.*s\n",
                        modes,
                        ugswidth + DATEWIDTH,
                        "Creating directory:",
                        length,
                        pathname);
        }
}


/*
 * Skip over <size> bytes of data in records in the archive.
 */
void skip_file( long   size )
{
        union record   *x;

        while (size > 0)
        {
                x = findrec();
                if (x == NULL)
                {                                               /* Check it... */
                        annorec(stderr, tar);
                        fprintf(stderr, "Unexpected EOF on archive file\n");
                        exit(EX_BADARCH);
                }
                userec(x);
                size -= RECORDSIZE;
        }
}


/*
 * Decode the mode string from a stat entry into a 9-char string and a null.
 * This is good, portable coding, John!
 */
void demode( unsigned int mode, char *string )
{
        unsigned int    mask;
        char             *rwx = "rwxrwxrwx";

        for (mask = 0400; mask != 0; mask >>= 1)
        {
                if (mode & mask)
                        *string++ = *rwx++;
                else
                {
                        *string++ = '-';
                        rwx++;
                }
        }

#ifdef S_ISUID
        if (mode & S_ISUID)
                if (string[-7] == 'x')
                        string[-7] = 's';
                else
                        string[-7] = 'S';
#endif
#ifdef S_ISGID
        if (mode & S_ISGID)
                if (string[-4] == 'x')
                        string[-4] = 's';
                else
                        string[-4] = 'S';
#endif
#ifdef S_ISVTX
        if (mode & S_ISVTX)
                if (string[-1] == 'x')
                        string[-1] = 't';
                else
                        string[-1] = 'T';
#endif
        *string = '\0';
}

/*
 * Main loop for reading an archive.
 */
void read_and( void (*do_something)( char *dummy ) )
{
        int             status = 1;
        int             prev_status;
        char           *xname;

        name_gather();                          /* Gather all the names */
        open_archive(1);                        /* Open for reading */

        for (;;)
        {
                prev_status = status;
                status = read_header();
                switch (status)
                {

                case 1:                         /* Valid header */
                        /* We should decode next field (mode) first... */
                        /* Ensure incoming names are null terminated. */
                        head->header.name[NAMSIZ - 1] = '\0';
                        /* make a valid filename for this OS */
                        xname = fixname(head->header.name);

                        if (!name_match(head->header.name))
                        {
                                /* Skip past it in the archive */
                                userec(head);
                                /* Skip to the next header on the archive */
                                if (head->header.linkflag != LF_BLK &&
                                    head->header.linkflag != LF_CHR)
                                        skip_file((long) phstat->st_size);
                                continue;
                        }

                        (*do_something) (xname);
                        continue;

                        /*
                         * If the previous header was good, tell them that we are
                         * skipping bad ones.
                         */
                case 0:                         /* Invalid header */
        case0:
                        userec(head);
                        if (prev_status == 1)
                        {
                                annorec(stderr, tar);
                                fprintf(stderr,
                                        "Skipping to next file header...\n");
                        }
                        continue;

                case 2:                         /* Block of zeroes */
                        if (f_ignorez)
                                goto case0;             /* Just skip if asked */
                        /* FALL THRU */
                case EOF:                               /* End of archive */
                        break;
                }
                break;
        };

        close_archive();
        names_notfound();                       /* Print names not found */
}
