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


MEPTR CreateMEntry( const char *name )
{
    MEPTR   mentry;
    int     macro_len;

    macro_len = strlen( name );
    if( macro_len == 0 )
        return( NULL );
    macro_len += sizeof( MEDEFN );
    mentry = (MEPTR)CMemAlloc( macro_len );
    strcpy( mentry->macro_name, name );
    mentry->macro_len = macro_len;
    mentry->parm_count = 0;
    mentry->macro_defn = 0; /* indicate special macro */
    return( mentry );
}

void FreeMEntry( MEPTR mentry )
{
    CMemFree( mentry );
}

void MacroAdd( MEPTR mentry, char *buf, int len, macro_flags mflags )
{
    unsigned    size;

    size = mentry->macro_len;
    if( len != 0 ) {                // if not a special macro
        mentry->macro_defn = size;
    }
    mentry->macro_len += len;
    MacroOverflow( size + len, 0 );
    MacroCopy( mentry, MacroOffset, size );
    if( len != 0 ) {
        MacroCopy( buf, MacroOffset + size, len );
    }
    MacLkAdd( mentry, size + len, mflags );
}


void AllocMacroSegment( unsigned minimum )
{
    struct macro_seg_list *msl;
    unsigned amount;

    amount = _RoundUp( minimum, 0x8000 );
    MacroSegment = FEmalloc( amount );
    MacroOffset = MacroSegment;
    MacroLimit = MacroOffset + amount - 2;
    if( MacroSegment == 0 ) {
        CErr1( ERR_OUT_OF_MACRO_MEMORY );
        CSuicide();
    }
    msl = CMemAlloc( sizeof( struct macro_seg_list ) );
    msl->macro_seg = MacroSegment;
    msl->next = MacSegList;
    MacSegList = msl;
}


void FreeMacroSegments( void )
{
    struct macro_seg_list *msl;

    for( ; (msl = MacSegList) != NULL; ) {
        FEfree( (void *)(msl->macro_seg) );
        MacSegList = msl->next;
        CMemFree( msl );
    }
}


void MacroCopy( void *mptr, MACADDR_T offset, unsigned amount )
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


local MEPTR *MacroLkUp( const char *name, MEPTR *lnk )
{
    int         len;
    MEPTR       mentry;

    len = strlen( name ) + 1;
    while( (mentry = *lnk) != NULL ) {
        if( NameCmp( mentry->macro_name, name, len ) == 0 )
            break;
        lnk = &mentry->next_macro;
    }
    return( lnk );
}


void MacLkAdd( MEPTR mentry, int len, macro_flags mflags )
{
    MEPTR       old_mentry, *lnk;
    macro_flags old_mflags;

    MacroCopy( mentry, MacroOffset, offsetof(MEDEFN,macro_name) );
    mentry = (MEPTR)MacroOffset;
    CalcHash( mentry->macro_name, strlen( mentry->macro_name ) );
    lnk  = &MacHash[ MacHashValue ];
    lnk = MacroLkUp( mentry->macro_name, lnk );
    old_mentry = *lnk;
    if( old_mentry != NULL ) {
        old_mflags = old_mentry->macro_flags;
        if( old_mflags & MFLAG_CAN_BE_REDEFINED ){//delete old entry
            *lnk = old_mentry->next_macro;
            old_mentry = NULL;
        } else if( MacroCompare( mentry, old_mentry ) != 0 ) {
            if( old_mentry->macro_defn ) {
                SetDiagMacro( old_mentry );
            }
            CErr2p( ERR_MACRO_DEFN_NOT_IDENTICAL, mentry->macro_name );
            if( old_mentry->macro_defn ) {
                SetDiagPop();
            }
        }
    }
    if( old_mentry == NULL ) {  //add new entry
        ++MacroCount;
        mentry->next_macro = MacHash[ MacHashValue ];
        MacHash[ MacHashValue ] = mentry;
        MacroOffset += _RoundUp( len, sizeof(int) );
        mentry->macro_flags = InitialMacroFlag | mflags;
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
    if( m1->macro_len != m2->macro_len )
        return( -1 );
    if( m1->macro_defn != m2->macro_defn )
        return( -1 );
    if( m1->parm_count != m2->parm_count )
        return( -1 );
    return( memcmp( m1->macro_name, m2->macro_name, m1->macro_len - offsetof(MEDEFN,macro_name) ) );
}


MEPTR MacroLookup( const char *buf )
{
    MEPTR       mentry, *lnk;

    lnk = MacroLkUp( buf, &MacHash[ MacHashValue ] );
    mentry = *lnk;
    return( mentry );
}
