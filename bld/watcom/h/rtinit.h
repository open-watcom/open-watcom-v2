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
* Description:  Structures for run-time initialization/finalization.
*
****************************************************************************/


#ifndef __RTINIT_H__
#define __RTINIT_H__

#include "langenvd.h"
#if defined( __PPC__ )
    #define   __TGT_SYS     __TGT_SYS_PPC_NT
    typedef unsigned        __type_rtp;
    typedef unsigned        __type_pad;
    typedef void(           *__type_rtn ) ( void );
#elif defined( __AXP__ )
    #define   __TGT_SYS     __TGT_SYS_AXP_NT
    typedef unsigned        __type_rtp;
    typedef unsigned        __type_pad;
    typedef void(           *__type_rtn ) ( void );
#elif defined( __MIPS__ )
    #define   __TGT_SYS     __TGT_SYS_MIPS
    typedef unsigned        __type_rtp;
    typedef unsigned        __type_pad;
    typedef void(           *__type_rtn ) ( void );
#else
    #define   __TGT_SYS     __TGT_SYS_X86
    typedef unsigned char   __type_rtp;
    typedef unsigned short  __type_pad;
  #if defined( __386__ )
    typedef void __near(    *__type_rtn ) ( void );
  #else
    typedef void(           *__type_rtn ) ( void );
  #endif
#endif
#include "langenv.h"

#if defined( __MEDIUM__ ) || defined( __LARGE__ ) || defined( __HUGE__ )
    #define __LARGE_CODE__
#endif

// initialization progresses from highest priority to lowest
// finalization progresses from lowest to highest
#include "pushpck1.h"
struct rt_init // structure placed in XI/YI segment
{
    __type_rtp  rtn_type; // - near=0/far=1 routine indication
                          //   also used when walking table to flag
                          //   completed entries
    __type_rtp  priority; // - priority (0-highest 255-lowest)
    __type_rtn  rtn;      // - routine
#if !( defined( __LARGE_CODE__ ) || defined( __386__ ) ) || defined( COMP_CFG_COFF )
    __type_pad  padding;  // - padding, when small code ptr
                          //   or when risc cpu
#endif
};
#include "poppck.h"

#if defined( _M_I86 )
  #if defined( __LARGE_CODE__ ) /* segmented large code models */
    #define YIXI( seg, label, routine, priority )               \
        struct rt_init __based( __segname( seg ) ) label =      \
        { 1, priority, routine };
  #else                         /* other segmented models */
    #define YIXI( seg, label, routine, priority )               \
        struct rt_init __based( __segname( seg ) ) label =      \
        { 0, priority, routine, 0 };
  #endif
#else                           /* non-segmented architectures */
    #define YIXI( seg, label, routine, priority )               \
        struct rt_init __based( __segname( seg ) ) label =      \
        { 0, priority, routine };
#endif

/*
    Use these when you want a global label for the XI/YI structure
*/
#define XI( label, routine, priority ) YIXI( TS_SEG_XI, label, routine, priority )
#define YI( label, routine, priority ) YIXI( TS_SEG_YI, label, routine, priority )

/*
    Use these when you don't care about the label on the XI/YI structure
*/
#define __ANON( x )     __anon ## x
#define ANON( x )       __ANON( x )
#define AXI( routine, priority ) static XI( ANON( __LINE__ ), routine, priority )
#define AYI( routine, priority ) static YI( ANON( __LINE__ ), routine, priority )

/* ..._PRIORITY_... constants definition */
#include "rtprior.h"

/* have to turn off, or we get unref'd warnings for AXI & AYI stuff */
#pragma off( unreferenced )

#endif
