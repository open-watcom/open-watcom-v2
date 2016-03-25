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


extern char *JmpLabel( uint_32 addr, uint_32 off );
extern char *ToStr( uint_32 value, uint_16 length, uint_32 addr );
extern char *ToIndex( uint_32 addr, uint_32 value );
extern char *ToBrStr( uint_32 value, uint_32 addr );
extern char *ToSegStr( uint_32 value, uint_16 seg, uint_32 addr );
extern uint_32 GetOffset( void );
extern int_16 GetNextByte( void );
extern int_16 GetDataByte( void );
extern int_16 GetDataWord( void );
extern int_32 GetDataLong( void );
extern bool EndOfSegment( void );

extern void  DoCode( instruction *curr, bool use_32 );
extern void  FormatIns( char *buf, instruction *curr_ins, form_option format );
