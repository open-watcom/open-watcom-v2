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


extern HFONT    FontHandle( font_type );
extern UINT     FontHeight( font_type );
extern int      FontlfHeight( font_type );
extern char     *FontlfFaceName( font_type );
extern char     FontIsFunnyItalic( font_type );
extern UINT     FontAverageWidth( font_type );
extern UINT     FontMaxWidth( font_type );
extern UINT     FontTabWidth( font_type );
extern UINT     FontFixed( font_type );
extern void     InitFonts( void );
extern void     FiniFonts( void );
extern void     PickFont( font_type, HWND );
extern void     SetUpFont( LOGFONT *, font_type );
extern void     EnsureUniformFonts( font_type, font_type, LOGFONT *, BOOL );
