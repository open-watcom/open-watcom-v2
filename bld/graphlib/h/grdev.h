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
* Description:  Graphics device function vectors.
*
****************************************************************************/


/* Assembly code device functions calling convention declarations */

#if defined( _M_I86 )
  #if defined( VERSION2 )
    #pragma aux PUTDOT_FUNC  "*" __far __parm __caller [__es __di] [__dx __ax] [__cx]
    #pragma aux GETDOT_FUNC  "*" __far __parm __caller [__es __di] [__cx]
    #pragma aux FILL_FUNC    "*" __parm __caller [__es __di] [__si __ax] [__bx] [__cx] [__dx]
    #pragma aux COPY_FUNC    "*" __parm __caller [__es __di] [__si __ax] [__cx] [__dx] [__bx]
    //endx now on stack
    #pragma aux SCAN_FUNC    "*" __parm __caller [__es __di] [__si __ax] [__bx] [__cx] [__dx] __value [__bx]
  #else
    #pragma aux PUTDOT_FUNC  "*" __far __parm __caller [__es __di] [__ax] [__cx]
    #pragma aux GETDOT_FUNC  "*" __far __parm __caller [__es __di] [__cx]
    #pragma aux FILL_FUNC    "*" __parm __caller [__es __di] [__ax] [__bx] [__cx] [__dx]
    #pragma aux COPY_FUNC    "*" __parm __caller [__es __di] [__si __ax] [__cx] [__dx] [__bx]
    #pragma aux SCAN_FUNC    "*" __parm __caller [__es __di] [__ax] [__bx] [__cx] [__dx] [__si] __value [__bx]
  #endif
#else
    #pragma aux PUTDOT_FUNC  "*" __parm __caller [__es __edi] [__eax] [__ecx]
    #pragma aux GETDOT_FUNC  "*" __parm __caller [__es __edi] [__ecx]
    #pragma aux FILL_FUNC    "*" __parm __caller [__es __edi] [__eax] [__ebx] [__ecx] [__edx]
    #pragma aux COPY_FUNC    "*" __parm __caller [__es __edi] [__esi __eax] [__ecx] [__edx] [__ebx]
    #pragma aux SCAN_FUNC    "*" __parm __caller [__es __edi] [__eax] [__ebx] [__ecx] [__edx] [__esi] __value [__bx]
#endif

typedef grcolor __near getdot_fn( char __far *, int );
typedef void    __near putdot_fn( char __far *, grcolor, int );
typedef void    __near fill_fn( char __far *, grcolor, int, int, int );
typedef void    __near copy_fn( char __far *, char __far *, int, int, int );
typedef short   __near scan_fn( char __far *, grcolor, int, int, int, int );

typedef void    __near __pascal move_fn( void );

#pragma aux (GETDOT_FUNC)  getdot_fn;
#pragma aux (PUTDOT_FUNC)  putdot_fn;
#pragma aux (FILL_FUNC)    fill_fn;
#pragma aux (COPY_FUNC)    copy_fn;
#pragma aux (SCAN_FUNC)    scan_fn;

/* C code device functions calling convention declarations */

typedef short   init_fn( short );
typedef void    finish_fn( void );
typedef void    set_fn( void );
typedef void    reset_fn( void );
typedef void    setup_fn( short, short, grcolor );


#pragma pack(push, 1);
typedef struct graphics_device {
    init_fn         *init;          // initialization
    finish_fn       *finish;        // finish up device
    set_fn          *set;           // set device
    reset_fn        *reset;         // reset device
    setup_fn        *setup;         // setup routine
    move_fn         *up;            // move up routine
    move_fn         *left;          // move left routine
    move_fn         *down;          // move down routine
    move_fn         *right;         // move right routine
    putdot_fn       *plot[4];       // plot-replace
    getdot_fn       *getdot;        // get pixel color
    fill_fn         *zap;           // zap routine
    fill_fn         *fill;          // fill style routine
    copy_fn         *pixcopy;       // copy pixels
    copy_fn         *readrow;       // read a row of pixels
    scan_fn         *scanleft;      // left scan in paint
    scan_fn         *scanright;     // right scan in paint
} gr_device;
#pragma pack (pop);
