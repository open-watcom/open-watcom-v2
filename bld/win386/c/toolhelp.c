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
* Description:  Covers for select toolhelp.dll routines.
*
****************************************************************************/


#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <toolhelp.h>
#include "stubs.h"

#pragma aux BackPatch_toolhelp parm [ax]
extern LPVOID FAR BackPatch_toolhelp( char *strx );

static DWORD (FAR PASCAL *toolhelpMemoryRead)(WORD wSel, DWORD dwOffset, void FAR* lpBuffer, DWORD dwcb);
static DWORD (FAR PASCAL *toolhelpMemoryWrite)(WORD wSel, DWORD dwOffset, void FAR* lpBuffer, DWORD dwcb);

/*
 * __MemoryRead - cover for toolhelp MemoryRead
 */
DWORD FAR PASCAL __MemoryRead( WORD sel, DWORD off, LPVOID buff, DWORD cb )
{
    DWORD       rc;
    DWORD       alias;

    if( toolhelpMemoryRead == NULL ) {
        toolhelpMemoryRead = BackPatch_toolhelp( "MemoryRead" );
        if( toolhelpMemoryRead == NULL ) {
            return( 0 );
        }
    }
    DPMIGetHugeAlias( (DWORD) buff, &alias, cb );
    rc = toolhelpMemoryRead( sel, off, (LPVOID) alias, cb );
    DPMIFreeHugeAlias( alias, cb );

    return( rc );

} /* __MemoryRead */

/*
 * __MemoryWrite - cover for toolhelp MemoryWrite
 */
DWORD FAR PASCAL __MemoryWrite( WORD sel, DWORD off, LPVOID buff, DWORD cb )
{
    DWORD       rc;
    DWORD       alias;

    if( toolhelpMemoryWrite == NULL ) {
        toolhelpMemoryWrite = BackPatch_toolhelp( "MemoryWrite" );
        if( toolhelpMemoryWrite == NULL ) {
            return( 0 );
        }
    }
    DPMIGetHugeAlias( (DWORD) buff, &alias, cb );
    rc = toolhelpMemoryWrite( sel, off, (LPVOID) alias, cb );
    DPMIFreeHugeAlias( alias, cb );

    return( rc );

} /* __MemoryWrite */
