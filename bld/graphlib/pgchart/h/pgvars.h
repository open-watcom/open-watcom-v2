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


#include "_graph.h"
#include "_pgchart.h"
#include "pgstubs.h"

#if defined ( __386__ )
    #define _WCI86FAR
    #define _HUGE
    #define StringLen   strlen
    #define StringCopy  strcpy
    #define MemorySet   memset
    #define MemoryCopy  memcpy
#else
    #define _WCI86FAR    __far
    #define _HUGE   __huge
    #define StringLen   _fstrlen
    #define StringCopy  _fstrcpy
    #define MemorySet   _fmemset
    #define MemoryCopy  _fmemcpy
#endif

#include "fpi.h"
#include "entry.h"
#include "pgentry.h"

enum {
    FALSE, TRUE
};

enum {
    _XVECTOR, _YVECTOR
};

/*  Global declarations for presentation graphics include files */

extern  styleset        _Style;
extern  palettetype     _PGPalette;
extern  short           _CharHeight;
extern  short           _CharWidth;

extern  void            _Axes( chartenv _WCI86FAR *, char _WCI86FAR *_WCI86FAR *, short );
extern  void            _CalcLegendWindow( chartenv _WCI86FAR *, char _WCI86FAR *_WCI86FAR *, short );
extern  void            _Chartwindow( windowtype _WCI86FAR * );
extern  void            _CommonSetup( chartenv _WCI86FAR *, char _WCI86FAR *_WCI86FAR *, short );
extern  short           _DrawLegendWindow( chartenv _WCI86FAR *, char _WCI86FAR *_WCI86FAR *, short );
extern  short           _MaxLabelLength( char _WCI86FAR *_WCI86FAR *, short, short );
extern  short           _sLabelLength( char _WCI86FAR *, short );
extern  short           _MaxValueLength( float _WCI86FAR *values, short, short );
extern  short           _MaxLabelWidth( char _WCI86FAR *_WCI86FAR *, short, short );
extern  short           _sMaxLabelWidth( char _WCI86FAR *, short );
extern  short           _analyze_common( chartenv _WCI86FAR *, float _WCI86FAR *, float _WCI86FAR *,
                                        short, short, short, char _WCI86FAR *_WCI86FAR * );
extern  short           _analyze_datawindow( chartenv _WCI86FAR *,
                                                char _WCI86FAR *_WCI86FAR *, short );
extern  void            _pg_autoscale( short, short, axistype _WCI86FAR *, float _WCI86FAR *,
                                       short, short, short );
extern  void            _pg_line( short, short, short, short, short, short );
extern  float           _pg_scale( axistype _WCI86FAR *, float );
extern  void            _pg_window( windowtype _WCI86FAR * );
extern  short           _StringPrint( char *, short, short, float );
extern  void            _Title( chartenv _WCI86FAR * );
extern  short           _XHeight( chartenv _WCI86FAR *, float );
extern  short           _YHeight( chartenv _WCI86FAR *, float );
