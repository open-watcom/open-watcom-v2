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


#include "variety.h"
#include <stddef.h>
#include <dos.h>
#if defined(__386__)
 #if defined(__WINDOWS_386__)
  #include "tinyio.h"
 #else
  #include "extender.h"
  extern  void pharlap_setvect( unsigned, void (interrupt _WCFAR *)());
  #pragma aux  pharlap_setvect =  0x1e   /* push ds   */\
                               0x8e 0xd9 /* mov ds,cx */\
                               0x88 0xc1 /* mov cl,al */\
                               0xb0 0x04 /* mov al,04 */\
                               0xb4 0x25 /* mov ah,25 */\
                               0xcd 0x21 /* int 21h   */\
                               0x1f      /* pop ds    */\
                        parm caller [al] [cx edx];

  extern  void os386_setvect( unsigned, void (interrupt _WCFAR *)());
  #pragma aux  os386_setvect =  0x1e      /* push ds   */\
                               0x8e 0xd9 /* mov ds,cx */\
                               0xb4 0x25 /* mov ah,25 */\
                               0xcd 0x21 /* int 21h   */\
                               0x1f      /* pop ds    */\
                        parm caller [al] [cx edx];
 #endif
#else
 extern  void _setvect( unsigned, void (interrupt _WCFAR *)());
 #pragma aux     _setvect =    0x1e      /* push ds   */\
                               0x8e 0xd9 /* mov ds,cx */\
                               0xb4 0x25 /* mov ah,25 */\
                               0xcd 0x21 /* int 21h   */\
                               0x1f      /* pop ds    */\
                        parm caller [ax] [cx dx];
#endif

_WCRTLINK void _dos_setvect( int intnum, void (interrupt _WCFAR *func)() )
    {
#if defined(__WINDOWS_386__)
        TinySetVect( intnum, (void _WCNEAR *) func );
#elif defined(__386__)
        if( _IsPharLap() ) {
            pharlap_setvect( intnum, func );
        } else {        /* os386 or DOS4G */
            os386_setvect( intnum, func );
        }
#else
        _setvect( intnum, func );
#endif
    }
