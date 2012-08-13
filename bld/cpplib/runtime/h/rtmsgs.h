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


#ifndef __RTMSGS_H__
#define __RTMSGS_H__

// RUN-TIME ERROR MESSAGES

#define RTMSG_PURE_ERR          \
    "pure virtual function called!"
#define RTMSG_NO_HANDLER        \
    "no handler active to catch thrown object!"
#define RTMSG_RET_UNEXPECT      \
    "return from \"unexpected\" function!"
#define RTMSG_RET_TERMIN        \
    "return from \"terminate\" function!"
#define RTMSG_THROW_TERMIN      \
    "throw while \"terminate\" function active!"
#define RTMSG_THROW_DTOR        \
    "throw during destructor while unwinding stack to catch exception!"
#define RTMSG_THROW_CTOR        \
    "throw during construction of exception!"
#define RTMSG_CORRUPT_STK       \
    "stack data has been corrupted!"
#define RTMSG_FNEXC             \
    "violation of function exception specification!"
#define RTMSG_EXC_NO_SPACE      \
    "no memory left to handle thrown exception!"
#define RTMSG_EXC_DTOR          \
    "throw during destructor for handled exception!"
#define RTMSG_RETHROW           \
    "re-throw when no exception handler active!"
#define RTMSG_UNDEFED_CDTOR     \
    "undefined constructor or destructor called!"
#define RTMSG_UNDEFED_MEMBER     \
    "undefined member function called through debug information!"
#define RTMSG_UNDEFED_VFUN      \
    "compiler error: eliminated virtual function call!"
#define RTMSG_SYS_EXC           \
    "system exception! code = 0x00000000"

#endif
