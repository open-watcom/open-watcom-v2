/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2024      The Open Watcom Contributors. All Rights Reserved.
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


#if defined(__NT__)
        /*
         * We have direct port I/O for NT and Win9x
         */
        extern unsigned outp( unsigned, unsigned );
        extern unsigned inp( unsigned );
#elif defined(__OS2__)
  #ifndef NOREMAPIO
    #if defined( _M_I86 )
        #define inp input_port
        #define outp output_port
    #else
        #define inp input_port
        #define outp output_port
    #endif
  #endif
    #ifdef _M_I86
        extern unsigned __far output_port( unsigned, unsigned );
        extern unsigned __far input_port( unsigned );
    #else
        extern unsigned short __far16 __pascal DosPortAccess( unsigned short, unsigned short, unsigned short, unsigned short );
        extern unsigned short __far16 __pascal output_port( unsigned short, unsigned short );
        extern unsigned short __far16 __pascal input_port( unsigned short );
    #endif
#elif defined(__NETWARE__)
    extern char inp( int );
    #pragma aux inp = "in al,dx" \
        __parm __routine    [__dx] \
        __value             [__al]
    extern void outp( int, int );
    #pragma aux outp = "out dx,al" \
        __parm __routine [__dx] [__ax]
#else
    #include <conio.h>
#endif
