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


#ifndef _PROGSW_H_INCLUDED
#define _PROGSW_H_INCLUDED

typedef enum {
    PS_FIRST_COMPILE      = 0x0001,
    PS_END_OF_SUBPROG     = 0x0002,
    PS_SOURCE_EOF         = 0x0004,
    PS_IN_SUBPROGRAM      = 0x0008,
    PS_BLOCK_DATA         = 0x0010,
    PS_PROGRAM_DONE       = 0x0020,
    PS_ERROR              = 0x0040,
    PS_LIBRARY_PROCESS    = 0x0080,  // processing undefined subprograms
    PS_ERR_OPEN_TRIED     = 0x0100,
    PS_DONT_GENERATE      = 0x0200,  // don't generate any code
    PS_SKIP_SOURCE        = 0x0400,  // skip source line because of macro
    PS_FATAL_ERROR        = 0x0800,
    PS_INC_EOF            = 0x1000,
    PS_LIB_OBJECT         = 0x2000,  // loading object
    PS_STMT_TOO_BIG       = 0x4000,  // out of memory building i.t list
    PS_SYMTAB_PROCESS     = 0x8000
} PSWT;

#endif
