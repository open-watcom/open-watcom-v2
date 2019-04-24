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


#define macroSizeAlign(x)   _RoundUp( (x), sizeof( int ) )

MEPTR CreateMEntryH( const char *name, size_t len )
{
    MEPTR   mentry;
    size_t  size;

    if( len == 0 ) {
        len = strlen( name );
    }
    size = offsetof( MEDEFN, macro_name ) + len + 1;
    mentry = (MEPTR)CMemAlloc( size );
    memcpy( mentry->macro_name, name, len );
    mentry->macro_name[len] = '\0';
    mentry->macro_len = size;
    mentry->parm_count = 0;
    mentry->macro_defn = 0; /* indicate special macro */
    return( mentry );
}

void FreeMEntryH( MEPTR mentry )
{
    CMemFree( mentry );
}

MEPTR CreateMEntry( const char *name, size_t len )
{
    MEPTR   mentry;
    size_t  size;

    if( len == 0 ) {
        len = strlen( name );
    }
    size = offsetof( MEDEFN, macro_name ) + len + 1;
    MacroReallocOverflow( size, 0 );
    mentry = (MEPTR)MacroOffset;
    memcpy( mentry->macro_name, name, len );
    mentry->macro_name[len] = '\0';
    mentry->macro_len = size;
    mentry->macro_defn = 0;
    mentry->parm_count = 0;
    mentry->src_loc.fno = 0;
    mentry->src_loc.line = 0;
    return( mentry );
}

void MacroAdd( MEPTR mentry, const char *buf, size_t len, macro_flags mflags )
{
    size_t      size;

    size = mentry->macro_len;
    if( len > 0 ) {                // if not a special macro
        mentry->macro_defn = size;
    }
    mentry->macro_len += len;
    MacroReallocOverflow( size + len, 0 );
    MacroCopy( mentry, MacroOffset, size );
    if( len > 0 ) {
        MacroCopy( buf, MacroOffset + size, len );
    }
    MacroDefineH( mentry, size + len, mflags );
}


void *MacroAllocateInSeg( size_t size )
{
    void *retn;

    retn = MacroOffset;
    size = macroSizeAlign( size );
    MacroOffset += size;
    MacroSegmentLimit -= size;
    return( retn );
}


void AllocMacroSegment( size_t minimum )
{
    struct macro_seg_list *msl;
    size_t amount;

    amount = _RoundUp( minimum, 0x8000 );
    MacroSegment = FEmalloc( amount );
    MacroOffset = MacroSegment;
    MacroSegmentLimit = amount - 2;
    if( MacroSegment == NULL ) {
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


void MacroCopy( const void *mptr, MACADDR_T offset, size_t amount )
{
    memcpy( offset, mptr, amount );
}


void MacroReallocOverflow( size_t amount_needed, size_t amount_used )
{
    MACADDR_T old_offset;

    amount_needed = macroSizeAlign( amount_needed );
    if( amount_needed > MacroSegmentLimit ) {
        old_offset = MacroOffset;
        AllocMacroSegment( amount_needed );
        if( amount_used > 0 ) {
            MacroCopy( old_offset, MacroOffset, amount_used );
        }
    }
}


static MEPTR *MacroLkUp( const char *name, MEPTR *lnk )
{
    size_t      len;
    MEPTR       mentry;

    len = strlen( name ) + 1;
    while( (mentry = *lnk) != NULL ) {
        if( NameCmp( mentry->macro_name, name, len ) == 0 )
            break;
        lnk = &mentry->next_macro;
    }
    return( lnk );
}


MEPTR MacroDefineH( MEPTR mentry, size_t mlen, macro_flags mflags )
{
    MEPTR       old_mentry;
    MEPTR       *lnk;
    MEPTR       new_mentry;
    macro_flags old_mflags;

    new_mentry = NULL;
    MacroCopy( mentry, MacroOffset, offsetof(MEDEFN,macro_name) );
    mentry = (MEPTR)MacroOffset;
    CalcHash( mentry->macro_name, strlen( mentry->macro_name ) );
    lnk = &MacHash[MacHashValue];
    lnk = MacroLkUp( mentry->macro_name, lnk );
    old_mentry = *lnk;
    if( old_mentry != NULL ) {
        old_mflags = old_mentry->macro_flags;
        if( old_mflags & MFLAG_CAN_BE_REDEFINED ) {//delete old entry
            *lnk = old_mentry->next_macro;
            old_mentry = NULL;
        } else if( MacroCompare( mentry, old_mentry ) != 0 ) {
            if( !MacroIsSpecial( old_mentry ) ) {
                SetDiagMacro( old_mentry );
            }
            CErr2p( ERR_MACRO_DEFN_NOT_IDENTICAL, mentry->macro_name );
            if( !MacroIsSpecial( old_mentry ) ) {
                SetDiagPop();
            }
        }
    }
    if( old_mentry == NULL ) {  //add new entry
        ++MacroCount;
        mentry->next_macro = MacHash[MacHashValue];
        MacHash[MacHashValue] = mentry;
        mentry->macro_flags = InitialMacroFlags | mflags;
        new_mentry = MacroAllocateInSeg( mlen );
    }
    return( new_mentry );
}

MEPTR MacroDefine( size_t mlen, macro_flags mflags )
{
    MEPTR       old_mentry;
    MEPTR       *lnk;
    MEPTR       new_mentry;
    macro_flags old_mflags;
    MEPTR       mentry;
    const char  *mname;

    new_mentry = NULL;
    mentry = (MEPTR)MacroOffset;
    mentry->macro_len = mlen;
    mname = mentry->macro_name;
    CalcHash( mname, strlen( mname ) );
    lnk = &MacHash[MacHashValue];
    lnk = MacroLkUp( mname, lnk );
    old_mentry = *lnk;
    if( old_mentry != NULL ) {
        old_mflags = old_mentry->macro_flags;
        if( old_mflags & MFLAG_CAN_BE_REDEFINED ) {//delete old entry
            *lnk = old_mentry->next_macro;
            old_mentry = NULL;
        } else if( MacroCompare( mentry, old_mentry ) != 0 ) {
            if( !MacroIsSpecial( old_mentry ) ) {
                SetDiagMacro( old_mentry );
            }
            CErr2p( ERR_MACRO_DEFN_NOT_IDENTICAL, mname );
            if( !MacroIsSpecial( old_mentry ) ) {
                SetDiagPop();
            }
        }
    }
    if( old_mentry == NULL ) {  //add new entry
        ++MacroCount;
        new_mentry = MacroAllocateInSeg( mlen );
        new_mentry->macro_flags = InitialMacroFlags | mflags;
        new_mentry->next_macro = MacHash[MacHashValue];
        MacHash[MacHashValue] = new_mentry;
    }
    return( new_mentry );
}

SYM_HASHPTR SymHashAlloc( size_t amount )
{
    amount = macroSizeAlign( amount );
    if( amount > MacroSegmentLimit ) {
        AllocMacroSegment( amount );
    }
    return( MacroAllocateInSeg( amount ) );
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

    lnk = MacroLkUp( buf, &MacHash[MacHashValue] );
    mentry = *lnk;
    return( mentry );
}
