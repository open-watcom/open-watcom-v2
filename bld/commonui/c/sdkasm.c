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
* Description:  Interface to x86 disassembler engine.
*
****************************************************************************/


#include "precomp.h"
#include "sdkasm.h"

static DisAsmRtns       *CurRtns;

/*
 * RegisterRtns - register a set of formatting routines
 *              - this function must be called for each DisAsmRtns structure
 *                before it is used
 */
void RegisterRtns( DisAsmRtns *rtns )
{
    rtns = rtns;

} /* RegisterRtns */

/*
 * MiscDoCode - cover function for the DoCode function of the disassembler
 */
void MiscDoCode( instruction *ins, char is32, DisAsmRtns *rtns )
{
    if( CurRtns != NULL ) {
        MessageBox( NULL, "Disassembly Error!!", "", MB_OK );
        return;
    }
    CurRtns = rtns;
    DoCode( ins, is32 );
    CurRtns = NULL;

} /* MiscDoCode */

/*
 * MiscFormatIns - cover function for the FormatIns function of the
 *                 disassembler
 */
void MiscFormatIns( char *buf, instruction *ins, form_option optn, DisAsmRtns *rtns )
{
    if( CurRtns != NULL ) {
        MessageBox( NULL, "Disassembly Error!!", "", MB_OK );
        return;
    }
    CurRtns = rtns;
    FormatIns( buf, ins, optn );
    CurRtns = NULL;

} /* MiscFormatIns */

/*
 * Cover Functions for disassembler interface routines
 */

/*
 * GetDataByte
 */
int_16 GetDataByte( void )
{
    return( CurRtns->GetDataByte() );

} /* GetDataByte */

/*
 * GetDataWord
 */
int_16 GetDataWord( void )
{
    return( CurRtns->GetDataWord() );

} /* GetDataWord */

/*
 * GetNextByte
 */
int_16 GetNextByte( void )
{
    return( CurRtns->GetNextByte() );

} /* GetNextByte */

/*
 * GetDataLong
 */
long GetDataLong( void )
{
    return( CurRtns->GetDataLong() );

} /* GetDataLong */

/*
 * EndOfSegment
 */
char EndOfSegment( void )
{
    return( CurRtns->EndOfSegment() );

} /* EndOfSegment */

/*
 * GetOffset
 */
DWORD GetOffset( void )
{
    return( CurRtns->GetOffset() );

} /* GetOffset */

/*
 * ToStr
 */
char *ToStr( unsigned long value, uint_16 len, DWORD addr )
{
    return( CurRtns->ToStr( value, len, addr ) );

} /* ToStr */

/*
 * JmpLabel
 */
char *JmpLabel( unsigned long addr, DWORD off )
{
    return( CurRtns->JmpLabel( addr, off ) );

} /* JmpLabel */

/*
 * ToBrStr
 */
char *ToBrStr( unsigned long value, DWORD addr )
{
    return( CurRtns->ToBrStr( value, addr ) );

} /* ToBrStr */

/*
 * ToIndex
 */
char *ToIndex( unsigned long value, unsigned long addr )
{
    return( CurRtns->ToIndex( value, addr ) );

} /* ToIndex */

/*
 * ToSegStr
 */
char *ToSegStr( DWORD value, WORD seg, DWORD addr )
{
    return( CurRtns->ToSegStr( value, seg, addr ) );

} /* ToSegStr */

/*
 * GetWtkInsName
 */
char *GetWtkInsName( unsigned ins )
{
    return( CurRtns->GetWtkInsName( ins ) );

} /* GetWtkInsName */

/*
 * DoWtk
 */
void DoWtk( void )
{
    CurRtns->DoWtk();

} /* DoWtk */

/*
 * IsWtk
 */
int IsWtk( void )
{
    return( CurRtns->IsWtk() );

} /* IsWtk */
