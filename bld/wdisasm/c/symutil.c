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



#include <string.h>
#include "disasm.h"
#include "wdismsg.h"

typedef struct fix_list {
    fixup               *fix;
    struct fix_list     *next_fix;
} fix_list;


static  fix_list        *FixList;


void  DumpImpList()
/*****************/

{
    fixup               *fix;
    info                *prev_targ;
    bool                head;
    int                 count;
    fix_list            *list;

    if( ( Pass != 1 ) && ( ImpDump ) ) {
        BuildFixList();
        list = FixList;
        prev_targ = NULL;
        head = false;
        count = 0;
        while( list != NULL ) {
            fix = list->fix;
            switch( _Class( fix->target ) ) {
            case TYPE_IMPORT:
                if( !head ) {
                    DoEmit( MSG_LIST_EXTERN_SYM );
                    EmitNL();
                    EmitNL();
                    DoEmit( MSG_SYMBOL_ELIST );
                    EmitNL();
                    EmitDashes( LABEL_LEN );
                    head = true;
                }
                if( prev_targ != fix->target ) {
                    prev_targ = fix->target;
                    if( count > 0 ) {
                        EmitNL();
                        count = 0;
                    }
                    if( EmitSym( prev_targ->name, LABEL_LEN ) ) {
                        EmitNL();
                        EmitSpaced( NULL, LABEL_LEN );
                    }
                } else {
                    if( count == 0 ) {
                        EmitSpaced( NULL, LABEL_LEN );
                    }
                }
                EmitAddr( fix->address, WORD_SIZE, NULL );
                if( ++count == IMPORT_LIST ) {
                    EmitNL();
                    count = 0;
                }
                break;
            default:
                break;
            }
            list = list->next_fix;
        }
        FreeSymTranslations();
        if( count > 0 ) {
            EmitNL();
        }
    }
}


static void BuildFixList()
/************************/

/* Since fixups now sorted by address, build list of fixups sorted
   old way (i.e. by target and alphabetically ) */

{
    import_sym          *targ;
    import_sym          *new_targ;
    fixup               *fix;
    fixup               *new_fix;
    fix_list            *list;
    fix_list            *prev_list;
    fix_list            *new_list;

    FixList = NULL;
    new_fix = Segment->fixes;
    while( new_fix != NULL ) {

        list = FixList;
        prev_list = NULL;
        while( list != NULL ) {
            fix = list->fix;
            targ = fix->target;
            new_targ = new_fix->target;
            if( targ == new_targ ) break;
            if( ImportClass( targ->class )
                && ImportClass( new_targ->class )
                && stricmp( targ->name, new_targ->name ) > 0 ) break;
            prev_list = list;
            list = list->next_fix;
        }
        new_list = AllocMem( sizeof( fix_list ) );
        new_list->fix = new_fix;
        new_list->next_fix = list;
        if( prev_list == NULL ) {
            FixList = new_list;
        } else {
            prev_list->next_fix = new_list;
        }

        new_fix = new_fix->next_fix;
    }
}


export_sym *AddLabel( uint_32 addr, char *name, segment *seg, bool pub, bool hidden )
/***********************************************************************************/
{
    export_sym          *new_exp;
    export_sym          *exp;
    export_sym          *prev_exp;
    uint_32             expaddr;

    prev_exp = seg->exports_rover;
    if( prev_exp != NULL ) {
        exp = prev_exp->next_exp;
        if( exp == NULL || exp->address >= addr ) {
            prev_exp = NULL;
            exp = seg->exports;
        }
    } else {
        /* remember prev_exp is NULL */
        exp = seg->exports;
    }
    while( exp != NULL ) {
        expaddr = exp->address;
        if( expaddr > addr ) break;
        if( expaddr == addr && !pub ) {
            seg->exports_rover = prev_exp;
            return( exp );
        }
        prev_exp = exp;
        exp = exp->next_exp;
    }
    new_exp = AllocMem( sizeof( export_sym ) );
    new_exp->name = name;
    new_exp->address = addr;
    new_exp->next_exp = NULL;
    new_exp->type_id = 0;
    new_exp->public = pub;
    new_exp->segment = seg;
    new_exp->next_exp = exp;
    if( UseORL ) {
        new_exp->hidden = hidden;       // only ORL uses it currently
    }
    if( prev_exp == NULL ) {
        seg->exports = new_exp;
    } else {
        prev_exp->next_exp = new_exp;
    }
    seg->exports_rover = new_exp;
    return( new_exp );
}


char  *NewName( fixup *fix )
/**************************/
{
    int                 len;
    char                *name;

    SegName();
    len = strlen( NameBuff );
    if( len != 0 ) {
        NameBuff[ len ] = ':';
        NameBuff[ len + 1 ] = '\0';
    }
    if( _Class( fix->target ) == TYPE_GROUP ) {
        name = _Name( fix->target );
    } else {
        AddLabel( fix->imp_address, NULL, fix->target, false, false );
        name = FindExpName( fix->imp_address, fix->target );
    }
    if( name != NULL ) {
        strcat( NameBuff, name );
    }
    return( NameBuff );
}
