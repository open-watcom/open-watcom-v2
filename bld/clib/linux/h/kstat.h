/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2025      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Linux kernel stat structure definitions
*
****************************************************************************/


#pragma pack( __push, 4 )
struct kstat32 {
    unsigned long           dev;
    unsigned long           ino;
    unsigned int            mode;
    unsigned int            nlink;
    unsigned int            uid;
    unsigned int            gid;
    unsigned long           rdev;
    unsigned long           __pad1;
    long                    size;
    int                     blksize;
    int                     __pad2;
    long                    blocks;
    long                    atime;       /* Time of last access.  */
    unsigned long           atime_nsec;
    long                    mtime;       /* Time of last modification.  */
    unsigned long           mtime_nsec;
    long                    ctime;       /* Time of last status change.  */
    unsigned long           ctime_nsec;
    unsigned int            __pad3;
    unsigned int            __pad4;
};

struct kstat64 {
    unsigned long long      dev;
    unsigned long long      ino;
    unsigned int            mode;
    unsigned int            nlink;
    unsigned int            uid;
    unsigned int            gid;
    unsigned long long      rdev;
    unsigned long long      __pad1;
    long long               size;
    int                     blksize;
    int                     __pad2;
    long                    blocks;
    int                     atime;       /* Time of last access.  */
    unsigned int            atime_nsec;
    int                     mtime;       /* Time of last modification.  */
    unsigned int            mtime_nsec;
    int                     ctime;       /* Time of last status change.  */
    unsigned int            ctime_nsec;
    unsigned int            __pad3;
    unsigned int            __pad4;
};
#pragma pack( __pop )

#define COPY_STAT(d,s) \
    d->st_dev = s.dev; \
    d->st_ino = s.ino; \
    d->st_mode = s.mode; \
    d->st_nlink = s.nlink; \
    d->st_uid = s.uid; \
    d->st_gid = s.gid; \
    d->st_rdev = s.rdev; \
    d->st_size = s.size; \
    d->st_blksize = s.blksize; \
    d->st_blocks = s.blocks; \
    d->st_atime.tv_sec = s.atime; \
    d->st_atime.tv_nsec = s.atime_nsec; \
    d->st_mtime.tv_sec = s.mtime; \
    d->st_mtime.tv_nsec = s.mtime_nsec; \
    d->st_ctime.tv_sec = s.ctime; \
    d->st_ctime.tv_nsec = s.ctime_nsec
