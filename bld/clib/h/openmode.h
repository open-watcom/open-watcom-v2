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


#ifndef _OPENMODE_H_INCLUDED
#define _OPENMODE_H_INCLUDED
enum {
        FILEATTR_MASK       = 0x003f,
        FILEATTR_NORMAL     = 0x0000,
        FILEATTR_READONLY   = 0x0001,
        FILEATTR_HIDDEN     = 0x0002,
        FILEATTR_SYSTEM     = 0x0004,
        FILEATTR_DIRECTORY  = 0x0010,
        FILEATTR_ARCHIVED   = 0x0020
};

enum {
        OPENFLAG_FAIL_IF_EXISTS         = 0x0000,
        OPENFLAG_OPEN_IF_EXISTS         = 0x0001,
        OPENFLAG_REPLACE_IF_EXISTS      = 0x0002,
        OPENFLAG_FAIL_IF_NOT_EXISTS     = 0x0000,
        OPENFLAG_CREATE_IF_NOT_EXISTS   = 0x0010
};

enum {
        OPENMODE_DASD                   = 0x8000,
        OPENMODE_WRITE_THROUGH          = 0x4000,
        OPENMODE_FAIL_ERRORS            = 0x2000,
        OPENMODE_NO_CACHE               = 0x1000,
        OPENMODE_LOCALITY_MASK          = 0x0700,
        OPENMODE_LOCALITY_RANDSEQ       = 0x0300,
        OPENMODE_LOCALITY_RANDOM        = 0x0200,
        OPENMODE_LOCALITY_SEQUENTIAL    = 0x0100,
        OPENMODE_LOCALITY_NONE          = 0x0000,
        OPENMODE_INHERITANCE            = 0x0080,
        OPENMODE_SHARE_MASK             = 0x0070,
        OPENMODE_DENY_COMPAT            = 0x0000,
        OPENMODE_DENY_ALL               = 0x0010,
        OPENMODE_DENY_WRITE             = 0x0020,
        OPENMODE_DENY_READ              = 0x0030,
        OPENMODE_DENY_NONE              = 0x0040,
        OPENMODE_ACCESS_MASK            = 0x0007,
        OPENMODE_ACCESS_RDONLY          = 0x0000,
        OPENMODE_ACCESS_WRONLY          = 0x0001,
        OPENMODE_ACCESS_RDWR            = 0x0002
};
#endif
