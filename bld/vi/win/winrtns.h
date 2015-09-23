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
* Description:  Windows specific prototypes.
*
****************************************************************************/


#ifdef __WIN__

#ifndef WINRTNS_INCLUDED
#define WINRTNS_INCLUDED

#include "winhdr.h"
void        DDEFini( void );
void        MessageLoop( bool block );
window_id   NewEditWindow( void );
window_id   NewCommandWindow( void );
window_id   NewFileCompleteWindow( void );
window_id   NewRepeatCountWindow( void );
window_id   NewMsgWindow( void );
window_id   NewStatWindow( void );
bool        WindowsKeyPush( WORD, WORD );
vi_rc       MenuCommand( UINT );
void        StatusLine( int, char *, int );
int         GetStatusHeight( void );

// display.c
void    ClearWindow( window_id );
#ifdef BITBLT_BUFFER_DISPLAY
int     DisplayLineInWindowWithSyntaxStyle( window_id, int, line *, linenum, char *, int, HDC, HDC );
#else
int     DisplayLineInWindowWithSyntaxStyle( window_id, int, line *, linenum, char *, int, HDC );
#endif
// int CountChars( char *, char * );
// int FindPos( char *, int );

void        ColorAColumnRange( int, int, int, type_style * );
void        HiliteAColumnRange( linenum, int, int );
void        BarfFontData( FILE * );
void        ResizeRoot( void );
bool        RegisterContainerWindow( HANDLE );
window_id   CreateMainWindow( HANDLE );
window_id   CreateContainerWindow( LPRECT );
void        SetInitialWindowSize( const char * );
void        SetInitialWindowRect( RECT *r );
bool        GetDWORD( const char **str, DWORD *res );
void        SelRgnInit( void );
void        SelRgnFini( void );
void        ExecWait( char * );
void        NewToolBar( RECT * );
void        DestroyToolBar( void );
void        CloseToolBar( void );
vi_rc       AddBitmapToToolBar( char * );
vi_rc       DeleteFromToolBar( char * );
UINT        NextMenuId( void );
vi_rc       HandleToolCommand( UINT );
void        Draw3DBox( window_id, RECT *, RECT * );
void        ResetExtraRects( void );
void        HandleInitMenu( HMENU hmenu );
void        ResetMenuBits( void );
HWND        GetToolbarWindow( void );
vi_key      GetAutosaveResponse( void );
void        ReadProfile( void );
void        WriteProfile( void );
void        FiniProfile( void );
void        StatusDisplay( char * );

// Cursor.c
void        GoodbyeCursor( HWND );

// Tab_hell.c
bool        CursorPositionOffRight( int vc );

// Snoop.c
bool        GetSnoopStringDialog( fancy_find **ff );

// Find.c
bool        GetFindStringDialog( fancy_find *ff );

bool    GetReplaceStringDialog( fancy_find *ff );
void    EditSubClass( HWND hwnd, int id, history_data *h );
void    RemoveEditSubClass( HWND hwnd, int id );
vi_key  MapVirtualKeyToVIKey( WORD vk, WORD data );
void    InitGrepDialog( void );
void    FiniGrepDialog( void );
bool    SetGrepDialogFile( char *str );
bool    GetCmdDialog( char *str, int len );
bool    GetLineDialog( long * );
void    RefreshColorbar( void );
void    RefreshSSbar( void );
void    RefreshFontbar( void );
void    SetSaveConfig( void );
void    BarfToolBarData( FILE * );
DWORD   GetEditStyle( bool is_max );
void    EditDrawScrollBars( HWND hwnd );
void    SetWindowTitle( HWND hwnd );
void    HandleMenuSelect( WPARAM wparam, LPARAM lparam );
void    GetMenuHelpString( char *res );
void    SetMenuHelpString( char *str );
void    ShowStartupDialog( void );
void    CloseStartupDialog( void );
void    UsageDialog( char **, char *, int );
char    *GetInitialFileName( void );
int     HasShare( void );
int     PickATag( int clist, char **list, const char *tagname );

// clrpick.c
void    InitClrPick( void );
void    FiniClrPick( void );

// ftpick.c
void    InitFtPick( void );
void    FiniFtPick( void );

bool    GetSetFSDialog( void );
bool    GetSetScrDialog( void );
bool    GetSetGenDialog( void );
void    CenterWindowInRoot( HWND );
void    StatusWndSetSeparatorsWithArray( short *, int );

// ideactiv.c
void    StartIDE( HANDLE instance, BOOL dospawn );

// filetype.c
void    UpdateFileTypeIcon( HWND hwnd, const char *filename );

#endif

#endif
