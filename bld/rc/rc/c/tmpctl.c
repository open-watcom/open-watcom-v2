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
#include "bool.h"
#include "rctypes.h"
#include "tmpctl.h"

#define MAX_NAMES       10

static int          numTmpNames;
static const char   *tmpFileNames[MAX_NAMES];

void RegisterTmpFile( const char *name )
/**************************************/
{
    int     i;

    for( i = 0; i < numTmpNames; i++ ) {
        if( tmpFileNames[i] == NULL ) {
            tmpFileNames[i] = name;
            return;
        }
    }
    numTmpNames++;
    if( numTmpNames != MAX_NAMES ) {
        tmpFileNames[i] = name;
    }
}

void UnregisterTmpFile( const char *name )
/****************************************/
{
    int     i;

    for( i = 0; i < numTmpNames; i++ ) {
        if( tmpFileNames[i] != NULL ) {
            if( !strcmp( tmpFileNames[i], name ) ) {
                tmpFileNames[i] = NULL;
                return;
            }
        }
    }
}

bool IsTmpFile( const char *name )
/********************************/
{
    int     i;

    for( i = 0; i < numTmpNames; i++ ) {
        if( tmpFileNames[i] != NULL ) {
            if( !strcmp( tmpFileNames[i], name ) ) {
                return( true );
            }
        }
    }
    return( false );
}

extern void TmpCtlInitStatics( void )
/***********************************/
{
    numTmpNames = 0;
    memset( tmpFileNames, 0, sizeof( tmpFileNames ) );
}
