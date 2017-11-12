/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2017 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Client independent utility functions for DWARF writer lib.
*
****************************************************************************/


#ifndef DWCLIUTI_H_INCLUDED
#define DWCLIUTI_H_INCLUDED

#include "dwcnf.h"

#ifndef __BIG_ENDIAN__
#define WriteU16( __p, __v )    ( *(uint_16 *)(__p) = (__v) )
#define WriteU32( __p, __v )    ( *(uint_32 *)(__p) = (__v) )
#define WriteS16( __p, __v )    ( *(uint_16 *)(__p) = (__v) )
#define WriteS32( __p, __v )    ( *(uint_32 *)(__p) = (__v) )
#else
#error "must define WriteU16/32 and WriteS16/32 functions"
#endif

#define CLISectionOffset(c,s)       (CLITell( c, s ) - (c)->section_base[s])
#define CLISectionAbs(c,s)          CLITell( c, s )
#define CLISectionSkip(c,s,l)       CLISeek( c, s, l, DW_SEEK_CUR )
#define CLISectionSeekOffset(c,s,o) CLISeek( c, s, (c)->section_base[s] + (o), DW_SEEK_SET )
#define CLISectionSeekAbs(c,s,o)    CLISeek( c, s, (o), DW_SEEK_SET )
#define CLISectionSeekEnd(c,s)      CLISeek( c, s, 0, DW_SEEK_END )

#define CLISectionReserveSize(c,s)  CLISectionSkip( c, s, sizeof( uint_32 ) )
#define InfoSkip(c,l)               CLISectionSkip( c, DW_DEBUG_INFO, l )
#define InfoSectionOffset(c)        CLISectionOffset( c, DW_DEBUG_INFO )

extern void     CLIWriteLEB128( dw_client cli, dw_sectnum sect, dw_sconst value );
extern void     CLIWriteULEB128( dw_client cli, dw_sectnum sect, dw_uconst value );
extern void     CLIWriteU8( dw_client cli, dw_sectnum sect, uint_8 data );
extern void     CLIWriteU16( dw_client cli, dw_sectnum sect, uint_16 data );
extern void     CLIWriteU32( dw_client cli, dw_sectnum sect, uint_32 data );
extern void     CLIWriteString( dw_client cli, dw_sectnum sect, const char *str );

extern void     CLISectionSetSize( dw_client cli, dw_sectnum sect );
extern void     CLISectionWriteZeros( dw_client cli, dw_sectnum sect, size_t len );

#endif
