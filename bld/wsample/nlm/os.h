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


#include <dos.h>

#define outp(__x,__y)   _inline_outp(__x,__y)
extern unsigned outp(unsigned __port, unsigned __value);

#define __FILLER unsigned short : 16;

/* input parm to an 'interrupt' function is union INTPACK */
/* e.g.  interrupt int10( union INTPACK r ) {}            */

struct INTPACKX {
        unsigned gs,fs,es,ds,edi,esi,ebp,esp,ebx,edx,ecx,eax,eip,cs,flags;
};
struct INTPACKW {
        unsigned short gs;  __FILLER
        unsigned short fs;  __FILLER
        unsigned short es;  __FILLER
        unsigned short ds;  __FILLER
        unsigned short di;  __FILLER
        unsigned short si;  __FILLER
        unsigned short bp;  __FILLER
        unsigned short sp;  __FILLER
        unsigned short bx;  __FILLER
        unsigned short dx;  __FILLER
        unsigned short cx;  __FILLER
        unsigned short ax;  __FILLER
        unsigned short ip;  __FILLER
        unsigned short cs;  __FILLER
        unsigned flags;
};
struct INTPACKB {
        unsigned /*gs*/ :32,/*fs*/ :32,
                 /*es*/ :32,/*ds*/ :32,
                 /*edi*/:32,/*esi*/:32,
                 /*ebp*/:32,/*esp*/:32;
        unsigned char bl, bh; __FILLER
        unsigned char dl, dh; __FILLER
        unsigned char cl, ch; __FILLER
        unsigned char al, ah; __FILLER
};
union  INTPACK {
        struct INTPACKB h;
        struct INTPACKW w;
        struct INTPACKX x;
};

/* bits defined for flags field defined in REGPACKW and INTPACKW */

enum {
    INTR_CF     = 0x0001,       /* carry */
    INTR_PF     = 0x0004,       /* parity */
    INTR_AF     = 0x0010,       /* auxiliary carry */
    INTR_ZF     = 0x0040,       /* zero */
    INTR_SF     = 0x0080,       /* sign */
    INTR_TF     = 0x0100,       /* trace */
    INTR_IF     = 0x0200,       /* interrupt */
    INTR_DF     = 0x0400,       /* direction */
    INTR_OF     = 0x0800        /* overflow */
};

void __far *MK_FP(unsigned short,unsigned);
#pragma aux      MK_FP = parm caller [dx] [eax] value [eax dx];
