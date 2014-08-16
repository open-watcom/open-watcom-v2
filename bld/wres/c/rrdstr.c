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


#include "layer0.h"
#include "varstr.h"
#include "read.h"
#include "wresrtns.h"

char *ResReadString( WResFileID handle, int *strlen )
/***************************************************/
{
    VarString *         newstring;
    bool                error;
    uint_8              nextchar;
    char *              retstring;

    newstring = VarStringStart();
    error = ResReadUint8( &nextchar, handle );
    while( !error && nextchar != '\0' ) {
        VarStringAddChar( newstring, nextchar );
        error = ResReadUint8( &nextchar, handle );
    }

    retstring = VarStringEnd( newstring, strlen );

    if( error && retstring != NULL ) {
        WRESFREE(retstring);
        retstring = NULL;
    }

    return( retstring );
} /* ResReadString */

char *ResRead32String( WResFileID handle, int *strlen )
/*****************************************************/
{
    VarString *         newstring;
    bool                error;
    uint_16             nextchar;
    char *              retstring;

    newstring = VarStringStart();
    error = ResReadUint16( &nextchar, handle );
    while( !error && nextchar != 0x0000 ) {
        VarStringAddChar( newstring, (uint_8)(nextchar & 0x00ff) );
        error = ResReadUint16( &nextchar, handle );
    }

    retstring = VarStringEnd( newstring, strlen );

    if( error && retstring != NULL ) {
        WRESFREE(retstring);
        retstring = NULL;
    }

    return( retstring );
} /* ResRead32String */
