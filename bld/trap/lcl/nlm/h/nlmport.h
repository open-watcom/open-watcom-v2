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


#pragma aux in_b =                                              \
0xec                    /* in   al,dx                           */      \
        parm    routine [ dx ];


#pragma aux in_w =                                              \
0x66 0xed               /* in   ax,dx                           */      \
        parm    routine [ dx ];


#pragma aux in_d =                                              \
0x66 0xed               /* in   eax,dx                          */      \
        parm    routine [ dx ];


#pragma aux out_b =                                             \
0xee                    /* out  dx,al                           */      \
        parm    routine [ dx ] [ ax ];


#pragma aux out_w =                                             \
0x66 0xef               /* out  dx,ax                           */      \
        parm    routine [ dx ] [ ax ];

#pragma aux out_d =                                             \
0xef                    /* out  dx,eax                          */      \
        parm    routine [ dx ] [ eax ];

#pragma aux _enable = \
0xfb;                   /* sti */

#pragma aux _disable = \
0xfa;                   /* cli */


extern byte in_b(word);
extern word  in_w(word);
extern dword  in_d(word);
extern void out_b(word,byte);
extern void out_w(word,word);
extern void out_d(word,dword);

#define inp(x) in_b(x)
#define outp(x,y) out_b(x,y)

extern void _enable(void);
extern void _disable(void);
