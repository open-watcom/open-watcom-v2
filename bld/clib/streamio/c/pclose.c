/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2025      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Implementation of _pclose() for OS/2 and Win32.
*
****************************************************************************/


#include "variety.h"
#include <process.h>
#include <stdio.h>
#include "rtdata.h"


_WCRTLINK int _pclose( FILE *fp )
/*******************************/
{
    int                 status;

    /*** Get the return code of the process ***/
    if( _FP_PIPEDATA( fp ).isPipe == 0 ) {
        return( -1 );                       /* must be a pipe */
    }
    if( fclose( fp ) ) {
        return( -1 );                       /* must close pipe before cwait */
    }
    if( cwait( &status, _FP_PIPEDATA( fp ).pid, WAIT_CHILD ) == -1 ) {
        return( -1 );
    }

    /*** Handle the return code ***/
    if( (status & 0x00FF) == 0 ) {
        return( (status & 0xFF00) >> 8 );   /* normal termination */
    } else {
        return( status >> 8 );              /* abnormal termination */
    }
}
