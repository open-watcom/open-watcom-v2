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
* Description:  DLL main procedure
*
****************************************************************************/


#include <stdio.h>
#ifdef __OS2__
#include <os2def.h>
#else
#include <windows.h>
#endif

#ifdef __NT__

BOOL WINAPI LibMain( HINSTANCE inst, DWORD reason, LPVOID *ptr )
/**************************************************************/
{
    inst = inst;
    reason = reason;
    ptr = ptr;
    return( TRUE );
}

#elif defined(__WINDOWS__)

int WINAPI LibMain( HANDLE inst, WORD wDataSeg, WORD wHeapSize, LPSTR lpszCmdLine )
/*********************************************************************************/
{
    wDataSeg = wDataSeg;
    wHeapSize = wHeapSize;
    lpszCmdLine = lpszCmdLine;
    inst = inst;
    return( 1 );
}

int WINAPI WEP( int res )
/***********************/
{
    res = res;

    return( 1 );
}

#else

unsigned APIENTRY LibMain( unsigned hmod, unsigned termination )
/**************************************************************/
{
/*
    if( termination != 0 ) {
        return( __dll_terminate() );
    }
    return( __dll_initialize() );
*/
    return( 1 );
}
#endif
