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


#ifndef _DWARF_MISC_H
#define _DWARF_MISC_H

#include <wstd.h>

// Defines ----------------------------------------------------------------

#define MAX_ULEB_SIZE   5

// Public functions -------------------------------------------------------

extern uint_8 *DecodeULEB128( const uint_8 *input, uint_32 *value );
extern uint_8 *DecodeLEB128( const uint_8 *input, int_32 *value );
extern uint_8 *readString( const uint_8 *input, char ** pString );
extern uint_8 *EncodeULEB128( uint_8 * buf, uint_32 value );
extern uint_8 *EncodeLEB128( uint_8 * buf, int_32 value );
extern uint_8 *SkipForm( uint_8 * p, uint_16 form );
extern uint_8 *SkipLEB128( uint_8 * p );
extern void    SetGlobalParms( uint_8 addressSize );

#if DEBUG_DUMP
extern void    dumpMemory( uint_8 * data, unsigned long length, char * desc );
#endif

#endif

