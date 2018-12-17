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


#define inp(x)      in_b(x)
#define outp(x,y)   out_b(x,y)

extern byte     in_b(word);
#pragma aux in_b = \
        "in   al,dx"    \
    __parm __routine    [__dx] \
    __value             [__al]

extern word     in_w(word);
#pragma aux in_w = \
        "in   ax,dx"    \
    __parm __routine    [__dx] \
    __value             [__ax]

extern dword    in_d(word);
#pragma aux in_d = \
        "in   eax,dx"   \
    __parm __routine    [__dx] \
    __value             [__eax]

extern void     out_b(word,byte);
#pragma aux out_b = \
        "out  dx,al"    \
    __parm __routine    [__dx] [__al]

extern void     out_w(word,word);
#pragma aux out_w = \
        "out  dx,ax"    \
    __parm __routine    [__dx] [__ax]

extern void     out_d(word,dword);
#pragma aux out_d = \
        "out  dx,eax"   \
    __parm __routine    [__dx] [__eax]

extern void     _enable(void);
#pragma aux _enable = \
        "sti"

extern void     _disable(void);
#pragma aux _disable = \
        "cli"
