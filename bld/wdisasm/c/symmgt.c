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
#include <ctype.h>
#include "disasm.h"
#include "wdismsg.h"
#include "demangle.h"

static  int             Count;
static  char            SeenInitChar[256];

/*
 * Static function prototypes
 */
static  void            AddSyms( handle * );
static  void            AddToList( void );


void  InitSymList()
/*****************/

{
    segment     *seg;

    Count = 0;
    for( seg = Mod->segments; seg != NULL; seg = seg->next_segment ) {
        Segment = seg;
        AddToList();
    }
    for( seg = Mod->segments; seg != NULL; seg = seg->next_segment ) {
        Segment = seg;
        FixExpList();
    }
}


static  void  AddToList()
/***********************/

{
    symbol_list         *sym;
    symbol_list         *new_sym;
    symbol_list         **owner;
    export_sym          *exp;
    char                *name;
    char                first;

    for( exp = Segment->exports; exp != NULL; exp = exp->next_exp ) {
        name = exp->name;
        if( name != NULL ) {
            new_sym = (symbol_list *) AllocMem( sizeof( symbol_list ) );
            new_sym->name = name;
            first = toupper( *name );
            if( !SeenInitChar[ first ] ) {
                for( ;; ) {
                    ++name;
                    if( *name == '\0' ) {
                        SeenInitChar[ first ] = 1;
                    }
                    if( !isdigit( *name ) ) break;
                }
            }
            new_sym->next_sym = NULL;
            new_sym->address = exp->address;
            new_sym->frame = Segment->grouped;
            new_sym->target = Segment;
            owner = &Mod->symbols;
            for( ;; ) {
                sym = *owner;
                if( sym == NULL ) break;
                if( stricmp( sym->name, new_sym->name ) > 0 ) break;
                owner = &sym->next_sym;
            }
            new_sym->next_sym = sym;
            *owner = new_sym;
        }
    }
}

#define OUT_RANGE_IND '#'

static  void  FixExpList()
/************************/

{
    export_sym          *exp;
    unsigned            index;
    unsigned            ch;
    uint_32             addr;
    char                *seg_label;
    export_sym          *seg_exp;
    char                buff[ MAX_NAME_LEN ];
    char                *p;

    index = 0;
    for( ;; ) {
        if( index >= sizeof( SeenInitChar ) ) {
            /* couldn't find anything */
            NameBuff[ 0 ] = '.';
            break;
        }
        ch = toupper( (index + IntLblStart) % sizeof( SeenInitChar ) );
        if( !SeenInitChar[ ch ] && (isalpha( ch ) || ch == '_') ) {
            NameBuff[ 0 ] = ch;
            break;
        }
        ++index;
    }
    seg_label = NULL;
    exp = Segment->exports;
    while( exp != NULL ) {
        addr = exp->address;
        if( addr > Segment->size ) {
            if( seg_label == NULL ) {
                seg_exp = Segment->exports;
                if( seg_exp->address == 0 ) {
                    seg_label = seg_exp->name;
                } else {
                    itoa( ++Count, &NameBuff[ 1 ], 10 );
                    seg_label = (char *) AllocMem( strlen( NameBuff ) + 1 );
                    strcpy( seg_label, NameBuff );
                    seg_exp = AllocMem( sizeof( export_sym ) );
                    seg_exp->name = seg_label;
                    seg_exp->address = 0;
                    seg_exp->type_id = 0;
                    seg_exp->public = FALSE;
                    seg_exp->dumped = FALSE;
                    seg_exp->segment = Segment;
                    seg_exp->next_exp = Segment->exports;
                    Segment->exports = seg_exp;
                }
            }
            exp->dumped = TRUE; /* don't want these to come out */
            /* if 16-bit segment, sign extend address to look nice */
            if( !Segment->use_32 ) addr = (signed short)addr;
            buff[0]=OUT_RANGE_IND;
            MyIToHS( &buff[ 1 ], addr );
            p = &buff[ strlen( buff ) ];
            *p++ = OUT_RANGE_IND;
            strcpy( p, seg_label );
            exp->name = (char *) AllocMem( strlen( buff ) + 1 );
            strcpy( exp->name, buff );
        } else {
            exp->dumped = FALSE;    /* label has not been dumped to file */
            if( exp->name == NULL ) {
                itoa( ++Count, &NameBuff[ 1 ], 10 );
                exp->name = (char *) AllocMem( strlen( NameBuff ) + 1 );
                strcpy( exp->name, NameBuff );
            }
        }
        exp = exp->next_exp;
    }
}


void  DumpSymList()
/*****************/

{
    symbol_list                 *sym;

    if( ( Pass != 1 ) && ExpDump ) {
        sym = Mod->symbols;
        if( sym != NULL ) {
            DoEmit( MSG_LIST_PUBLIC_SYM );
            EmitNL();
            EmitNL();
            DoEmitSpaced( MSG_SYMBOL_PLIST, LABEL_LEN );
            DoEmitSpaced( MSG_GROUP_PLIST, LABEL_LEN );
            DoEmitSpaced( MSG_SEGMENT_PLIST, LABEL_LEN + 1 );
            DoEmit( MSG_ADDRESS_PLIST );
            EmitNL();
            EmitDashes( LABEL_LEN * 3 + 9 );
            do {
                if( EmitSym( sym->name, LABEL_LEN ) ) {
                    EmitNL();
                    EmitSpaced( NULL, LABEL_LEN );
                }
                if( sym->frame != NULL ) {
                    EmitSpaced( _Name( sym->frame ), LABEL_LEN );
                } else {
                    EmitSpaced( NULL, LABEL_LEN );
                }
                EmitSpaced( _Name( sym->target ), LABEL_LEN );
                EmitAddr( sym->address, WORD_SIZE, NULL );
                EmitNL();
                sym = sym->next_sym;
            } while( sym != NULL );
            EmitNL();
            EmitDashes( LINE_LEN );
            EmitNL();
            FreeSymTranslations();
        }
    }
}


char  *GetFixName( fixup *fix )
/*****************************/
{
    char                *name;
    symbol_list         *sym;
    segment             *target;
    int                 len;
    uint_32             addr;

    sym = Mod->symbols;
    name = NULL;
    target = fix->target;
    addr = fix->imp_address + ( fix->seg_address << 4 );
    if( target != NULL ) {
        if( !ImportClass( _Class( target ) ) ) {
            while( sym != NULL ) {
                if( sym->address == addr ) {
                    if( ( sym->target == target ) ) {
                        if( ( sym->frame == fix->frame ) ||
                            ( sym->frame == NULL ) ||
                            ( sym->frame == target ) ) {
                                name = sym->name;
                                break;
                        }
                    }
                }
                sym = sym->next_sym;
            }
            if( name != NULL ) {
                SegName();
                len = strlen( NameBuff );
                if( len != 0 ) {
                    NameBuff[ len ] = ':';
                    NameBuff[ len + 1 ] = '\0';
                }
                strcat( NameBuff, name );
                name = NameBuff;
            }
        } else {
            name = FormSym( _Name( target ) );
            if( ( fix->class & SEG_RELATIVE ) != 0 && addr != 0 ) {
                strcpy( NameBuff, name );
                len = strlen( NameBuff );
                MyIToHS( &NameBuff[ len ], addr );
                name = NameBuff;
            }
        }
    }
    return( name );
}

typedef struct translation {
    struct translation  *next;
    char                name[MAX_LINE_LEN+2];
} translation;

static translation      *SymTransHead;

char *FormSym( char *sym )
{
    char        *p;
    char        ch;
    int         quote;
    translation *sym_trans;
    char        *range;
    unsigned    range_len;

    if( sym == NULL ) return( NULL );   /* might happen on Pass 1 */
    if( sym[0] == OUT_RANGE_IND ) {
        range = &sym[1];
        do {
            ++sym;
        } while( sym[0] != OUT_RANGE_IND );
        range_len = sym - range;
        ++sym;
    } else {
        range_len = 0;
    }
    sym_trans = AllocMem( sizeof( translation ) );
    if( UnMangle && !(Options & FORM_ASSEMBLER) ) {
        __demangle_l( sym, 0, &sym_trans->name[1], MAX_LINE_LEN );
    } else {
        strcpy( &sym_trans->name[1], sym );
    }
    sym = &sym_trans->name[1];
    p = sym;
    if( Options & FORM_ASSEMBLER ) {
        /* put backquotes around symbol if it has strange characters */
        quote = FALSE;
        for( ;; ) {
            ch = *p;
            if( ch == '\0' ) break;
            if( p == sym && isdigit( ch ) ) {
                quote = TRUE;
            } else if( !(isalnum( ch ) || ch == '_' || ch == '?') ) {
                quote = TRUE;
            }
            ++p;
        }
        if( quote ) {
            sym--;
            *sym = '`';
            *p++ = '`';
            *p = '\0';
        }
    } else {
        p += strlen( p );
    }
    if( range_len != 0 ) {
        memcpy( p, range, range_len );
        p[range_len] = '\0';
    }
    sym_trans->next = SymTransHead;
    SymTransHead = sym_trans;
    return( sym );
}

void FreeSymTranslations()
{
    translation         *tmp;

    while( SymTransHead != NULL ) {
        tmp = SymTransHead->next;
        FreeMem( SymTransHead );
        SymTransHead = tmp;
    }
}
