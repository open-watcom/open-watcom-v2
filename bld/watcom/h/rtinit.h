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


#ifndef __RTINIT_H__
#define __RTINIT_H__

// RTINIT.H -- structure for run-time initialization/finalization
//
// 93/01/22     -- J.W.Welch        -- defined
// 95/03/07     -- Greg Bentz       -- generalize for AXP
// 95/12/01     -- Greg Bentz       -- categorize shutdown sequencing

#include "langenvd.h"
#if defined(__PPC__)
  #define   __TGT_SYS   __TGT_SYS_AXP_PPC
  typedef unsigned      __type_rtp;
  typedef unsigned      __type_pad;
  typedef void          (*__type_rtn)( void );
#elif defined(__AXP__)
  #define   __TGT_SYS   __TGT_SYS_AXP_NT
  typedef unsigned      __type_rtp;
  typedef unsigned      __type_pad;
  typedef void          (*__type_rtn)( void );
#else
  #define   __TGT_SYS   __TGT_SYS_X86
  typedef unsigned char __type_rtp;
  typedef unsigned short __type_pad;
  #if defined(__386__)
    typedef void __near (* __type_rtn )( void );
  #else
    typedef void        (* __type_rtn )( void );
  #endif
#endif
#include "langenv.h"

#if defined(__MEDIUM__) || defined(__LARGE__) || defined(__HUGE__)
  #define __LARGE_CODE__
#endif


// initialization progresses from highest priority to lowest
// finalization progresses from lowest to highest
#pragma pack(1);
struct rt_init                  // structure placed in XI/YI segment
{
    __type_rtp rtn_type;        // - near=0/far=1 routine indication
                                //   also used when walking table to flag
                                //   completed entries
    __type_rtp priority;        // - priority (0-highest 255-lowest)
    __type_rtn rtn;             // - routine
#if ! ( defined(__LARGE_CODE__) || defined(__386__) ) || defined( COMP_CFG_COFF )
    __type_pad padding;         // - padding, when small code ptr
                                //   or when risc cpu
#endif
};
#pragma pack();

#if defined(__386__) || defined(__AXP__) || defined(__PPC__)

  #define YIXI( seg, label, routine, priority )             \
    struct rt_init __based( __segname( seg ) ) label =      \
    { 0, priority, routine };

#elif defined(__LARGE_CODE__)

  #define YIXI( seg, label, routine, priority )             \
    struct rt_init __based( __segname( seg ) ) label =      \
    { 1, priority, routine };


#else

  #define YIXI( seg, label, routine, priority )             \
    struct rt_init __based( __segname( seg ) ) label =      \
    { 0, priority, routine, 0 };

#endif

/*
        Use these when you want a global label for the XI/YI structure
*/
#define XI( label, routine, priority ) YIXI( TS_SEG_XI, label, &routine, priority )
#define YI( label, routine, priority ) YIXI( TS_SEG_YI, label, &routine, priority )

/*
        Use these when you don't care about the label on the XI/YI structure
*/
#define __ANON( x )     __anon ## x
#define ANON( x )       __ANON( x )
#define AXI( routine, priority ) static XI( ANON( __LINE__ ), routine, priority )
#define AYI( routine, priority ) static YI( ANON( __LINE__ ), routine, priority )

#define INIT_PRIORITY_THREAD     1 // priority for thread data init
#define INIT_PRIORITY_FPU        2 // priority for FPU/EMU init
#define INIT_PRIORITY_RUNTIME   10 // priority for run/time initialization
#define INIT_PRIORITY_IOSTREAM  20 // priority for IOSTREAM
#define INIT_PRIORITY_LIBRARY   32 // default library-initialization priority
#define INIT_PRIORITY_PROGRAM   64 // default program-initialization priority
#define FINI_PRIORITY_DTOR      40 // priority for module DTOR
#define DTOR_PRIORITY           40 // priority for module DTOR
#define FINI_PRIORITY_EXIT      16 // when exit() is called, functions between
                                   // 255 and this are invoked, the rest of
                                   // the fini routines are invoked from
                                   // the __exit() function


/* have to turn off, or we get unref'd warnings for AXI & AYI stuff */
#pragma off(unreferenced)

#endif
