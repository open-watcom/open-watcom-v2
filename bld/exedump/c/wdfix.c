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
* Description:  NE/LE/LX fixups dump routines.
*
****************************************************************************/


#include <stdio.h>
#include <setjmp.h>

#include "wdglb.h"
#include "wdfunc.h"


/*
 * Dump the internal reference table
 */
static unsigned_32 internal_ref( unsigned_8 flags, unsigned_8 src, unsigned_32 off )
/**********************************************************************************/
{
    Wdputs( "   object #    = " );
    if( flags & OSF_OBJ_ORD ) {
        unsigned_16 object;
        Wread( &object, sizeof( object ) );
        Puthex( object, 2 * sizeof( object ) );
        off += sizeof( object );
    } else {
        unsigned_8 object;
        Wread( &object, sizeof( object ) );
        Puthex( object, 2 * sizeof( object ) );
        off += sizeof( object );
    }
    Wdputs( "   target off       = " );
    if( (src & 0xf) != SELECTOR_FIX ) {
        if( flags & OSF_TARGET_OFF ) {
            unsigned_32 taroff;
            Wread( &taroff, sizeof( taroff ) );
            Puthex( taroff, 2 * sizeof( taroff ) );
            off += sizeof( taroff );
        } else {
            unsigned_16 taroff;
            Wread( &taroff, sizeof( taroff ) );
            Puthex( taroff, 2 * sizeof( taroff ) );
            off += sizeof( taroff );
        }
    }
    return( off );
}

/*
 * Dump the imported reference by ordinal table
 */
static unsigned_32 imp_ord_ref( unsigned_8 flags, unsigned_32 off )
/*****************************************************************/
{
    Wdputs( "   mod ord #   = " );
    if( flags & OSF_OBJ_ORD ) {
        unsigned_16 object;
        Wread( &object, sizeof( object ) );
        Puthex( object, 2 * sizeof( object ) );
        off += sizeof( object );
    } else {
        unsigned_8  object;
        Wread( &object, sizeof( object ) );
        Puthex( object, 2 * sizeof( object ) );
        off += sizeof( object );
    }
    Wdputs( "   import ord #     = " );
    if( flags & OSF_ORD_FLAG ) {
        unsigned_8  add_ord;
        Wread( &add_ord, sizeof( add_ord ) );
        Puthex( add_ord, 2 * sizeof( add_ord ) );
        off += sizeof( add_ord );
    } else if( flags & OSF_TARGET_OFF ) {
        unsigned_32 add_ord;
        Wread( &add_ord, sizeof( add_ord ) );
        Puthex( add_ord, 2 * sizeof( add_ord ) );
        off += sizeof( add_ord );
    } else {
        unsigned_16 add_ord;
        Wread( &add_ord, sizeof( add_ord ) );
        Puthex( add_ord, 2 * sizeof( add_ord ) );
        off += sizeof( add_ord );
    }
    if( flags & OSF_ADD_FIX ) {
        Wdputslc( "\n          additive = " );
        if( flags & OSF_ADD_FIX_32 ) {
            unsigned_32 add_ord;
            Wread( &add_ord, sizeof( add_ord ) );
            Puthex( add_ord, 2 * sizeof( add_ord ) );
            off += sizeof( add_ord );
        } else {
            unsigned_16 add_ord;
            Wread( &add_ord, sizeof( add_ord ) );
            Puthex( add_ord, 2 * sizeof( add_ord ) );
            off += sizeof( add_ord );
        }
    }
    return( off );
}

/*
 * Dump the import reference by name table
 */
static unsigned_32 imp_name_ref( unsigned_8 flags, unsigned_32 off )
/******************************************************************/
{
    Wdputs( "   mod ord #   = " );
    if( flags & OSF_OBJ_ORD ) {
        unsigned_16 object;
        Wread( &object, sizeof( object ) );
        Puthex( object, 2 * sizeof( object ) );
        off += sizeof( object );
    } else {
        unsigned_8  object;
        Wread( &object, sizeof( object ) );
        Puthex( object, 2 * sizeof( object ) );
        off += sizeof( object );
    }
    Wdputs( "   proc name offset = " );
    if( flags & OSF_TARGET_OFF ) {
        unsigned_32 add_off;
        Wread( &add_off, sizeof( add_off ) );
        Puthex( add_off, 2 * sizeof( add_off ) );
        off += sizeof( add_off );
    } else {
        unsigned_16 add_off;
        Wread( &add_off, sizeof( add_off ) );
        Puthex( add_off, 2 * sizeof( add_off ) );
        off += sizeof( add_off );
    }
    if( flags & OSF_ADD_FIX ) {
        Wdputslc( "\n          additive = " );
        if( flags & OSF_ADD_FIX_32 ) {
            unsigned_32 add_off;
            Wread( &add_off, sizeof( add_off ) );
            Puthex( add_off, 2 * sizeof( add_off ) );
            off += sizeof( add_off );
        } else {
            unsigned_16 add_off;
            Wread( &add_off, sizeof( add_off ) );
            Puthex( add_off, 2 * sizeof( add_off ) );
            off += sizeof( add_off );
        }
    }
    return( off );
}

/*
 * Dump the internal reference via entry table
 */
static unsigned_32 int_ent_ref( unsigned_8 flags, unsigned_32 off )
/*****************************************************************/
{
    Wdputs( "   entry ord # = " );
    if( flags & OSF_OBJ_ORD ) {
        unsigned_16 object;
        Wread( &object, sizeof( object ) );
        Puthex( object, 2 * sizeof( object ) );
        off += sizeof( object );
    } else {
        unsigned_8  object;
        Wread( &object, sizeof( object ) );
        Puthex( object, 2 * sizeof( object ) );
        off += sizeof( object );
    }
    if( flags & OSF_ADD_FIX ) {
        Wdputs( "   additive         = " );
        if( flags & OSF_ADD_FIX_32 ) {
            unsigned_32 addfix;
            Wread( &addfix, sizeof( addfix ) );
            Puthex( addfix, 2 * sizeof( addfix ) );
            off += sizeof( addfix );
        } else {
            unsigned_16 addfix;
            Wread( &addfix, sizeof( addfix ) );
            Puthex( addfix, 2 * sizeof( addfix ) );
            off += sizeof( addfix );
        }
    }
    return( off );
}

/*
 * Dump the fixup page table
 */
void Dmp_fixpage_tab( unsigned_32 fix_off, unsigned_32 fix_size )
/***************************************************************/
{
    unsigned_32     offset;
    unsigned_32     i;

    Wlseek( fix_off );
    Wdputslc( "\n" );
    Banner( "Fixup Page Table" );
    for( i = 0; i * sizeof( offset ) < fix_size; i++ ) {
        if( i != 0 ) {
            if( (i) % 4 == 0 ) {
                Wdputslc( "\n" );
            } else {
                Wdputs( "     " );
            }
        }
        Putdecl( i, 3 );
        Wdputc( ':' );
        Wread( &offset, sizeof( offset ) );
        Puthex( offset, 2 * sizeof( offset ) );
    }
    Wdputslc( "\n" );
}

/*
 * Dump the fixup record table
 */
void Dmp_fixrec_tab( unsigned_32 fix_off )
/****************************************/
{
    unsigned_16     srcoff;
    unsigned_8      cnt;
    unsigned_8      source;
    unsigned_8      flags;
    unsigned_8      i;

    if( !(Options_dmp & FIX_DMP) ) {
        return;
    }
    Wdputslc( "\n" );
    Banner( "Fixup Record Table" );
    Wdputslc( "Source  Target\n" );
    Wdputslc( "  type  flags\n" );
    Wdputslc( "  ====  ====\n" );
    for( ; fix_off < Os2_386_head.impmod_off + New_exe_off; ) {
        cnt = 0;
        Wlseek( fix_off );
        Wread( &source, sizeof( source ) );
        Wread( &flags, sizeof( flags ) );
        fix_off += sizeof( source ) + sizeof( flags );
        Wdputs( "   " );
        Puthex( source, 2 * sizeof( source ) );
        Wdputs( "    " );
        Puthex( flags, 2 * sizeof( flags ) );
        if( source & OSF_SOURCE_LIST ) {
            Wread( &cnt, sizeof( cnt ) );
            Wdputs( "   count   = " );
            Puthex( cnt, 2 * sizeof( cnt ) );
            Wdputs( "  " );
            fix_off += sizeof( cnt );
        } else {
            Wread( &srcoff, sizeof( srcoff ) );
            Wdputs( "   src off = " );
            Puthex( srcoff, 2 * sizeof( srcoff ) );
            fix_off += sizeof( srcoff );
        }
        if( (flags & 0x03) == OSF_INTERNAL_REF ) {
            fix_off = internal_ref( flags, source, fix_off );
        } else if( (flags & 0x03) == OSF_IMP_ORD_REF ) {
            fix_off = imp_ord_ref( flags, fix_off );
        } else if( (flags & 0x03) == OSF_IMP_NAME_REF ) {
            fix_off = imp_name_ref( flags, fix_off );
        } else if( (flags & 0x03) == OSF_INT_ENT_REF ) {
            fix_off = int_ent_ref( flags, fix_off );
        }
        if( cnt != 0 ) {
            Wdputslc( "\n" );
            Wdputs( "source offsets = " );
            for( i = 0; i < cnt; i++ ) {
                if( i ) {
                    if( (i) % 9 == 0 ) {
                        Wdputslc( "\n                 " );
                    } else {
                        Wdputs( "   " );
                    }
                }
                Wread( &srcoff, sizeof( srcoff ) );
                Puthex( srcoff, 2 * sizeof( srcoff ) );
            }
        }
        fix_off += cnt * sizeof( srcoff );
        Wdputslc( "\n" );
    }
}
