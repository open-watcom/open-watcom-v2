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
* Description:  Internal error handling macros.
*
****************************************************************************/


#ifndef MYASSERT_H
#if defined( __WATCOMC__ )
#pragma aux InternalError aborts;
#endif
extern int InternalError( const char *file, unsigned line );

#ifndef __FNAME__
#define __FNAME__ __FILE__
#endif

#ifdef NDEBUG
#   define never_reach()    ((void)0)
#   define myassert(expr)   ((void)0)
#else
#   define never_reach()    InternalError(__FNAME__,__LINE__)
#   define myassert(expr)   ((void)((expr)?0:InternalError(__FNAME__,__LINE__)))
#endif

#define MYASSERT_H  1
#endif
