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


#include "owlpriv.h"

static patricia *newNode( owl_string_table *table, const char *text, unsigned bit ) {
//***********************************************************************************

    patricia            *node;
    size_t              len;

    len = strlen( text );
    table->bytes += len + 1;
    node = _ClientAlloc( table->file, sizeof( *node ) + len );
    node->left = NULL;
    node->right = NULL;
    node->bit = bit;
    strcpy( node->text, text );
    return( node );
}

static unsigned getBit( const char *text, unsigned len, unsigned bit_pos ) {
//**************************************************************************

    unsigned            char_pos;
    unsigned            bit_off;

    char_pos = bit_pos / 8;
    bit_off = bit_pos % 8;

    if( char_pos > len ) return( 0 );

    return( text[ char_pos ] & ( 0x80 >> bit_off ) );
}


static owl_string_handle patInsert( owl_string_table *table, const char *text ) {
//*******************************************************************************

    patricia            *prev;
    patricia            *curr;
    patricia            *n;
    patricia            *other;
    unsigned            len;
    unsigned            curr_len;
    unsigned            last_bit;
    unsigned            diff_bit;

    len = strlen( text ) + 1;

    prev = table->tree;
    curr = prev->left;

    while( prev->bit < curr->bit ) {
        prev = curr;
        curr = getBit( text, len, curr->bit ) ? curr->right : curr->left;
    }

    if( strcmp( text, curr->text ) == 0 ) return( curr );

    curr_len = strlen( curr->text ) + 1;
    last_bit = curr_len;
    if( last_bit > len )
        last_bit = len;
    last_bit *= 8;

    for( diff_bit = 0; diff_bit < last_bit; diff_bit++ ) {
        if( getBit( text, len, diff_bit ) != getBit( curr->text, curr_len, diff_bit ) ) break;
    }

    prev = table->tree;
    other = prev->left;

    while( prev->bit < other->bit && other->bit < diff_bit ) {
        prev = other;
        other = getBit( text, len, other->bit ) ? other->right : other->left;
    }

    n = newNode( table, text, diff_bit );

    if( getBit( text, len, diff_bit ) ) {
        n->left = other;
        n->right = n;
    } else {
        n->right = other;
        n->left = n;
    }

    if( getBit( text, len, prev->bit ) ) {
        prev->right = n;
    } else {
        prev->left = n;
    }

    return( n );

}

static void patFree( owl_string_table *table, patricia *tree ) {
//**************************************************************

    patricia            *child;

    child = tree->left;
    if( child != NULL && child->bit > tree->bit ) {
        patFree( table, child );
    }

    child = tree->right;
    if( child != NULL && child->bit > tree->bit ) {
        patFree( table, child );
    }

    _ClientFree( table->file, tree );
}

static void patEmit( owl_string_table *table, patricia *tree, char **buffer, owl_offset *offset ) {
//*************************************************************************************************

    patricia            *child;
    unsigned            len;

    table = table;

    tree->offset = *offset;
    len = strlen( tree->text ) + 1;
    memcpy( *buffer, tree->text, len );
    *buffer += len;
    *offset += len;

    child = tree->left;
    if( child != NULL && child->bit > tree->bit ) {
        patEmit( table, child, buffer, offset );
    }

    child = tree->right;
    if( child != NULL && child->bit > tree->bit ) {
        patEmit( table, child, buffer, offset );
    }
}

static void patDump( patricia *tree, unsigned indent ) {
//******************************************************

    patricia            *child;
    unsigned            i;

    child = tree->left;
    if( child != NULL && child->bit > tree->bit ) {
        patDump( child, indent + 1 );
    }
    for( i = 0; i < indent; i++ ) {
        printf( "\t" );
    }
    printf( "%lx (%d,'%s',l=%lx,r=%lx)\n", (unsigned long)(pointer_int)tree, tree->bit, tree->text,
            (unsigned long)(pointer_int)tree->left, (unsigned long)(pointer_int)tree->right );
    child = tree->right;
    if( child != NULL && child->bit > tree->bit ) {
        patDump( child, indent + 1 );
    }
}

extern owl_string_table * OWLENTRY OWLStringInit( owl_file_handle file ) {
//************************************************************************

    owl_string_table    *table;

    table = _ClientAlloc( file, sizeof( *table ) );
    table->file = file;
    table->bytes = 0;
    table->tree = newNode( table, "", 0 );
    table->tree->left  = table->tree;
    table->tree->right = table->tree;
    return( table );
}

extern void OWLENTRY OWLStringFini( owl_string_table *table ) {
//*************************************************************

    patFree( table, table->tree );
    _ClientFree( table->file, table );
}

extern owl_string_handle OWLENTRY OWLStringAdd( owl_string_table *table, const char *string ) {
//*********************************************************************************************

    return( patInsert( table, string ) );
}

extern const char * OWLENTRY OWLStringText( owl_string_handle string ) {
//**********************************************************************

    return( string->text );
}

extern owl_offset OWLENTRY OWLStringOffset( owl_string_handle string ) {
//**********************************************************************

    return( string->offset );
}

extern owl_offset OWLENTRY OWLStringTableSize( owl_string_table *table ) {
//************************************************************************

    return( table->bytes );
}

extern void OWLENTRY OWLStringEmit( owl_string_table *table, char *buffer ) {
//***************************************************************************

    owl_offset          offset;

    offset = 0;
    patEmit( table, table->tree, &buffer, &offset );
}

extern void OWLENTRY OWLStringDump( owl_string_table *table ) {
//*************************************************************

    patDump( table->tree, 0 );
}
