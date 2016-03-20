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


// KLUGES.H
//
// philosophy: when a kluge is required, include it here with a #define to
//             enable it
//
// for example: #define KLUGE_CPPLIB_END    // need kluge at end of CPPLIB
//              #include "kluges.h"
//
//      kluges.h would have a fragment:
//
//              #ifdef KLUGE_CPPLIB_END
//                  ... code for kluge
//                  #undef KLUGE_CPPLIB_END
//              #endif
//
// 93/03/23     J.W.Welch       Reluctantly, defined
// 93/07/29     A.F.Scian       reduced number of kludges by using __pragma()
//                              syntax to label functions as aborting
// 93/10/01     A.F.Scian       reduced number of kludges by fixing problem
//                              in CLIB where caling convention of __exit
//                              and __exit_with_msg didn't match the default
// 93/11/08     Greg Bentz      add pragma to suppress integer truncation
//                              warning temporarily until warning is fixed
// 93/11/11     Greg Bentz      use exitwmsg.h out of the c library to handle
//                              properties of __exit_with_msg()
// 93/11/15     Greg Bentz      remove exitwmsg.h, __exit_with_msg() is an
//                              ordinary function in the C library
// 95/02/27     J.W.Welch       moved definition of RTN_ABORTS here
//                              disabled for alpha

// KLUGE: at end of CPPLIB.H
// *************************
// - CLIB depends upon #pragma in source for __exit
//
#ifdef KLUGE_CPPLIB_END
  #ifdef _M_IX86
  #endif
    #undef KLUGE_CPPLIB_END
#endif
