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


#include "skel.h"

/*
   Misc. stuff.
*/

const char      DIPImpName[] = "Put the name of your DIP here";

unsigned        DIPENTRY DIPImpQueryHandleSize( handle_kind hk )
{
    //TODO:
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

dip_status      DIPENTRY DIPImpMoreMem( unsigned size )
{
    //TODO:
/*
        Return DS_OK if you could release some memory, DS_FAIL if you
        couldn't.
*/
    return( DS_FAIL );
}

dip_status      DIPENTRY DIPImpStartup( void )
{
    //TODO:
/*
        Return DS_OK if startup initialization when OK, or a DS_ERR|DS_?
        constant if something went wrong.
*/
    return( DS_OK );
}

void            DIPENTRY DIPImpShutdown( void )
{
    //TODO:
/*
        Shutting down and unloading. Last chance to free up stuff.
*/
}

void            DIPENTRY DIPImpCancel( void )
{
    //TODO:
/*
        The client is about to longjmp, and may bypass
        returns through the DIP. All handles remain valid though.
*/
}
