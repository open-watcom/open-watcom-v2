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


/*
Date            By              Reason
====            ==              ======
26-apr-91       A.F.Scian       changed MX_CEIL from 1024 to 63 to fix a bug
                                with corrupted sample files (the maximum size
                                for a sample record is 64k)
07-may-91       C.G.Eisler      windows specific stuff
19-nov-92       S.B.Feyler      added changes for NEC
*/
typedef unsigned short seg;
#include "offset.h"
#include "exeext.h"

typedef struct {
    off         offset;
    seg         segment;
} seg_offset;

#define MKMKSTR( data ) # data
#define MKSTR( data )   MKMKSTR( data )

#define PREFIX_STRING   "WATCOM Profiler data file V" \
                        MKSTR( SAMP_MAJOR_VER ) "." MKSTR( SAMP_MINOR_VER ) \
                        "\r\n\x1a"


#define LO_WORD( lng )      (unsigned)( lng )

#define MN_CEIL     1
#define MX_CEIL     63          /* in kilobytes (must never exceed 63k) */
#define DEF_CEIL    MX_CEIL

#if defined(_NEC_PC)
#define INT_CTRL        0x00    /* interrupt controller port */
#define INT_MASK        0x02    /* interrupt controller mask register */
#else
#define TIMER0          0x40    /* timer port */
#define INT_CTRL        0x20    /* interrupt controler port */
#endif
#define EOI             0x20    /* End-Of-Interrupt */

#undef TRUE
#undef FALSE
typedef enum { FALSE, TRUE } bool;

extern unsigned                 InsiderTime;
extern unsigned                 Margin;
extern unsigned                 Ceiling;
extern unsigned                 SamplerOff;

typedef struct {
    struct  samp_block_prefix   pref;
    struct  samp_info           d;
}   info_struct;
#ifndef __WINDOWS__
extern info_struct Info;
extern unsigned long            CurrTick;
extern bool                     FarWriteProblem;
extern unsigned                 SampleIndex;
extern bool                     LostData;
#endif

extern bool                     CallGraphMode;
extern unsigned                 SampleIndex;
extern unsigned                 LastSampleIndex;
extern unsigned                 SampleCount;
extern samp_block               FAR_PTR *Samples;
extern samp_block               FAR_PTR *CallGraph;
extern off                      CGraphOff;
extern seg                      CGraphSeg;

extern struct {
    unsigned char       in_hook;
    unsigned long       cgraph_top;
    off                 top_ip;
    seg                 top_cs;
    unsigned short      pop_no;
    unsigned short      push_no;
} Comm;

extern char                     ExeName[128];
extern char                     SampName[256];
