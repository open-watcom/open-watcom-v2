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
 * Create a tar archive.
 *
 * Written 25 Aug 1985 by John Gilmore, ihnp4!hoptoad!gnu.
 * MS-DOS port 2/87 by Eric Roskos.
 * Minix  port 3/88 by Eric Roskos.
 *
 * @(#)create.c 1.19 9/9/86 Public Domain - gnu
 */
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wio.h"
#include "tar.h"
#include "create.h"
#include "list.h"
#include "buffer.h"
#ifndef MSDOS
#include <pwd.h>
#include <grp.h>
#else                                                   /* ifdef MSDOS */
#include <assert.h>
#endif

#ifdef BSD42
#include <sys/dir.h>
#else
/*
 * FIXME: On other systems there is no standard place for the header file
 * for the portable directory access routines.  Change the #include line
 * below to bring it in from wherever it is.  Of course, where it is
 * on BSD is the standard place ... :-)?
 */
#ifdef V7
#include "dir.h"
#else
#if defined( __WATCOMC__ ) || !defined( __UNIX__ )
#include "direct.h"
#else
#include "ndir.h"
#endif /* __WATCOMC__ */
#endif /* V7 */
#endif /* !BSD42 */

#ifdef USG
#ifdef MSDOS
/* DOS doesn't have minor device numbers */
#ifndef major
    #define major(n)        n
#endif
#ifndef minor
    #define minor(n)        0
#endif
#else
#include <sys/sysmacros.h>              /* major() and minor() defined here */
#endif
#endif
#include <errno.h>

#include "clibext.h"


/*
 * V7 doesn't have a #define for this.
 */
#ifndef O_RDONLY
#define O_RDONLY        0
#endif

/*
 * If there are no symbolic links, there is no lstat().  Use stat().
 */
#ifndef S_IFLNK
#define lstat stat
#endif

union record   *start_header( char *name, struct stat *st );

/* Forward declarations */
int dump_file( char *fname );
void finish_header( union record *header );
void write_eot( void );
void to_oct( long value, int digs, char *where);

void create_archive( void )
{
        char  *p;

        open_archive(0);                        /* Open for writing */

        while (p = name_next()) {
                dump_file(p);
        }

        write_eot();
        close_archive();
        name_close();
}

#ifdef MSDOS
/*
 * count the number of bytes in file f.  This is done because, with DOS,
 * the results of the stat call may not reflect the actual file size, due
 * to conversion of CR/LF pairs to plain LF's.  The *only* way to find this
 * out is to read the whole file in order to let the MSC library routines
 * determine how many characters will actually be in the file...
 */
long countbytes( int f )
{
        long            cb;
        int             n;
        char            buf[512];

        assert(lseek(f, 0L, 1) == 0L);

        for (cb = 0; (n = read(f, buf, sizeof(buf))) > 0;)
                cb += n;
        lseek(f, 0L, 0);

        assert(lseek(f, 0L, 1) == 0L);

        return (cb);
}

#endif

/*
 * Dump a single file.  If it's a directory, recurse.
 * Result is 1 for success, 0 for failure.
 */
int dump_file( char *fname )
{
        struct stat     statbuff[1];
        struct stat    *statbuf = statbuff;
        union record   *header;
        char            type;

        /*
         * Use stat if following (rather than dumping) 4.2BSD's symbolic links.
         * Otherwise, use lstat (which, on non-4.2 systems, is #define'd to stat
         * anyway.
         */
        if (0 != f_follow_links ? stat(fname, statbuf) : lstat(fname, statbuf))
        {
badperror:
                perror(fname);
badfile:
                errors++;
                return 0;
        }

        switch (statbuf->st_mode & S_IFMT)
        {

        case S_IFREG:                           /* Regular file */
                {
                        int     f;      /* File descriptor */
                        int     bufsize, count;
                        long    sizeleft;
                        long    exp, actl;      /* byte counts for file size errs */
                        int     need;           /* for block read loop */
                        int     n;      /* # bytes read in this read() call */
                        char    *bufp;           /* where to start this read in buf */
                        union record *start;

                        /*
                         * Handle a regular file with multiple links.
                         *
                         * We maintain a list of all such files that we've written so far.
                         * Any time we see another, we check the list and avoid dumping
                         * the data again if we've done it once already.
                         */
                        if (statbuf->st_nlink > 1)
                        {
                                struct link *lp;

                                /*
                                 * First quick and dirty.  Hashing, etc later FIXME
                                 */
                                for (lp = linklist; lp; lp = lp->next)
                                {
                                        if (lp->ino == statbuf->st_ino &&
                                                lp->dev == statbuf->st_dev)
                                        {
                                                /* We found a link. */
                                                statbuf->st_size = 0;
                                                header = start_header(fname, statbuf);
                                                if (header == NULL)
                                                        goto badfile;
                                                strcpy(header->header.linkname,
                                                        lp->name);
                                                header->header.linkflag = LF_LINK;
                                                finish_header(header);
                                                if (f_verbose)
                                                        annorec(stdout, (char *) NULL);
                                                printf("%s link to %s\n",
                                                        fname, lp->name);

                                                /*
                                                 * Maybe remove from list after all links found?
                                                 */

                                                /*
                                                 * If so, have to compare names in case he dumps
                                                 * twice.
                                                 */

                                                /*
                                                 * Later: I don't understand the above.  If she dumps
                                                 * the file twice, it would be BAD to dump it the
                                                 * second time as a link... gnu 25Jul86
                                                 */
                                                /* FIXME */
                                                goto donefile;
                                        }
                                }

                                /* Not found.  Add it to the list. */
                                lp = (struct link *) malloc((unsigned)
                                        (strlen(fname) + sizeof(struct link) - NAMSIZ));
                                lp->ino = statbuf->st_ino;
                                lp->dev = statbuf->st_dev;
                                strcpy(lp->name, fname);
                                lp->next = linklist;
                                linklist = lp;
                        }

                        sizeleft = statbuf->st_size;
#ifdef NOTDEF                                   /* don't understand reason for following,
                                                                 * causes abort */
                        /* Don't bother opening empty, world readable files. */
                        if (sizeleft > 0 || 0444 != (0444 & statbuf->st_mode))
                        {
#endif
                                f = open(fname, O_RDONLY | convmode(fname));
                                if (f < 0)
                                        goto badperror;
#ifdef NOTDEF
                        }
                        else
                        {
                                f = -1;
                        }
#endif

#ifdef MSDOS

                        /*
                         * See comment before countbytes(), above.
                         */
                        if (convmode(fname) & O_TEXT)
                        {
                                statbuf->st_size = countbytes(f);
                                sizeleft = statbuf->st_size;
                        }
#endif
                        exp = sizeleft;         /* number of bytes we expect to see */
                        actl = 0;                       /* number of bytes we really saw */

                        header = start_header(fname, statbuf);
                        if (header == NULL)
                                goto badfile;
                        finish_header(header);
                        while (sizeleft > 0)
                        {
                                start = findrec();
                                bufsize = endofrecs()->charptr - start->charptr;
                                if (sizeleft < bufsize)
                                        bufsize = sizeleft;

                                /*
                                 * use a read loop since reads < number requested do NOT
                                 * imply EOF as John assumed -- jer 22Aug87
                                 */
                                need = bufsize;
                                bufp = start->charptr;
                                count = 0;
                                do {
                                    n = read(f, bufp, need);
                                    if (n > 0) {
                                        count += n;
                                        bufp += n;
                                        need -= n;
                                    }
                                } while (need > 0 && n > 0);

                                if (n < 0)
                                {
                                        annorec(stderr, tar);
                                        fprintf(stderr,
                                                "read error at byte %ld, reading %d bytes, in file ",
                                                statbuf->st_size - sizeleft,
                                                bufsize);
                                        perror(fname);      /* FIXME */
                                        goto padit;
                                }

                                actl += count;
                                sizeleft -= count;

                                userec(start + (count - 1) / RECORDSIZE);
                                if (count == bufsize)
                                        continue;

                                annorec(stderr, tar);
                                fprintf(stderr,
                                        "%s: file size error: expected %ld, read %ld.\n",
                                        fname, exp, actl);
                                fprintf(stderr,
                                        "%s: file shrunk by %d bytes, padding with zeros.\n",
                                        fname, sizeleft);
                                goto padit;             /* Short read */
                        }
                        if (f >= 0)
                                (void) close(f);

                        /* Clear last block garbage to zeros, FIXME */

#ifdef OLDVERBOSE
                        if (f_verbose) {
                            annorec(stdout, (char *) NULL);
                            printf("%s\n", fname);
                        }
#endif
        donefile:
                        break;

                        /*
                         * File shrunk or gave error, pad out tape to match the size we
                         * specified in the header. ??? Doesn't do that here!  Must be a
                         * FIXME. -- jer
                         */
        padit:
                        abort();
                }

#if defined S_IFLNK && defined __UNIX__
        case S_IFLNK:                           /* Symbolic link */
                {
                        int             size;

                        statbuf->st_size = 0;           /* Force 0 size on symlink */
                        header = start_header(fname, statbuf);
                        if (header == NULL)
                                goto badfile;
                        size = readlink(fname, header->header.linkname, NAMSIZ);
                        if (size < 0)
                                goto badperror;
                        if (size == NAMSIZ)
                        {
                                annorec(stderr, tar);
                                fprintf(stderr,
                                        "%s: symbolic link too long\n", fname);
                                break;
                        }
                        header->header.linkname[size] = '\0';
                        header->header.linkflag = LF_SYMLINK;
                        finish_header(header);          /* Nothing more to do to it */
#ifdef OLDVERBOSE
                        if (f_verbose)
                        {
                                annorec(stdout, (char *) NULL);
                                printf("%s\n", fname);
                        }
#endif
                }
                break;
#endif

        case S_IFDIR:                           /* Directory */
                {
                        DIR     *dirp;
                        struct dirent *d;
                        char    namebuf[NAMSIZ + 2];
                        int     len;

                        /* Build new prototype name */
                        strncpy(namebuf, fname, sizeof(namebuf));
                        len = strlen(namebuf);
                        while (len >= 1 && '/' == namebuf[len - 1])
                                len--;                  /* Delete trailing slashes */
                        namebuf[len++] = '/';           /* Now add exactly one back */

                        /*
                         * Output directory header record with permissions FIXME, do this
                         * AFTER files, to avoid R/O dir problems? If Unix Std format,
                         * don't put / on end of dir name If old archive format, don't
                         * write record at all.
                         */
                        if (!f_oldarch)
                        {
                                statbuf->st_size = 0;   /* Force 0 size on dir */

                                /*
                                 * If people could really read standard archives, this should
                                 * be:          (FIXME) header = start_header(f_standard? p:
                                 * namebuf, statbuf); but since they'd interpret LF_DIR
                                 * records as regular files, we'd better put the / on the
                                 * name.
                                 */
                                header = start_header(namebuf, statbuf);
                                if (header == NULL)
                                        goto badfile;           /* eg name too long */
                                if (f_standard)
                                {
                                        header->header.linkflag = LF_DIR;
                                }
                                finish_header(header);  /* Done with directory header */
                        }
#ifdef OLDVERBOSE
                        if (f_verbose)
                        {
                                annorec(stdout, (char *) NULL);
                                printf("%s\n", fname);
                        }
#endif
                        /*
                         * Hack to remove "./" from the front of all the file names
                         */
                        if (len == 2 && namebuf[0] == '.')
                        {
                                len = 0;
                        }

                        /* Now output all the files in the directory */
                        errno = 0;
                        dirp = opendir(fname);
                        if (!dirp) {
                            if (errno) {
                                perror(fname);
                            } else {
                                annorec(stderr, tar);
                                fprintf(stderr, "%s: error opening directory",
                                                fname);
                            }
                            break;
                        }

                        /* Should speed this up by cd-ing into the dir, FIXME */
                        while (NULL != (d = readdir(dirp)))
                        {
                                /* Skip . and .. */
                                if (d->d_name[0] == '.')
                                {
                                        if (d->d_name[1] == '\0')
                                                continue;
                                        if (d->d_name[1] == '.')
                                        {
                                                if (d->d_name[2] == '\0')
                                                        continue;
                                        }
                                }
                                if (strlen(d->d_name) + len >= NAMSIZ)
                                {
                                        annorec(stderr, tar);
                                        fprintf(stderr, "%s%s: name too long\n",
                                                namebuf, d->d_name);
                                        continue;
                                }
                                strcpy(namebuf + len, d->d_name);
                                dump_file(namebuf);
                        }

                        closedir(dirp);
                }
                break;

        case S_IFCHR:                           /* Character special file */
                type = LF_CHR;
                goto easy;

#ifdef S_IFBLK
        case S_IFBLK:                           /* Block     special file */
                type = LF_BLK;
                goto easy;
#endif

#ifdef S_IFIFO
        case S_IFIFO:                           /* Fifo      special file */
                type = LF_FIFO;
#endif /* S_IFIFO */

easy:
                if (!f_standard)
                        goto unknown;

#if 0
                /* this was a bad kludge, it had the side-effect of causing
                 * skipping of the data part of files when a "list" was
                 * done to work right for special files.  But, Minix needs
                 * this size since it's used to size the device. - JER */
                statbuf->st_size = 0;   /* Force 0 size */
#endif
                header = start_header(fname, statbuf);
                if (header == NULL)
                        goto badfile;           /* eg name too long */

                header->header.linkflag = type;
                if (type != LF_FIFO)
                {
                        to_oct((long) major(statbuf->st_rdev), 8,
                                header->header.devmajor);
                        to_oct((long) minor(statbuf->st_rdev), 8,
                                header->header.devminor);
                }

                finish_header(header);
#ifdef OLDVERBOSE
                if (f_verbose)
                {
                        annorec(stdout, (char *) NULL);
                        printf("%s\n", fname);
                }
#endif
                break;

        default:
unknown:
                annorec(stderr, tar);
                fprintf(stderr,
                        "%s: Unknown file type; file ignored.\n", fname);
                break;
        }

        return 1;                                       /* Success */
}


/*
 * Make a header block for the file  name  whose stat info is  st .
 * Return header pointer for success, NULL if the name is too long.
 */
union record   *start_header( char *name, struct stat *st )
{
        union record *header;

        hstat[0] = *st;                         /* save stat for verbose-mode listing */

        header = (union record *) findrec();
        memset(header->charptr, 0, sizeof(*header));        /* XXX speed up */
        strcpy(header->header.name, name);
        if (header->header.name[NAMSIZ - 1])
        {
                annorec(stderr, tar);
                fprintf(stderr, "%s: name too long\n", name);
                return NULL;
        }
#ifdef MSDOS
        to_oct((long) (st->st_mode & ~(S_IFMT | DOSUMASK)),
                8, header->header.mode);
#else
        to_oct((long) (st->st_mode & ~S_IFMT),
                8, header->header.mode);
#endif
        to_oct((long) st->st_uid, 8, header->header.uid);
        to_oct((long) st->st_gid, 8, header->header.gid);
        to_oct((long) st->st_size, 1 + 12, header->header.size);
        to_oct((long) st->st_mtime, 1 + 12, header->header.mtime);
        /* header->header.linkflag is left as null */

        /* Fill in new Unix Standard fields if desired. */
        if (f_standard) {
                header->header.linkflag = LF_NORMAL;    /* New default */
                strcpy(header->header.magic, TMAGIC);   /* Mark as Unix Std */
#ifndef NONAMES
                finduname(header->header.uname, st->st_uid);
                findgname(header->header.gname, st->st_gid);
#else
                /* don't leave garbage in if no name - JER */
                header->header.uname[0] = '\0';
                header->header.gname[0] = '\0';
#endif
        }
        return header;
}

/*
 * Finish off a filled-in header block and write it out.
 */
void finish_header( union record *header )
{
    int    i, sum;
    char  *p;

    memcpy(CHKBLANKS, header->header.chksum, sizeof(header->header.chksum));

    sum = 0;
    p = header->charptr;
    for (i = sizeof(*header); --i >= 0;) {
        /*
        * We can't use unsigned char here because of old compilers, e.g. V7.
        */
        sum += 0xFF & *p++;
    }

    /*
    * Fill in the checksum field.  It's formatted differently from the other
    * fields:  it has [6] digits, a null, then a space -- rather than
    * digits, a space, then a null. We use to_oct then write the null in
    * over to_oct's space. The final space is already there, from
    * checksumming, and to_oct doesn't modify it.
    *
    * This is a fast way to do: (void) sprintf(header->header.chksum, "%6o",
    * sum);
    */
    to_oct((long) sum, 8, header->header.chksum);
    header->header.chksum[6] = '\0';        /* Zap the space */

    head = header;
    if (f_verbose) {
        print_header(header->header.name);
    }
    
    userec(header);
    return;
}


/*
 * Quick and dirty octal conversion.
 * Converts long "value" into a "digs"-digit field at "where",
 * including a trailing space and room for a null.  "digs"==3 means
 * 1 digit, a space, and room for a null.
 *
 * We assume the trailing null is already there and don't fill it in.
 * This fact is used by start_header and finish_header, so don't change it!
 *
 * This should be equivalent to:
 *      (void) sprintf(where, "%*lo ", digs-2, value);
 * except that sprintf fills in the trailing null and we don't.
 */
void to_oct( long value, int digs, char *where)
{

    --digs;                                         /* Trailing null slot is left alone */
    where[--digs] = ' ';            /* Put in the space, though */

    /* Produce the digits -- at least one */
    do {
        where[--digs] = '0' + (value & 7);              /* one octal digit */
        value >>= 3;
    } while (digs > 0 && value != 0);

    /* Leading spaces, if necessary */
    while (digs > 0) {
        where[--digs] = ' ';
    }

}


/*
 * Write the EOT block(s).
 */
void write_eot( void )
{
        union record   *p;

        p = findrec();
        memset(p->charptr, 0, RECORDSIZE);
        userec(p);
        /* FIXME, only one EOT block should be needed. */
        p = findrec();
        memset(p->charptr, 0, RECORDSIZE);
        userec(p);
}
