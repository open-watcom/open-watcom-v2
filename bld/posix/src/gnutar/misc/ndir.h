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


/* @(#)ndir.h   1.4     4/16/85 */
#ifndef DEV_BSIZE
#define DEV_BSIZE       512
#endif
#define DIRBLKSIZ       DEV_BSIZE
#define MAXNAMLEN       255

struct direct
{
                long d_ino;                             /* inode number of entry */
                short d_reclen;                 /* length of this record */
                short d_namlen;                 /* length of string in d_name */
                char d_name[MAXNAMLEN + 1];             /* name must be no longer than this */
};

/*
 * The DIRSIZ macro gives the minimum record length which will hold
 * the directory entry.  This requires the amount of space in struct direct
 * without the d_name field, plus enough space for the name with a terminating
 * null byte (dp->d_namlen+1), rounded up to a 4 byte boundary.
 */

#ifdef DIRSIZ
#undef DIRSIZ
#endif                                                  /* DIRSIZ */
#define DIRSIZ(dp) \
    ((sizeof (struct direct) - (MAXNAMLEN+1)) + (((dp)->d_namlen+1 + 3) &~ 3))

/*
 * Definitions for library routines operating on directories.
 */
typedef struct _dirdesc
{
        char whichdir[256]; /* pathname used to open directory */

        struct dosdir           /* DOS directory-search structure  */
        {
                char    doshdr[21];
                char    attrib;
                short   time;
                short   date;
                short   size_l;
                short   size_h;
                char    name[13];
        } dosdir;
} DIR;

#ifndef NULL
#define NULL 0
#endif
extern DIR *opendir();
extern struct direct *readdir();
extern void closedir();
