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
        Loading/unloading symbolic information.
*/

dip_status      DIGENTRY DIPImpLoadInfo( dig_fhandle dfh, imp_image_handle *ii )
{
    //TODO:
    /*
        See if the given file handle has our kind of symbolic information
        on it. If so, fill in the "*ii" structure and return DS_OK. If it
        isn't our type of information, return DS_FAIL. If it's our information,
        but something goes wrong during the load, return a DS_ERR|DS_?
        constant. If the load succeeds the client will not close the
        file handle so if you don't need to read out of the file after the
        load, you should close the handle at this point.

     */
     return( DS_FAIL );
}

void            DIGENTRY DIPImpMapInfo( imp_image_handle *ii, void *d )
{
    //TODO:
    /*
        After the symbolic information has been loaded, any addresses
        must be relocated from the map values that the linker put in to
        the actual load time addresses. The client is not always able to
        perform the relocation at the time it loads the information, so
        it calls this entry point when it has all the information required.
        The 'd' parameter is a magic cookie for the client. Just pass it
        on as the second parameter of the DCMapAddr function when you call
        it.
    */
}

void            DIGENTRY DIPImpUnloadInfo( imp_image_handle *ii )
{
    //TODO:
    /*
        The indicated image is going away. Free any structures pointed to
        by the 'ii' parameter. Don't try and free the 'ii' pointer itself.
        The DIP manager will take care of that. If you hung onto the file
        handle passed in to the DIPImpLoadInfo function, you should close
        it now.
    */
}
