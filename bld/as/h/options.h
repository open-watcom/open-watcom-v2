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
    EMPTY               = 0x0000,
    OBJ_COFF            = 0x0001, // ELF if this bit is not set
    BE_QUIET            = 0x0002,
    PRINT_HELP          = 0x0004,
    WARNING_ERROR       = 0x0008,
    DUMP_PARSE_TREE     = 0x0100,
    DUMP_INS_TABLE      = 0x0200,
    DUMP_SYMBOL_TABLE   = 0x0400,
    DUMP_INSTRUCTIONS   = 0x0800,
    DUMP_LEXER_BUFFER   = 0x1000,
    DUMP_DEBUG_MSGS     = 0x2000
} as_flags;

#define _SetOption( x )         ( AsOptions |= (x) )
#define _UnsetOption( x )       ( AsOptions &= ~(x) )
#define _IsOption( x )          ( ( AsOptions & (x) ) != EMPTY )

extern as_flags AsOptions;
