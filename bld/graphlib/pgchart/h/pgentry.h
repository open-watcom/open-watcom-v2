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


// Alternate entry points for WATFOR-77

// Functions are in uppercase, do not have underscores on the end, and
// perform a jump to the appropriate function.


#if defined( VERSION2 )
  #define _PGCHART_ALT_ENABLED  0
#elif defined( __386__ )
  #define _PGCHART_ALT_ENABLED  0       // not necessary for 32-bit
#else
  #define _PGCHART_ALT_ENABLED  1
#endif

#if _PGCHART_ALT_ENABLED
  #define Entry2( f1, f2 )      extern void f1( void ); void f1( void ) { JMP ## f2(); }
#else
  #define Entry2( f1, f2 )
#endif

#if _PGCHART_ALT_ENABLED

void JMP_pg_analyzechart();
void JMP_pg_analyzechartms();
void JMP_pg_analyzepie();
void JMP_pg_analyzescatter();
void JMP_pg_analyzescatterms();
void JMP_pg_chart();
void JMP_pg_chartms();
void JMP_pg_chartpie();
void JMP_pg_chartscatter();
void JMP_pg_chartscatterms();
void JMP_pg_defaultchart();
void JMP_pg_getchardef();
void JMP_pg_getpalette();
void JMP_pg_getstyleset();
void JMP_pg_hlabelchart();
void JMP_pg_initchart();
void JMP_pg_resetpalette();
void JMP_pg_resetstyleset();
void JMP_pg_setchardef();
void JMP_pg_setpalette();
void JMP_pg_setstyleset();
void JMP_pg_vlabelchart();

#pragma aux JMP_pg_analyzechart         = 0xe9 offset _pg_analyzechart;
#pragma aux JMP_pg_analyzechartms       = 0xe9 offset _pg_analyzechartms;
#pragma aux JMP_pg_analyzepie           = 0xe9 offset _pg_analyzepie;
#pragma aux JMP_pg_analyzescatter       = 0xe9 offset _pg_analyzescatter;
#pragma aux JMP_pg_analyzescatterms     = 0xe9 offset _pg_analyzescatterms;
#pragma aux JMP_pg_chart                = 0xe9 offset _pg_chart;
#pragma aux JMP_pg_chartms              = 0xe9 offset _pg_chartms;
#pragma aux JMP_pg_chartpie             = 0xe9 offset _pg_chartpie;
#pragma aux JMP_pg_chartscatter         = 0xe9 offset _pg_chartscatter;
#pragma aux JMP_pg_chartscatterms       = 0xe9 offset _pg_chartscatterms;
#pragma aux JMP_pg_defaultchart         = 0xe9 offset _pg_defaultchart;
#pragma aux JMP_pg_getchardef           = 0xe9 offset _pg_getchardef;
#pragma aux JMP_pg_getpalette           = 0xe9 offset _pg_getpalette;
#pragma aux JMP_pg_getstyleset          = 0xe9 offset _pg_getstyleset;
#pragma aux JMP_pg_hlabelchart          = 0xe9 offset _pg_hlabelchart;
#pragma aux JMP_pg_initchart            = 0xe9 offset _pg_initchart;
#pragma aux JMP_pg_resetpalette         = 0xe9 offset _pg_resetpalette;
#pragma aux JMP_pg_resetstyleset        = 0xe9 offset _pg_resetstyleset;
#pragma aux JMP_pg_setchardef           = 0xe9 offset _pg_setchardef;
#pragma aux JMP_pg_setpalette           = 0xe9 offset _pg_setpalette;
#pragma aux JMP_pg_setstyleset          = 0xe9 offset _pg_setstyleset;
#pragma aux JMP_pg_vlabelchart          = 0xe9 offset _pg_vlabelchart;

#pragma aux _PG_ANALYZECHART "*";
#pragma aux _PG_ANALYZECHARTMS "*";
#pragma aux _PG_ANALYZEPIE "*";
#pragma aux _PG_ANALYZESCATTER "*";
#pragma aux _PG_ANALYZESCATTERMS "*";
#pragma aux _PG_CHART "*";
#pragma aux _PG_CHARTMS "*";
#pragma aux _PG_CHARTPIE "*";
#pragma aux _PG_CHARTSCATTER "*";
#pragma aux _PG_CHARTSCATTERMS "*";
#pragma aux _PG_DEFAULTCHART "*";
#pragma aux _PG_GETCHARDEF "*";
#pragma aux _PG_GETPALETTE "*";
#pragma aux _PG_GETSTYLESET "*";
#pragma aux _PG_HLABELCHART "*";
#pragma aux _PG_INITCHART "*";
#pragma aux _PG_RESETPALETTE "*";
#pragma aux _PG_RESETSTYLESET "*";
#pragma aux _PG_SETCHARDEF "*";
#pragma aux _PG_SETPALETTE "*";
#pragma aux _PG_SETSTYLESET "*";
#pragma aux _PG_VLABELCHART "*";

#endif
