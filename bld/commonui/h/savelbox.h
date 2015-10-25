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
* Description:  Save file listbox interface.
*
****************************************************************************/


#ifndef _SAVELBOX_H_INCLUDED
#define _SAVELBOX_H_INCLUDED

#include <stdio.h>

#define SLB_SAVE_AS     1
#define SLB_SAVE_TMP    2

void    SaveListBox( int how, void (*writefn)( FILE * ), char *tmpname, char *appname, HWND mainhwnd, HWND listbox );
void    ReportSave( HWND parent, char *fname, char *appname, bool save_ok );
bool    GetSaveFName( HWND mainhwnd, char *fname );
bool    GenTmpFileName( char *tmpname, char *buf );

WINEXPORT UINT_PTR CALLBACK LBSaveHook( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );

#endif /* _SAVELBOX_H_INCLUDED */
