/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Definition used for memory size and rounding
*
****************************************************************************/


#ifdef _M_I86
#define _1K             0x00000400UL
#define _2K             0x00000800UL
#define _4K             0x00001000UL
#define _8K             0x00002000UL
#define _16K            0x00004000UL
#define _32K            0x00008000UL
#define _64K            0x00010000UL
#define _1M             0x00100000UL
#define _4M             0x00400000UL
#define _16M            0x01000000UL
#define _256M           0x10000000UL
#else
#define _1K             0x00000400U
#define _2K             0x00000800U
#define _4K             0x00001000U
#define _8K             0x00002000U
#define _16K            0x00004000U
#define _32K            0x00008000U
#define _64K            0x00010000U
#define _1M             0x00100000U
#define _4M             0x00400000U
#define _16M            0x01000000U
#define _256M           0x10000000U
#endif

#ifdef _M_I86
#define PWROF2(r)       (1U<<(r))
#else
#define PWROF2(r)       (1UL<<(r))
#endif
#define PWROF2LL(r)     (1ULL<<(r))

/*
 * these are for size which is any value
 */
#define __ROUND_DOWN_SIZE_TO(__x,__amt)             ((__x)/(__amt))
#define __ROUND_UP_SIZE_TO(__x,__amt)               (((__x)+((__amt)-1))/(__amt))

#define __ROUND_DOWN_SIZE_ANY(__x,__amt)            (__ROUND_DOWN_SIZE_TO(__x,__amt)*(__amt))
#define __ROUND_UP_SIZE_ANY(__x,__amt)              (__ROUND_UP_SIZE_TO(__x,__amt)*(__amt))
/*
 * these are for size which is power of 2
 */
#define __ROUND_DOWN_SIZE(__x,__amt)                ((__x)&(~((__amt)-1)))
#define __ROUND_UP_SIZE(__x,__amt)                  (((__x)+((__amt)-1))&(~((__amt)-1)))

#define __ROUND_DOWN_SIZE_EVEN(__x)                 __ROUND_DOWN_SIZE(__x,2)
#define __ROUND_UP_SIZE_EVEN(__x)                   __ROUND_UP_SIZE(__x,2)

#define __ROUND_DOWN_SIZE_WORD(__x)                 __ROUND_DOWN_SIZE(__x,2)
#define __ROUND_UP_SIZE_WORD(__x)                   __ROUND_UP_SIZE(__x,2)
#define __ROUND_DOWN_SIZE_DWORD(__x)                __ROUND_DOWN_SIZE(__x,4)
#define __ROUND_UP_SIZE_DWORD(__x)                  __ROUND_UP_SIZE(__x,4)
#define __ROUND_DOWN_SIZE_QWORD(__x)                __ROUND_DOWN_SIZE(__x,8)
#define __ROUND_UP_SIZE_QWORD(__x)                  __ROUND_UP_SIZE(__x,8)
#define __ROUND_DOWN_SIZE_PARA(__x)                 __ROUND_DOWN_SIZE(__x,16)
#define __ROUND_UP_SIZE_PARA(__x)                   __ROUND_UP_SIZE(__x,16)
#define __ROUND_DOWN_SIZE_PAGE(__x)                 __ROUND_DOWN_SIZE(__x,256)
#define __ROUND_UP_SIZE_PAGE(__x)                   __ROUND_UP_SIZE(__x,256)
#define __ROUND_DOWN_SIZE_4K(__x)                   __ROUND_DOWN_SIZE(__x,_4K)
#define __ROUND_UP_SIZE_4K(__x)                     __ROUND_UP_SIZE(__x,_4K)
#define __ROUND_DOWN_SIZE_64K(__x)                  __ROUND_DOWN_SIZE(__x,_64K)
#define __ROUND_UP_SIZE_64K(__x)                    __ROUND_UP_SIZE(__x,_64K)

#define __ROUND_DOWN_SIZE_TO_PARA(__x)              __ROUND_DOWN_SIZE_TO((__x),16)
#define __ROUND_UP_SIZE_TO_PARA(__x)                __ROUND_UP_SIZE_TO((__x),16)

#define __ROUND_DOWN_SIZE_TO_4K(__x)                __ROUND_DOWN_SIZE_TO((__x),_4K)
#define __ROUND_UP_SIZE_TO_4K(__x)                  __ROUND_UP_SIZE_TO((__x),_4K)

#define __ROUND_UP_SIZE_PWROF2(__x,__pwr)           __ROUND_UP_SIZE(__x,PWROF2(__pwr))
#define __ROUND_UP_SIZE_PWROF2LL(__x,__pwr)         __ROUND_UP_SIZE(__x,PWROF2LL(__pwr))

#define __ROUND_DOWN_SIZE_TO_PWROF2LL(__x,__pwr)    __ROUND_DOWN_SIZE_TO(__x,PWROF2LL(__pwr))
#define __ROUND_UP_SIZE_TO_PWROF2LL(__x,__pwr)      __ROUND_UP_SIZE_TO(__x,PWROF2LL(__pwr))
