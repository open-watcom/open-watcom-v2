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


#include "cvars.h"
#include <stddef.h>
#include "cgstd.h"


void MacroAdd( MEPTR mentry, char *buf, int len, enum macro_flags flags )
{
    unsigned    size;

    size = offsetof(MEDEFN,macro_name) + strlen( mentry->macro_name ) + 1;
    if( len != 0 ) {                    // if not a special macro
        mentry->macro_defn = size;
    }
    mentry->macro_len = size + len;
    MacroOverflow( size + len, 0 );
    MacroCopy( (char *)mentry, MacroOffset, size );     /* 01-apr-94 */
    if( len != 0 ) {
        MacroCopy( buf, MacroOffset + size, len );
    }
    MacLkAdd( mentry, size + len, flags );
    CMemFree( mentry );
}


void AllocMacroSegment( unsigned minimum )
{
    struct macro_seg_list *msl;

    minimum = minimum;
    MacroSegment = FEmalloc( 0x8000 );
    MacroOffset = MacroSegment;
    MacroLimit = MacroOffset + 0x7FFE;
    if( MacroSegment == 0 ) {                   /* 16-aug-93 */
        CErr1( ERR_OUT_OF_MACRO_MEMORY );
        CSuicide();
    }
    msl = CMemAlloc( sizeof( struct macro_seg_list ) );
    msl->segment = MacroSegment;
    msl->next = MacSegList;
    MacSegList = msl;
}


void FreeMacroSegments()
{
    struct macro_seg_list *msl;

    for( ; msl = MacSegList; ) {
        FEfree( (void *)(msl->segment) );
        MacSegList = msl->next;
        CMemFree( msl );
    }
}


void MacroCopy( MPTR_T mptr, MACADDR_T offset, unsigned amount )
{
    memcpy( offset, mptr, amount );
}


void MacroOverflow( unsigned amount_needed, unsigned amount_used )
{
    MACADDR_T old_seg;
    MACADDR_T old_offset;

    amount_needed = _RoundUp( amount_needed, sizeof(int) );
    if( MacroLimit - MacroOffset < amount_needed ) {
        old_seg = MacroSegment;
        old_offset = MacroOffset;
        AllocMacroSegment( amount_needed );
        MacroCopy( old_offset, MacroOffset, amount_used );
    }
}


local MEPTR *MacroLkUp( char *name, MEPTR *lnk )
{
    int         len;
    MEPTR       mentry;

    len = strlen( name ) + 1;
    while( (mentry = *lnk) != NULL ) {
        if( NameCmp( mentry->macro_name, name, len ) == 0 ) break;
        lnk = &mentry->next_macro;
    }
    return( lnk );
}


void MacLkAdd( MEPTR mentry, int len, enum macro_flags flags )
{
    MEPTR       old_mentry, *lnk;
    enum macro_flags  old_flags;

    MacroCopy( (char *)mentry, MacroOffset, offsetof(MEDEFN,macro_name) + 1 );
    mentry = (MEPTR)MacroOffset;
    CalcHash( mentry->macro_name, strlen( mentry->macro_name ) );
    lnk  = &MacHash[ MacHashValue ];
    lnk = MacroLkUp( mentry->macro_name, lnk );
    old_mentry = *lnk;
    if( old_mentry != NULL ) {
        old_flags = old_mentry->macro_flags;
        if( old_flags & MACRO_CAN_BE_REDEFINED ){//delete old entry
            *lnk = old_mentry->next_macro;
            old_mentry = NULL;
        } else if( MacroCompare( mentry, old_mentry ) != 0 ) {
            CErr2p( ERR_MACRO_DEFN_NOT_IDENTICAL, mentry->macro_name );
        }
    }
    if( old_mentry == NULL ) {  //add new entry
        ++MacroCount;
        mentry->next_macro = MacHash[ MacHashValue ];
        MacHash[ MacHashValue ] = mentry;
        MacroOffset += _RoundUp( len, sizeof(int) );
        mentry->macro_flags = InitialMacroFlag | flags;
    }
}

SYM_HASHPTR SymHashAlloc( unsigned amount )
{
    SYM_HASHPTR hsym;

    amount = _RoundUp( amount, sizeof(int) );
    if( MacroLimit - MacroOffset < amount ) {
        AllocMacroSegment( amount );
    }
    hsym = (SYM_HASHPTR) MacroOffset;
    MacroOffset += amount;
    return( hsym );
}


int MacroCompare( MEPTR m1, MEPTR m2 )
{
    char        *p1;
    char        *p2;

    if( m1->macro_len  != m2->macro_len )   return( -1 );
    if( m1->macro_defn != m2->macro_defn )  return( -1 );
    if( m1->parm_count != m2->parm_count )  return( -1 );
    p1 = (char *)m1 + offsetof(MEDEFN,macro_name);
    p2 = (char *)m2 + offsetof(MEDEFN,macro_name);
    return( memcmp( p1, p2, m1->macro_len - offsetof(MEDEFN,macro_name) ) );
}


MEPTR MacroLookup()
{
    MEPTR       mentry, *lnk;

    lnk = MacroLkUp( Buffer, &MacHash[ MacHashValue ] );
    mentry = *lnk;
    return( mentry );
}
