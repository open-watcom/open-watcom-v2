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
 * Look up user and/or group names.
 *
 * This file should be modified for non-unix systems to do something
 * reasonable.
 *
 * @(#)names.c 1.1 9/9/86 Public Domain - gnu
 * MS-DOS port 2/87 by E. Roskos.
 * Minix  port 3/88 by E. Roskos.
 */
#include <sys/types.h>
#ifndef MSDOS
#include <pwd.h>
#include <grp.h>
#endif
#include "tar.h"

#if 0
static int      saveuid = -993;
static char     saveuname[TUNMLEN];
static int      my_uid = -993;

static int      savegid = -993;
static char     savegname[TGNMLEN];
static int      my_gid = -993;
#endif

#define myuid   ( my_uid < 0? my_uid = getuid(): my_uid )
#define mygid   ( my_gid < 0? my_gid = getgid(): my_gid )


#ifndef NONAMES
/*
 * Look up a user or group name from a uid/gid, maintaining a cache.
 * FIXME, for now it's a one-entry cache.
 * FIXME2, the "-993" is to reduce the chance of a hit on the first lookup.
 *
 * This is ifdef'd because on Suns, it drags in about 38K of "yellow
 * pages" code, roughly doubling the program size.  Thanks guys.
 */
void finduname( char uname[TUNMLEN], int uid )
{
        struct passwd  *pw;

        if (uid != saveuid)
        {
                saveuid = uid;
                saveuname[0] = '\0';
                pw = getpwuid(uid);
                if (pw)
                        strncpy(saveuname, pw->pw_name, TUNMLEN);
        }
        strncpy(uname, saveuname, TUNMLEN);
}

int finduid( char uname[TUNMLEN] )
{
        struct passwd  *pw;

        if (uname[0] != saveuname[0]/* Quick test w/o proc call */
                || 0 != strncmp(uname, saveuname, TUNMLEN))
        {
                strncpy(saveuname, uname, TUNMLEN);
                pw = getpwnam(uname);
                if (pw)
                {
                        saveuid = pw->pw_uid;
                }
                else
                {
                        saveuid = myuid;
                }
        }
        return saveuid;
}


void findgname( char gname[TGNMLEN], int gid )
{
        struct group   *gr;

        if (gid != savegid)
        {
                savegid = gid;
                savegname[0] = '\0';
                (void) setgrent();
                gr = getgrgid(gid);
                if (gr)
                        strncpy(savegname, gr->gr_name, TGNMLEN);
        }
        (void) strncpy(gname, savegname, TGNMLEN);
}


int findgid( char gname[TUNMLEN] )
{
        struct group   *gr;

        if (gname[0] != savegname[0]/* Quick test w/o proc call */
                || 0 != strncmp(gname, savegname, TUNMLEN))
        {
                strncpy(savegname, gname, TUNMLEN);
                gr = getgrnam(gname);
                if (gr)
                {
                        savegid = gr->gr_gid;
                }
                else
                {
                        savegid = mygid;
                }
        }
        return savegid;
}

#endif
