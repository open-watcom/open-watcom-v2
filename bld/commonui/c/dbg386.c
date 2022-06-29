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
* Description:  Win386 debugging routines.
*
****************************************************************************/


#include "commonui.h"
#include "bool.h"
#include "wdebug.h"
#include "winintrf.h"
#include "di386cli.h"


#define pick(c,d,r,a) c ## _func *c;
#include "_di386.h"
#undef pick

static HANDLE   wint32DLL;

/*
 * Start386Debug - initialize for 32-bit debugging
 */
bool Start386Debug( void )
{
    WDebug386 = false;
    if( CheckWin386Debug() == WGOD_VERSION ) {
        wint32DLL = LoadLibrary( "wint32.dll" );
        if( (UINT)wint32DLL >= 32 ) {

            #define pick(c,d,r,a) c = (c ## _func *)GetProcAddress( wint32DLL, #d );
            #include "_di386.h"
            #undef pick

            if( SetDebugInterrupts32() ) {
                WDebug386 = true;
            } else {
                FreeLibrary( wint32DLL );
            }
        }
    }
    return( WDebug386 );

} /* Start386Debug */


/*
 * Done386Debug - done with 386 debug handling
 */
void Done386Debug( void )
{
    if( WDebug386 ) {
        ResetDebugInterrupts32();
        FreeLibrary( wint32DLL );
        WDebug386 = false;
    }

} /* Done386Debug */
