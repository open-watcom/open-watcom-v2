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


#include <malloc.h>
#include "gdefn.h"
#include "stkavail.h"
#include "gbios.h"


/*  Use PASCAL pragma to define our convention for
    calling the 'compiled' line drawing routine.    */

#define LINE_FUNC   pascal

#if defined( __386__ )
    #pragma aux pascal "*" parm caller [es edi] [eax] [ebx] [ecx] [edx] [esi];
    #define MAXLEN      25
    #if defined( __QNX__ )
      #define LINERET     0xCB  // QNX uses a segmented 32 bit model
    #else
      #define LINERET     0xC3
    #endif
#else
    #pragma aux pascal "*" far parm caller [es di] [ax] [bx] [cx] [dx] [si];
    #define MAXLEN      20
    #define LINERET     0xCB
#endif


#if defined( __QNX__ )
    #define COMP_FAR    _far            // compile into another segment
    #define FUNC_FAR    _far            // code segment is always far for QNX
#else
    #define COMP_FAR                    // compile onto the stack
    #define FUNC_FAR    _WCI86FAR               // near for 32-bit/far for 16-bit
#endif


#define OutByte( p )    *stack++ = p;
#define OutInt( p )     *( (unsigned int COMP_FAR *)stack ) = p; \
                        stack += sizeof( int );


void _L0DrawLine( char far *screen_ptr, short color, short style,
/*=============*/ short bit_mask, short majordif, short minordif,
                  void (near *majorfn)(), void (near *minorfn)(),
                  void (near *plot)() )

/*  This function 'compiles' the line drawing routine on the stack.
    The corresponding assembler pseudo-code is:

    L1:         rol     bx,1                         ... check line style
                jnc     L2
    ( L1: )     [ inline code for plot function ]
    L2:         dec     dx
                jl      L3
                [ inline code for major function ]
                sub     si,minordif
                jg      L1
                add     si,majordif
                [ inline code for minor function ]
                jmp     L1
    L3:         retf
*/

{
    short               size;
    char                plot_len;
    char                minor_len;
    char                major_len;
    char COMP_FAR       *stack;
    char COMP_FAR       *start;
    void LINE_FUNC      (FUNC_FAR * line)();

    plot_len = *( (char FUNC_FAR *)plot - 1 );
    minor_len = *( (char FUNC_FAR *)minorfn - 1 );
    major_len = *( (char FUNC_FAR *)majorfn - 1 );
    size = _RoundUp( plot_len + minor_len + major_len + MAXLEN );

#if defined( __QNX__ )
    stack = MK_FP( _CompileBuf, 0 );
    line = MK_FP( _CompileSeg, 0 );     // shadow seg which is executable
#else
    if( _stackavail() - size > 0x100 ) {
        stack = __alloca( size );
    } else {
        _ErrorStatus = _GRINSUFFICIENTMEMORY;
        return;         /* not enough memory to proceed */
    }
    #if defined( __386__ )
        line = (void LINE_FUNC *) stack;
    #else
        line = MK_FP( _StackSeg, FP_OFF( stack ) );
    #endif
#endif

    start = stack;              /* save start address of compiled routine   */
    if( style != -1 ) {             /* add instructions for a style line    */
        #if defined ( __386__ )
            OutByte( 0x66 );                    /* rotate only bx, not ebx  */
        #endif
        OutByte( 0xD1 );                        /* rol      bx,1    */
        OutByte( 0xC3 );
        OutByte( 0x73 );                        /* jnc      xxx     */
        OutByte( plot_len );
    }
    movedata( FP_SEG( plot ), FP_OFF( plot ),       /* copy plot routine    */
              FP_SEG( stack ), FP_OFF( stack ), plot_len );
    stack += plot_len;
    OutByte( 0x4A );                            /* dec      dx      */
    OutByte( 0x7C );                            /* jl       xxx     */
    OutByte( major_len + minor_len + 8 + ( sizeof( int ) << 1 ) );
    movedata( FP_SEG( majorfn ), FP_OFF( majorfn ), /* copy major function  */
              FP_SEG( stack ), FP_OFF( stack ), major_len );
    stack += major_len;
    OutByte( 0x81 );                            /*  sub     si, minordif    */
    OutByte( 0xEE );
    OutInt( minordif );
    OutByte( 0x7F );                            /*  jg      xxx     */
    OutByte( start - stack - 1 );
    OutByte( 0x81 );                            /*  add     si, majordif    */
    OutByte( 0xC6 );
    OutInt( majordif );
    movedata( FP_SEG( minorfn ), FP_OFF( minorfn ), /* copy minor function  */
              FP_SEG( stack ), FP_OFF( stack ), minor_len );
    stack += minor_len;
    OutByte( 0xEB );                            /*  jmp     xxx     */
    OutByte( start - stack - 1 );
    OutByte( LINERET );
    (*line)( screen_ptr, color, style, bit_mask, majordif >> 1, majordif >> 1 );
}
