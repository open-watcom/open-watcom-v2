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


#include "dbgdefn.h"
#include <stddef.h>
#include "dbgdata.h"
#include "dbglit.h"
#include "sortlist.h"


extern void             SetLastSym( char *to );
extern bool             SymBrowse( char **name );
extern char             *DupStr(char*);
extern void             WndGblFuncInspect( mod_handle mod );
extern void             WndGblVarInspect( mod_handle mod );
extern void             WndModListInspect( mod_handle mod );
extern image_entry      *ImagePrimary(void);


char *ImgSymName( image_entry *img, bool always )
{
    if( img->dip_handle != NO_MOD || always ) {
        if( img->sym_name != NULL ) {
            return( img->sym_name );
        } else {
            return( img->image_name );
        }
    } else {
        return( LIT( Empty ) );
    }
}

static int ImageCompare( void *_pa, void *_pb )
{
    image_entry **pa = _pa;
    image_entry **pb = _pb;

    if( *pa == ImagePrimary() ) return( -1 );
    if( *pb == ImagePrimary() ) return( 1 );
    return( DIPImagePriority( (*pa)->dip_handle ) - DIPImagePriority( (*pb)->dip_handle ) );
}

void    ImgSort( void )
{
    DbgImageList = SortLinkedList( DbgImageList, offsetof( image_entry, link ),
                                ImageCompare, DbgAlloc, DbgFree );
}


