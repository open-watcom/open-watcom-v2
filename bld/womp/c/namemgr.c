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
#include "womp.h"
#include "memutil.h"
#include "namemgr.h"
#include "myassert.h"
#include "array.h"

typedef struct {
    char    *name;
} name_elm;

#define NAMEBLK_SIZE    1024
typedef struct nameblk nameblk_t;
struct nameblk {
    nameblk_t   *next;
    size_t      cur_off;
    uint_8      data[ NAMEBLK_SIZE ];
};

STATIC array_hdr        *nameArr;
STATIC char             nullString[] = { '\0' };
STATIC uint_16          numNames;
STATIC nameblk_t        *nameBlks;      /* stack of name blocks */

extern void NameInit( void ) {
/**************************/

    nameArr = ArrCreate( sizeof( name_elm ), NULL );
    numNames = 0;
    nameBlks = MemAlloc( sizeof( nameblk_t ) );
    nameBlks->next = NULL;
    nameBlks->cur_off = 0;
}

extern void NameFini( void ) {
/**************************/
    nameblk_t   *cur;
    nameblk_t   *next;

    ArrDestroy( nameArr );
    cur = nameBlks;
    while( cur != NULL ) {
        next = cur->next;
        MemFree( cur );
        cur = next;
    }
}

extern name_handle NameAdd( const char *name, size_t len ) {
/********************************************************/
    char        *p;
    name_elm    *elm;
    size_t      num;
    nameblk_t   *cur;

    num = numNames;
    ++numNames;
    elm = ArrNewElm( nameArr, num );
    ++len;
    cur = nameBlks;
    while( cur != NULL ) {
        if( cur->cur_off <= NAMEBLK_SIZE - len ) {
            break;
        }
        cur = cur->next;
    }
    if( cur == NULL ) {
        cur = MemAlloc( sizeof( *cur ) );
        cur->next = nameBlks;
        nameBlks = cur;
        cur->cur_off = 0;
    }
/**/myassert( cur != NULL );
    p = elm->name = (char *)cur->data + cur->cur_off;
    cur->cur_off += len;
    --len;
    memcpy( p, name, len );
    p[ len ] = 0;
    return( num );
}

extern const char *NameGet( name_handle hdl ) {
/*******************************************/
    name_elm    *elm;

/**/myassert( hdl < numNames || hdl == NAME_NULL );
    if( hdl == NAME_NULL ) {
        return( nullString );
    }
    elm = ArrAccess( nameArr, hdl );
/**/myassert( elm->name != NULL );
    return( elm->name );
}
