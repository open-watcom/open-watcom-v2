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


#define FONT_COURIER        0
#define FONT_COURIERBOLD    1
#define FONT_HELV           2
#define FONT_ARIAL          3
#define FONT_ARIALBOLD      4
#define FONT_FIXED          5
#define FONT_SANSSERIF      6

#define FONT_DEFAULT        FONT_COURIER
#define FONT_DEFAULTBOLD    FONT_COURIERBOLD

extern UINT DefineFont( LOGFONT * );
extern HFONT FontHandle( UINT );
extern UINT FontHeight( UINT );
extern int FontlfHeight( UINT );
extern char *FontlfFaceName( UINT );
extern char FontIsFunnyItalic( UINT );
extern UINT FontAverageWidth( UINT );
extern UINT FontMaxWidth( UINT );
extern UINT FontTabWidth( UINT );
extern UINT FontFixed( UINT );
extern InitFonts();
extern FiniFonts();
extern void PickFont( int, HWND );
extern void SetUpFont( LOGFONT *, int );
void EnsureUniformFonts( int, int, LOGFONT *, BOOL );
