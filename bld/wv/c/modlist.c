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
#include "dbgdefn.h"
#include "dbgdata.h"
//#include "dbgwind.h"
#include "dbglit.h"
#include "dbgerr.h"
#include "modlist.h"
#include "strutil.h"

#include "clibext.h"


extern bool             IsInternalMod( mod_handle mod );
extern image_entry      *ImagePrimary(void);
extern image_entry      *ImageEntry( mod_handle );
extern bool             FindFirstCue( mod_handle mod, cue_handle *ch );

extern  bool            ModHasSourceInfo( mod_handle handle )
{
    if( ModHasInfo( handle, HK_CUE ) == DS_OK ) return( TRUE );
    if( ModHasInfo( handle, HK_TYPE ) == DS_OK ) return( TRUE );
    if( ModHasInfo( handle, HK_SYM ) == DS_OK ) return( TRUE );
    return( FALSE );
}


OVL_EXTERN walk_result CheckAnyMod( mod_handle mh, void *d )
{
    module_list *list = d;

    if( IsInternalMod( mh ) ) return( WR_CONTINUE );
    if( list->sort != NULL ) {
        list->sort[list->numrows] = mh;
    }
    if( list->prefix != NULL && list->sort != NULL ) {
        ModName( mh, TxtBuff, TXT_LEN );
        if( strnicmp( list->prefix, TxtBuff, list->pref_len ) != 0 ) {
            return( WR_CONTINUE );
        }
    }
    list->numrows++;
    return( WR_CONTINUE );
}

OVL_EXTERN walk_result CheckMod( mod_handle mh, void *d )
{
    if( !ModHasSourceInfo( mh ) ) return( WR_CONTINUE );
    return( CheckAnyMod( mh, d ) );
}


int ModCompare( mod_handle const *a, mod_handle const *b )
{
    char        *namea;
    char        *nameb;

    namea = TxtBuff;
    nameb = TxtBuff + TXT_LEN / 2;
    namea[0] = nameb[0] = '\0';
    ModName( *a, namea, TXT_LEN/2 );
    ModName( *b, nameb, TXT_LEN/2 );
    return( stricmp( namea, nameb ) );
}

static int ModOrder( const void *ap, const void *bp )
{
    image_entry *ia;
    image_entry *ib;

    ia = ImageEntry( *(mod_handle const *)ap );
    ib = ImageEntry( *(mod_handle const *)bp );
    if( ia == ib ) return( ModCompare( ap, bp ) );
    if( ia == ImagePrimary() ) return( -1 );
    if( ib == ImagePrimary() ) return( +1 );
    if( ia->dip_handle < ib->dip_handle ) return( -1 );
    if( ia->dip_handle > ib->dip_handle ) return( +1 );
    return( 0 );
}

void ModListAddModules( module_list *list, mod_handle mod, bool any )
{
    walk_result (*rtn)( mod_handle mh, void *d );

    rtn = any ? &CheckAnyMod : &CheckMod;
    DbgFree( list->sort );
    list->numrows = 0;
    list->sort = NULL;
    /* get number of rows */
    WalkModList( mod, rtn, list );
    if( list->numrows == 0 ) {
        rtn = CheckAnyMod;
        WalkModList( mod, rtn, list );
        if( list->numrows == 0 ) return;
    }
    list->sort = DbgMustAlloc( list->numrows * sizeof( mod_handle ) );
    list->numrows = 0;
    WalkModList( mod, rtn, list );
    qsort( list->sort, list->numrows, sizeof( mod_handle ), ModOrder );
}


void    ModListInit( module_list *list, char *prefix )
{
    list->sort = NULL;
    list->prefix = NULL;
    if( prefix ) {
        list->prefix = DupStr( prefix );
        list->pref_len = strlen( prefix );
    }
}


void    ModListFree( module_list *list )
{
    DbgFree( list->prefix );
    DbgFree( list->sort );
    list->sort = NULL;
    list->prefix = NULL;
    list->pref_len = 0;
    list->numrows = 0;
}

int     ModListNumRows( const module_list *list )
{
    if( list->sort == NULL ) return( 0 );
    return( list->numrows );
}

mod_handle ModListMod( const module_list *list, int i )
{
    if( list->sort == NULL || i >= list->numrows ) return( NO_MOD );
    return( list->sort[i] );
}

void ModListName( const module_list *list, int i, char *buff )
{
    buff[0] = '\0';
    if( list->sort == NULL || i >= list->numrows ) return;
    ModName( list->sort[i], buff, TXT_LEN );
}

address ModFirstAddr( mod_handle mod )
{
    address     addr;
    DIPHDL( cue, ch );

    if( FindFirstCue( mod, ch ) ) {
        addr = CueAddr( ch );
    } else {
        addr = NilAddr;
    }
    if( IS_NIL_ADDR( addr ) ) {
        addr = ModAddr( mod );
    }
    return( addr );
}
