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


#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "disasm.h"
#include "wdismsg.h"


char  *FindExpName( uint_32 addr, segment *seg )
/**********************************************/
{
    export_sym          *exp;
    uint_32             expaddr;

    exp = seg->exports_rover;
    if( exp == NULL || exp->address > addr ) {
        exp = seg->exports;
    }
    while( exp != NULL ) {
        expaddr = exp->address;
        if( expaddr > addr ) {
            return( NULL );
        }
        if( expaddr == addr ) {
            seg->exports_rover = exp;
            return( FormSym( exp->name ) );
        }
        exp = exp->next_exp;
    }
    return( NULL );
}


char  *FindLabel( uint_32 ref, uint_32 addr, segment *seg )
/*********************************************************/
{
    char                *name;
    fixup               *fix;

    switch( _Class( seg ) ) {
    case TYPE_IMPORT:               /* fall through */
    case TYPE_COMDEF:
        name = FormSym( _Name( seg ) );
        break;
    case TYPE_GROUP:
        name = _Name( seg );
        break;
    default:
        if( ref == BAD_OFFSET ) {
            fix = NULL;
        } else {
            fix = FindFixup( ref, seg );
        }
        if( fix == NULL ) {
            name = FindExpName( addr, seg );
            if( name == NULL ) {
                AddLabel( addr, NULL, seg, FALSE, FALSE );
            }
        } else {
            name = GetFixName( fix );
            if( name == NULL ) {
                AddLabel( fix->imp_address, NULL, seg, FALSE, FALSE );
                name = FindExpName( fix->imp_address, seg );
            }
        }
        break;
    }
    return( name );
}


fixup  *FindFixup( uint_32 addr, segment *seg )
/*********************************************/
{
    fixup               *fix;
    uint_32             fixaddr;

    fix = seg->fixes_rover;
    if( fix == NULL || fix->address > addr ) {
        fix = seg->fixes;
    }

    while( fix != NULL ) {
        fixaddr = fix->address;
        if( fixaddr == addr ) {
            seg->fixes_rover = fix;
            return( fix );
        }
        if( fixaddr > addr ) {
            return( NULL );
        }
        fix = fix->next_fix;
    }
    return( fix );
}


char  *FindSymbol( uint_32 ref )
/******************************/
{
    fixup               *fix;
    char                *name;

    name = NULL;
    fix = FindFixup( ref, Segment );
    if( fix != NULL ) {
        name = GetFixName( fix );
        if( name == NULL ) {
            name = NewName( fix );
        }
    }
    return( name );
}


void  FindSrcLine( uint_32 addr )
/*******************************/
{
    line_num            *line;
    line_num            *check;
    unsigned            next;
    int                 len;
    char                txt_line[ MAX_LINE_LEN + 1 ];

    line = Mod->src_rover;
    if( line == NULL ) return;
    if( line->seg != Segment ) {
        if( Segment->src_done ) return;
        check = line;
        for( ;; ) {
           check = check->next_num;
           if( check == NULL ) {
               Segment->src_done = TRUE;
               return;
           }
           if( check->seg == Segment ) break;
        }
        FlipToSeg( line->seg );
        return;
    }
    if( line->address > addr ) return;

    if( addr == Segment->start && LastNum <= 1 ) {
        if( line->next_num == NULL ) {
            next = UINT_MAX;
        } else {
            next = line->next_num->num;
        }
    } else if( addr == Segment->size ) {
        for( ;; ) {
            if( line == NULL ) break;
            if( line->seg != Segment ) break;
            line = line->next_num;
        }
    } else {
        next = LastNum;
        for( ;; ) {
            if( line == NULL ) return;
            if( line->seg != Segment ) return;
            if( line->address == addr ) break;
            line = line->next_num;
        }
        for( ;; ) {
            line = line->next_num;
            if( line == NULL ) {
                next = UINT_MAX;
                break;
            }
            if( line->num < next ) continue;
            next = line->num;
            if( line->seg != Segment ) break;
            if( line->address > addr ) break;
        }
    }
    Mod->src_rover = line;
    EmitNL();
    for( ;; ) {
        if( LastNum + 1 >= next ) return;
        len = FGetTxtRec( Source, txt_line, MAX_LINE_LEN );
        txt_line[ len ] = NULLCHAR;
        if( feof( Source ) ) break;
        if( Options & FORM_ASSEMBLER ) {
            if( DO_UNIX ) {
                Emit( "/ " );
            } else {
                Emit( "; " );
            }
        }
        EmitLine( txt_line );
        ++LastNum;
    }
    if( next != UINT_MAX ) {
        DoEmitError( ERR_EOF_ENCOUNTERED );
    }
}
