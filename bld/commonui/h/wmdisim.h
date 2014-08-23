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
* Description:  Portable MDI interface.
*
****************************************************************************/


#ifndef _WMDISIM_H_INCLUDED
#define _WMDISIM_H_INCLUDED

#include "wpi.h"

typedef struct {
    HWND        root;
    HWND        container;
    DWORD       reg_style;
    DWORD       max_style;
    UINT        data_off;
    char        *main_name;
    void        (*start_max_restore)( HWND );
    void        (*end_max_restore)( HWND );
    void        (*set_window_title)( HWND );
    void        (*set_style)( HWND, int );
    WPI_INST    hinstance;
} mdi_info;

void    MDIInit( mdi_info * );
void    MDIInitMenu( void );
bool    MDINewWindow( HWND hwnd );
void    MDISetMainWindowTitle( char *fname );
void    MDIClearMaximizedMenuConfig( void );
bool    MDIIsMaximized( void );
bool    MDIIsWndMaximized( HWND );
bool    MDIUpdatedMenu( void );
void    MDISetMaximized( bool setting );
void    MDITile( bool is_horz );
void    MDICascade( void );
bool    MDIChildHandleMessage( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam, WPI_MRESULT *lrc );
bool    MDIHitClose( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam );
bool    MDIIsSysCommand( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam );
void    MDIResizeContainer( void );
void    MDIContainerResized( void );
void    MDISetOrigSize( HWND hwnd, WPI_RECT *rect );

#endif /* _WMDISIM_H_INCLUDED */
