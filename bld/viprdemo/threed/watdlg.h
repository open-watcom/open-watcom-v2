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


#ifndef WATDLG_WATDLG_H
#define WATDLG_WATDLG_H
/*
 Description:
 ============
    This file defines the external functions for the WATDLG DLL library.

*/

#include "wpitypes.h"

extern BOOL APIENTRY watdlg_dll_init( void );
extern void APIENTRY watdlg_dll_done( void );
extern BOOL APIENTRY watdlg_choose_font(void __far *,WPI_COLOUR far *,int );
extern BOOL APIENTRY watdlg_choose_font_default( void far *, WPI_COLOUR far *, int num_colors, WPI_LPLOGFONT def_font );
extern BOOL APIENTRY watdlg_choose_font_default_action( void far *, WPI_COLOUR far *, int num_colors, WPI_LPLOGFONT def_font, BOOL *def );

extern BOOL APIENTRY watdlg_choose_color(HWND ,WPI_COLOUR *);

extern BOOL APIENTRY watdlg_edit_palette( WPI_COLOUR far *palette, int far *size, int max_size, HWND parent );

extern int watdlg_version_check( void );

extern int APIENTRY dll_fopen(char *,HFILE_FORMAT );
extern int APIENTRY dll_fclose(HFILE );
extern char *APIENTRY dll_fgets(char *,int ,HFILE );
extern void APIENTRY dll_fread(void *,int ,int ,HFILE );
extern void APIENTRY dll_fputs(char *,HFILE );
extern void APIENTRY dll_fwrite(void *,int ,int ,HFILE );

extern void APIENTRY watdlg_register_app(char __far * );
extern void APIENTRY watdlg_deregister_app(char __far * );

extern void __near *APIENTRY watdlg_help_init(HWND ,char __far *);
extern void APIENTRY watdlg_help_done(void __near *);
extern void APIENTRY watdlg_do_help(void __near *);
extern void APIENTRY watdlg_help_set_context(void __near *,int ,DWORD );

/*** wide line prototype and enums ***/
/* line widths */
enum {
    LW_SINGLE = 1,
    LW_NARROW,
    LW_MEDIUM,
    LW_WIDE,
    LW_HUGE
};
typedef unsigned short wl_width;

/* line styles */
enum {
    LS_NONE = 1,
    LS_SOLID,
    LS_DASH,
    LS_DASH_DOT,
    LS_DOT
};
typedef unsigned short wl_style;

extern BOOL APIENTRY wide_line_to( WPI_PRES, short x_end, short y_end, wl_width width, unsigned short style, WPI_COLOUR );
extern BOOL APIENTRY wide_line_to_style( WPI_PRES hdc, short x_end, short y_end, wl_width width, wl_style style, WPI_COLOUR colour, int * curr_bit, BOOL );


#ifndef DBG_INTERNAL
#ifndef PROD

extern void APIENTRY watdlg_dbg_start( char *module );
extern void APIENTRY watdlg_dbg_print( char *module, char *fmt, ... );
#define watdlg_dbg( parm ) watdlg_dbg_print parm

#else

#define watdlg_dbg_start( m )
#define watdlg_dbg( parm )

#endif
#endif


#endif
