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
* Description:  Single DGROUP in a DLL sanity check.
*
****************************************************************************/


#include "variety.h"
#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include "ddgrp.h"

#define message_prefix \
    "This Dynamic Link Library is already in use by:\n"

#define message_suffix \
    "\n\nThe Open Watcom Win32 runtime library does not support "\
    "attaching more than one process to a Dynamic Link Library "\
    "that does not have a separate data space for each attached process.\n"

static char buf[sizeof( message_prefix ) + _MAX_PATH + sizeof( message_suffix )];
static char dllname[_MAX_PATH];
static char pgmname[_MAX_PATH];

BOOL __disallow_single_dgroup( HANDLE hdll )
{
    GetModuleFileName( 0, pgmname, sizeof( pgmname ) );
    GetModuleFileName( hdll, dllname, sizeof( dllname ) );
    buf[0] = '\0';
    strcat( buf, message_prefix );
    strcat( buf, pgmname );
    strcat( buf, message_suffix );
    MessageBox( NULL, buf, dllname, MB_OK );
    return( 1 );
}
