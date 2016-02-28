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
* Description:  Prototypes for resource strings helper routines.
*
****************************************************************************/


#ifndef _LDSTR_H_INCLUDED
#define _LDSTR_H_INCLUDED

#include <stdio.h>
#include <stdarg.h>


/************************************************************************
 * SetInstance - must be called before any other routine in this file
 ***********************************************************************/
void    SetInstance( HANDLE inst );
char    *AllocRCString( msg_id id );
void    FreeRCString( char *str );
char    *GetRCString( msg_id msgid );
int     CopyRCString( msg_id id, char *buf, int bufsize );

/*
 * All functions below this point obtain strings using AllocRCString and
 * FreeRCString
 */

int     RCMessageBox( HWND hwnd , msg_id msgid, char *title, UINT type );
void    RCfprintf( FILE *fp, msg_id strid, ...  );
int     RCsprintf( char *buf, msg_id fmtid, ... );
void    RCvfprintf( FILE *fp, msg_id strid, va_list al );

#endif /* _LDSTR_H_INCLUDED */
