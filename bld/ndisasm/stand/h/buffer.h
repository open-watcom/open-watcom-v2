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


#ifndef WDIS_BUFFER_INCLUDED
#define WDIS_BUFFER_INCLUDED

extern void             BufferConcat( const char * );
extern void             BufferQuoteText( const char *text, char quote );
extern void             BufferQuoteName( const char * );
extern void             BufferConcatChar( char );
extern void             BufferConcatNL( void );
extern void             BufferHex( unsigned prec, dis_value value );
extern void             BufferHexU32( unsigned prec, uint_32 value );
extern void             BufferHex2( unsigned char value );
extern void             BufferHex4( unsigned short value );
extern void             BufferHex8( uint_32 value );
extern void             BufferDecimal( long value );
extern void             BufferUnsigned( unsigned long value );
extern void             BufferPrint( void );
extern size_t           BufferMsg( int );
extern void             BufferAlignToTab( unsigned );
extern void             BufferLabelNum( uint_32 value );
extern void             BufferLinePrefixAddress( dis_sec_offset, bool );
extern void             BufferLinePrefixData( unsigned_8 *, dis_sec_offset, dis_sec_offset, unsigned, unsigned );
extern char             *FmtHexNum( char *buff, unsigned prec, dis_value value );
extern char             *FmtLabelNum( char *buff, uint_32 value );

#endif
