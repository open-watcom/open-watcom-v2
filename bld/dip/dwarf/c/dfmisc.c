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


#include "dfdip.h"
#include "dfclean.h"
#include "dfld.h"
#include "dfcue.h"
#include "dfmod.h"
/*
   Misc. stuff.
*/

char const  DIPImpName[] = "DWARF";

df_cleaner       *Cleaners;

static void CallCleaners( void ){
/********************************/
//Call all stacked resource cleanup
    df_cleaner *curr;

    curr = Cleaners;
    while( curr != NULL ){
        curr->rtn( curr->d );
        curr = curr->prev;
    }
    Cleaners = NULL;
}

unsigned        DIPENTRY DIPImpQueryHandleSize( handle_kind hk )
{
/*
        Return the sizes of the individual handle types. This version
        should be OK as given.
*/
    static unsigned_8 Sizes[] = {
        sizeof( imp_image_handle ),
        sizeof( imp_type_handle ),
        sizeof( imp_cue_handle ),
        sizeof( imp_sym_handle )
    };

    return( Sizes[ hk ] );
}

static imp_image_handle *Images;

extern void DFAddImage( imp_image_handle *ii ){
/**************************************/
    ii->next = Images;
    Images = ii;
}

extern void DFFreeImage( imp_image_handle *ii ){
/**************************************/
    imp_image_handle *curr, **lnk;

    lnk = &Images;
    while( (curr = *lnk) != NULL  ){
       if( curr == ii ){
           *lnk = curr->next;
       }
       lnk = &curr->next;
    }
}

dip_status      DIPENTRY DIPImpMoreMem( unsigned size )
{
/*
        Return DS_OK if you could release some memory, DS_FAIL if you
        couldn't.
*/
    imp_image_handle *curr;
    dip_status ret;

    size = size;

    ret = DS_FAIL;
    curr = Images;
    while( curr != NULL ){
        if( ClearMods( curr ) ){
            ret = DS_OK;
        }
        if( DRDbgClear( curr->dwarf->handle ) ){
            ret = DS_OK;
        }
        if( FiniImpCueInfo( curr ) ){
            ret = DS_OK;
        }
        curr = curr->next;
    }
    return( ret );
}

dip_status      DIPENTRY DIPImpStartup(void)
{
/*
        Return DS_OK if startup initialization when OK, or a DS_ERR|DS_?
        constant if something went wrong.
*/
    Cleaners = NULL;
    Images = NULL;
    DRInit();
    return( DS_OK );
}

void            DIPENTRY DIPImpShutdown( void )
{
/*
        Shutting down and unloading. Last chance to free up stuff.
*/
        Images = NULL;
        DRFini();
}

void            DIPENTRY DIPImpCancel( void )
{
/*
        The client is about to longjmp, and may bypass
        returns through the DIP. All handles remain valid though.
*/
    CallCleaners();
}
