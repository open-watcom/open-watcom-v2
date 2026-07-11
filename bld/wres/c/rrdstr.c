/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2026      The Open Watcom Contributors. All Rights Reserved.
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


char *ResReadString( FILE *fp, size_t *len )
/******************************************/
{
    VarString           *varstr;
    bool                error;
    uint_8              nextchar;
    char                *str;

    varstr = VarStringStart();
    error = false;
    nextchar = ResReadUint8( &error, fp );
    while( !error && nextchar != '\0' ) {
        VarStringAddChar( varstr, nextchar );
        nextchar = ResReadUint8( &error, fp );
    }

    str = VarStringEnd( varstr, len );

    if( error && str != NULL ) {
        WRESFREE( str );
        str = NULL;
    }

    return( str );
} /* ResReadString */

char *ResRead32String( FILE *fp, size_t *len )
/********************************************/
{
    VarString           *varstr;
    bool                error;
    uint_16             nextchar;
    char                *str;

    varstr = VarStringStart();
    error = false;
    nextchar = ResReadUint16( &error, fp );
    while( !error && nextchar != 0x0000 ) {
        VarStringAddChar( varstr, UNI2ASCII( nextchar ) );
        nextchar = ResReadUint16( &error, fp );
    }

    str = VarStringEnd( varstr, len );

    if( error && str != NULL ) {
        WRESFREE( str );
        str = NULL;
    }

    return( str );
} /* ResRead32String */
