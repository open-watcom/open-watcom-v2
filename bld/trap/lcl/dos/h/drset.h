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


#define _SET_DR         0x66 0xc1 0xe2 0x10     /* shl     edx,10H */ \
                        0x66 0x0f 0xb7 0xc0     /* movzx   eax,ax */ \
                        0x66 0x0b 0xc2          /* or      eax,edx */ \
                        0x0f 0x23               /* mov     dr?,eax */

#define _GET_DRa        0x0f 0x21               /* mov     eax,dr? */
#define _GET_DRb        0x8b 0xd0               /* mov     dx,ax */ \
                        0x66 0xc1 0xe8 0x10     /* shl     eax,16 */ \
                        0x92                    /* xchg    dx,ax */

extern void SetDR0( unsigned long );
extern void SetDR1( unsigned long );
extern void SetDR2( unsigned long );
extern void SetDR3( unsigned long );
extern void SetDR6( unsigned long );
extern void SetDR7( unsigned long );

extern unsigned long GetDR0(void);
extern unsigned long GetDR1(void);
extern unsigned long GetDR2(void);
extern unsigned long GetDR3(void);
extern unsigned long GetDR6(void);
extern unsigned long GetDR7(void);

#pragma aux SetDR0 = _SET_DR 0xc0 parm [ dx ax ];
#pragma aux SetDR1 = _SET_DR 0xc8 parm [ dx ax ];
#pragma aux SetDR2 = _SET_DR 0xd0 parm [ dx ax ];
#pragma aux SetDR3 = _SET_DR 0xd8 parm [ dx ax ];
#pragma aux SetDR6 = _SET_DR 0xf0 parm [ dx ax ];
#pragma aux SetDR7 = _SET_DR 0xf8 parm [ dx ax ];

#pragma aux GetDR0 = _GET_DRa 0xc0 _GET_DRb value [ dx ax ];
#pragma aux GetDR1 = _GET_DRa 0xc8 _GET_DRb value [ dx ax ];
#pragma aux GetDR2 = _GET_DRa 0xd0 _GET_DRb value [ dx ax ];
#pragma aux GetDR3 = _GET_DRa 0xd8 _GET_DRb value [ dx ax ];
#pragma aux GetDR6 = _GET_DRa 0xf0 _GET_DRb value [ dx ax ];
#pragma aux GetDR7 = _GET_DRa 0xf8 _GET_DRb value [ dx ax ];

