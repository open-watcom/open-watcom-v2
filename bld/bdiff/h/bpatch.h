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
* Description:  Binary patch utility internals.
*
****************************************************************************/


#ifndef __BPATCH_H_
#define __BPATCH_H_

/* Usage message codes for whomever wishes to use them. */

#define MSG_USAGE_FIRST                 16

#define MSG_USAGE_LN_1                  16
#define MSG_USAGE_LN_2                  17
#define MSG_USAGE_LN_3                  18
#define MSG_USAGE_LN_4                  19
#define MSG_USAGE_LN_5                  20
#define MSG_USAGE_LN_6                  21
#define MSG_USAGE_LN_7                  22
#define MSG_USAGE_LN_8                  23
#define MSG_USAGE_LN_9                  24
#define MSG_USAGE_LN_10                 25
#define MSG_USAGE_LN_11                 26
#define MSG_USAGE_LN_12                 27

#define MSG_USAGE_LAST                  27

/* messages issued by BDIFF only */

#define MSG_IO_ERR                      32
#define MSG_DEBUG_INFO                  33
#define MSG_OVERLAYS                    34
#define MSG_IO_ERROR                    35
#define ERR_NO_MEMORY                   36
#define ERR_PATCH_BUNGLED               37

/* messages issued by BPATCH, BPATCHP, BDUMP only */

#define MSG_NOT_PATCHED                 32
#define MSG_PATCHED_TO_LEVEL            33
#define MSG_ALREADY_PATCHED             34
#define MSG_MODIFY                      35
#define ERR_TWO_NAMES                   36
#define ERR_NO_NAME                     37
#define ERR_WRONG_SIZE                  38
#define ERR_CANT_RENAME                 39
#define ERR_PATCH_ABORTED               40
#define ERR_WRONG_CHECKSUM              41
#define ERR_USE_REAL                    42
#define ERR_CANT_GET_ATTRIBUTES         43
#define MSG_SUCCESSFULLY_PATCHED        44

/* messages common to all */

#define ERR_NOT_PATCHFILE               48
#define ERR_BAD_PATCHFILE               49
#define ERR_CANT_FIND                   50
#define ERR_CANT_OPEN                   51
#define ERR_CANT_READ                   52
#define ERR_CANT_WRITE                  53
#define ERR_IO_ERROR                    54
#define ERR_MEMORY_OUT                  55
#define MSG_ERROR                       56
#define ERR_NO_SYNCSTRING               57
#define ERR_CANT_DELETE                 58
#define ERR_CANT_CREATE                 59
#define ERR_CANT_CREATE_TMP             60

#define MSG_LANG_SPACING    1000

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
