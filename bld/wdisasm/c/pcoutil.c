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
#include "disasm.h"
#include "wdismsg.h"

void                    **NameTab;
uint_16                 NameIndex;
void                    **SegTab;
uint_16                 SegIndex;
void                    **GrpTab;
uint_16                 GrpIndex;
void                    **ExtTab;
uint_16                 ExtIndex;
void                    **ThrdTab;


uint_32 GetLong()
/***************/

{
    int_32  first_word;
    int_32  next_word;

    first_word = GetWord();
    next_word = GetWord();
    return( first_word | ( next_word << 16 ) );
}


uint_16 GetWord()
/***************/

{
    char                first_byte;
    char                next_byte;

    first_byte = GetByte();
    next_byte = GetByte();
    return( first_byte | ( next_byte << 8 ) );
}


uint_16 GetIndex()
/****************/

{
    uint_16 index;

    index = GetByte();
    if( index & 0x80 ) {
        index = ( ( index & 0x7f ) << 8 ) | GetByte();
    }
    return( index );
}


uint_32  GetVarSize()
/*******************/

/* Variable length field is part of object record 0xb0 (COMDEF).
   If the value is <= 128 then it is a 1-byte field containing the
   value. If the value is > 128 then there is an extra initial byte
   that indicates whether the value is represented in 2 bytes (0x81),
   3 bytes (0x84) or 4 bytes (0x88). */

{
    uint_16             index;
    uint_32             size;

    index = GetByte();
    if( index > COMDEF_LEAF_SIZE ) {
        size = (uint_8)GetByte();
        size += (uint_16)GetByte() << 8;
        if( index >= COMDEF_LEAF_3 ) {
            size += (uint_32)GetByte() << 16;
            if( index >= COMDEF_LEAF_4 ) {
                size += (uint_32)GetByte() << 24;
            }
        }
    } else {
        size = index;
    }
    return( size );
}


void  DoError( char *msg_buff, bool fatal )
/*****************************************/
{
    char                rec_num[ 7 ];

    PutText( MSG_ASTERISK_1 );
    if( fatal ) {
        PutText( MSG_FATAL );
    } else {
        PutText( MSG_WARNING );
    }
    itoa( RecNumber, rec_num, 10 );
    MsgSubStr( msg_buff, rec_num, 's' );
    PutString( msg_buff );
    PutText( MSG_ASTERISK_2 );
    FPutEnd( stdout );
    if( fatal ) {
        MsgFini();
        exit( 1 );
    }
}


void  Error( int msg_num, bool fatal )
/************************************/
{
    char                msg_buff[MAX_RESOURCE_SIZE];

    MsgGet( msg_num, msg_buff );
    DoError( msg_buff, fatal );
}


char  *NameAlloc( char *text, int length )
/****************************************/
{
    char                *name;

    name = AllocMem( length + 1 );
    memmove( name, text, length );
    name[ length ] = NULLCHAR;
    return( name );
}


char  *GetName( int max )
/***********************/
{
    int                 i;
    int                 length;
    char                name[ MAX_NAME_LEN + 1 ];

    length = GetByte();
    if( length > max ) {
        Error( ERR_NAME_LONG, false );
    } else {
        max = length;
    }
    i = 0;
    while( i < max ) {
        name[ i++ ] = _Trans( GetByte() );
        --length;
    }
    while( length > 0 ) {
        GetByte();
        --length;
    }
    return( NameAlloc( name, max ) );
}


module  *NewModule()
/******************/

{
    return( AllocNull( sizeof( module ) ) );
}


segment  *NewSegment()
/********************/

{
    segment             *seg;
    segment             **owner;

    /* add segment to end of list for better ordering during output */
    owner = &Mod->segments;
    for( ;; ) {
        seg = *owner;
        if( seg == NULL ) break;
        owner = &seg->next_segment;
    }
    seg = AllocNull( sizeof( segment ) );
    seg->class = TYPE_SEGMENT;
    seg->offset = BAD_OFFSET;
    seg->scan_tabs = NULL;
    *owner = seg;
    return( seg );
}


group  *NewGroup()
/****************/

{
    group               *grp;

    grp = AllocNull( sizeof( group ) );
    grp->class = TYPE_GROUP;
    grp->next_group = Mod->groups;
    Mod->groups = grp;
    return( grp );
}


handle  *NewHandle( void *seg )
/*****************************/
{
    handle              *hndl;

    hndl = AllocNull( sizeof( handle ) );
    hndl->data = seg;
    return( hndl );
}


fixup  *NewFixup( char fix_type )
/*******************************/
{
    fixup               *fix;

    fix = AllocNull( sizeof( fixup ) );
    fix->class = fix_type;
    return( fix );
}


void  AddFix( segment *seg, fixup *new_fix )
/******************************************/
{
    fixup               *fix;
    fixup               *prev_fix;
    uint_32             addr;

    addr = new_fix->address;

    prev_fix = seg->fixes_rover;
    if( prev_fix != NULL ) {
        fix = prev_fix->next_fix;
        if( fix == NULL || fix->address > addr ) {
            prev_fix = NULL;
            fix = seg->fixes;
        }
    } else {
        /* remember prev_fix == NULL */
        fix = seg->fixes;
    }

    while( fix != NULL ) {
        if( fix->address > addr ) {
            break;
        }
        prev_fix = fix;
        fix = fix->next_fix;
    }
    new_fix->next_fix = fix;
    if( prev_fix == NULL ) {
        seg->fixes = new_fix;
    } else {
        prev_fix->next_fix = new_fix;
    }
    seg->fixes_rover = prev_fix;
}


void  *GetTab( uint_16 id, void **beg_tab, bool stop )
/****************************************************/
{
    void                **tab;
    int                 tab_num;
    int                 position;

    tab = beg_tab;
    position = id % TAB_SIZE;
    tab_num = id / TAB_SIZE;
    while( tab != NULL ) {
        if( --tab_num < 0 ) break;
        tab = (void **) *tab;
    }
    if( tab != NULL ) {
        tab = (void **) *( tab + ( position + 1 ) );
    }
    if( tab == NULL ) {
        Error( ERR_UNDEF_FORWARD, stop );
        PutTab( id, beg_tab, NULL );
    }
    return( (void *) tab );
}


void  PutTab( uint_16 id, void **tab, void *value )
/****************************/
{
    int                 tab_num;
    int                 position;
    void                **prev;
    void                **loc;

    position = id % TAB_SIZE;
    tab_num = id / TAB_SIZE;
    prev = NULL;
    while( tab != NULL ) {
        if( --tab_num < 0 ) break;
        prev = tab;
        tab = (void **) *tab;
    }
    if( tab == NULL ) {
        do {
            tab = AllocNull( ( TAB_SIZE + 1 ) * sizeof( char * ) );
            *prev = (char *) tab;
            prev = tab;
        } while( --tab_num >= 0 );
        *tab = NULL;
    }
    loc = tab + ( position + 1 );
    *loc = value;
}


static  void  InitTab( void **to_tab )
/************************************/
{
    *to_tab = AllocNull( ( TAB_SIZE + 1 ) * sizeof( char * ) );
}


void  InitTables()
/****************/

{
    int                 i;

    NameIndex = 0;
    SegIndex = 0;
    GrpIndex = 0;
    ExtIndex = 0;
    InitTab( &NameTab );
    InitTab( &SegTab );
    InitTab( &GrpTab );
    InitTab( &ExtTab );
    InitTab( &ThrdTab );
    i = 0;
    while( i < 8 ) {
        PutTab( i, ThrdTab, AllocMem( sizeof( thread ) ) );
        ++i;
    }
    PutTab( 0, GrpTab, NULL );
    PutTab( 0, SegTab, NULL );
    LastNum = 0;
    RetFarUsed = false;
}
