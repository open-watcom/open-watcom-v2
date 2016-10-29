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


typedef enum {
    OP_NONE =           0x0000,
    OP_DELETE =         0x0001,
    OP_ADD =            0x0002,
    OP_EXTRACT =        0x0004,

    OP_DELETED =        0x0010,
    OP_ADDED =          0x0020,
    OP_EXTRACTED =      0x0040,
    OP_FOUND =          0x0080,

    OP_IMPORT =         0x0100,
    OP_PRINT =          0x0200,
    OP_TABLE =          0x0400
} operation;

typedef struct lib_cmd {
    struct      lib_cmd *next;
    operation   ops;
    char        *fname;
    char        name[1];
} lib_cmd;

extern lib_cmd *CmdList;
