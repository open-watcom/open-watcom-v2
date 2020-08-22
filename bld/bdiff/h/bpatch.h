/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Binary patch utility internals.
*
****************************************************************************/


#ifndef __BPATCH_H_
#define __BPATCH_H_

#include "bpatch.rh"

/* Function prototypes */

/* From msg.c */

typedef enum {
    PATCH_RET_OKAY,
    PATCH_NO_MEMORY,
    PATCH_BAD_PATCH_FILE,
    PATCH_CANT_FIND_PATCH,
    PATCH_BAD_PATCH,
    PATCH_ALREADY_PATCHED,
    PATCH_BAD_LENGTH,
    PATCH_CANT_RENAME,
    PATCH_CANT_WRITE,
    PATCH_CANT_READ,
    PATCH_IO_ERROR,
    PATCH_CANT_OPEN_FILE,
    PATCH_CANT_GET_ATTRIBUTES,
    PATCH_RET_CANCEL,
    PATCH_BAD_CHECKSUM
} PATCH_RET_CODE;

#endif
