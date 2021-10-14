/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


/********************************/
/*  Windows graphics' functions */
/********************************/

#include <malloc.h>

// Macros

#if defined( _M_I86 )
#define _MyAlloc(size)  _fmalloc( size )
#define _MyFree(ptr)    _ffree( ptr )
#else
#define _MyAlloc(size)  malloc( size )
#define _MyFree(ptr)    free( ptr )
#endif

#if defined( __OS2__ )
#define _GPIEBORDER     15
#endif

// Global Variables
extern HWND             _CurrWin;
#if defined( __WINDOWS__ )
extern DWORD            _ColorMap[ 16 ];
#endif
//extern int            image_height;
//extern int            image_width;
extern WPI_PRES         _Mem_dc;
extern HDC              _Hdc;
extern WPI_HBITMAP      _Mem_bmp;
extern HFONT            _CurFnt;
extern HRGN             _ClipRgn;
extern struct xycoord   _BitBlt_Coord;
#if defined( __OS2__ )
extern PFNWP            _OldFrameProc;
#endif

// Function headers
/* Mapping utitlity functions */
extern int              _MapPlotAct( void );
extern WPI_HBITMAP      _Mask2Bitmap( HDC, unsigned char * );
extern int              _MapLineStyle( unsigned short );
extern WPI_COLOUR       _Col2RGB( short );
extern short            _RGB2Col( WPI_COLOUR );

/* Other Utilities */
extern WPI_MRESULT CALLBACK GraphWndProc( HWND, WPI_MSG, WPI_PARAM1, WPI_PARAM2 );
extern WPI_MRESULT CALLBACK GraphFrameProc( HWND, WPI_MSG, WPI_PARAM1, WPI_PARAM2 );
extern void             _MyInvalidate( WPI_RECTDIM x1, WPI_RECTDIM y1, WPI_RECTDIM x2, WPI_RECTDIM y2 );
extern HPEN             _MyCreatePen( WPI_COLOUR );
extern short            _CreateSysMonoFnt( WPI_PRES );
extern HFONT            _GetSysMonoFnt( void );
extern short            _IsStockFont( void );
extern long             _GetPresHeight( void );
extern void             _SetPresHeight( long );
extern void             _SetInst( WPI_INST* );
extern WPI_INST         _GetInst( void );
extern HFONT            _MySelectFont( WPI_PRES, HFONT );
extern void             _MyGetOldFont( WPI_PRES, HFONT );
extern void             _Set_RGB_COLOR( short, WPI_COLOUR );
extern WPI_COLOUR       _Get_RGB_COLOR( short );

/* external functions */
extern int              _MessageLoop( BOOL );
