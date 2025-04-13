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
* Description:  Linux kernel statfs structure definitions
*
****************************************************************************/


#pragma pack( __push, 4 )
struct kstatfs32 {
    long                f_type;
    long                f_bsize;
    unsigned long       f_blocks;
    unsigned long       f_bfree;
    unsigned long       f_bavail;
    unsigned long       f_files;
    unsigned long       f_ffree;
    struct {
        int val[2];
    }                   f_fsid;
    long                f_namelen;
    long                f_frsize;
    long                f_flag;
    long                f_spare[4];
};

struct kstatfs64 {
    long                f_type;
    long                f_bsize;
    unsigned long long  f_blocks;
    unsigned long long  f_bfree;
    unsigned long long  f_bavail;
    unsigned long long  f_files;
    unsigned long long  f_ffree;
    struct {
        int val[2];
    }                   f_fsid;
    long                f_namelen;
    long                f_frsize;
    long                f_flag;
    long                f_spare[4];
};
#pragma pack( __pop )

#define COPY_STATFS(d,s) \
    d->f_bsize = s.f_bsize; \
    d->f_blocks = s.f_blocks; \
    d->f_bfree = s.f_bfree; \
    d->f_bavail = s.f_bavail; \
    d->f_files = s.f_files; \
    d->f_ffree = s.f_ffree; \
    d->f_fsid = *(unsigned long *)s.f_fsid.val; \
    d->f_namemax = s.f_namelen; \
    d->f_frsize = s.f_frsize; \
    d->f_flag = s.f_flag
