/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Allocate and read DDE data.
*
****************************************************************************/


#include "wrglbl.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/*****************************************************************************/

/****************************************************************************/
/* external variables                                                       */
/****************************************************************************/

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/


bool    WRAPI WRAllocDataFromDDE( HDDEDATA hData, char **pdata, size_t *dsize )
{
    char        *data;
    size_t      size;
    bool        ok;

    ok = ( hData != NULL && pdata != NULL && dsize != NULL );
    if( ok ) {
        *dsize = size = (size_t)DdeGetData( hData, NULL, 0, 0 );
        if( size == 0 ) {
            ok = false;
        } else {
            *pdata = data = WRMemAlloc( size );
            if( data == NULL ) {
                ok = false;
            } else {
                if( (DWORD)size != DdeGetData( hData, (LPBYTE)data, (DWORD)size, 0 ) ) {
                    ok = false;
                }
            }
        }
        if( !ok ) {
            if( data != NULL ) {
                WRMemFree( data );
            }
            *pdata = NULL;
            *dsize = 0;
        }
    }
    return( ok );
}
