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


/* FMERROR - report form editting error messages */

#include <string.h>
#include <windows.h>

#include "fmedit.def"
#include "state.def"
#include "memory.def"

extern void SetError( char * msg )
/********************************/

/* record the passed error so it can be reported later */

  {
    char * err;

    err = GetErr();
    if( GetErr() != NULL ) {
        EdFree( err );        /* new error over-rides old error */
    }
    err = EdAlloc( strlen( msg ) + 1 );
    strcpy( err, msg );
    SetErr( err );
  }

extern void ReportPendingError()
/******************************/

/* report the previously reported error, if there is one */

  {
    char * err;

    err = GetErr();
    if( err != NULL )  {
        MessageBox( GetAppWnd(), err, NULL, MB_OK | MB_ICONEXCLAMATION );
        EdFree( err );
        SetErr( NULL );
    }
  }

extern void ReportError( char * msg, HANDLE hwnd )
/************************************************/

/* report the passed error */

  {
    MessageBox( hwnd, msg, NULL, MB_OK | MB_ICONEXCLAMATION );
  }
