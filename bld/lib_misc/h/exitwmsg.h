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
* Description:  Fatal runtime error handlers.
*
****************************************************************************/


#ifndef _EXITWMSG_H_INCLUDED
#define _EXITWMSG_H_INCLUDED

#ifdef __cplusplus
    extern "C" {
#endif

// C interface
// - tracks normal calling convention
// - this is the funtion that is called from ASM and from C, C++
// - note there is no #pragma aborts so that debugger can trace out
_WCRTLINK extern void           __exit_with_msg( char _WCI86FAR *, unsigned );
_WCRTLINK extern void           __fatal_runtime_error( char _WCI86FAR *, unsigned );
_WCRTLINK extern void           _Not_Enough_Memory( void );
_WCRTLINK _NORETURN extern void __exit( unsigned );

// ASM interface
// - always uses register calling convention
// - this function is only called from the C implementation
//   of __exit_with_msg
#undef _EWM_PARM1
#undef _EWM_PARM2
#if defined(__386__)
    #define _EWM_PARM1  eax
    #define _EWM_PARM2  edx
#elif defined( _M_I86 )
    #define _EWM_PARM1  ax dx
    #define _EWM_PARM2  bx
#else
    #define _EWM_PARM1
    #define _EWM_PARM2
#endif

_NORETURN extern void __do_exit_with_msg( char _WCI86FAR *, unsigned );
#ifdef _M_IX86
    #pragma aux __do_exit_with_msg "*__" parm caller [_EWM_PARM1] [_EWM_PARM2];
#endif

#undef _EWM_PARM1
#undef _EWM_PARM2

// WVIDEO interface

_WCRTDATA extern char volatile __WD_Present;

// this function should be called before __exit_with_msg()
// to allow Watcom Debugger (nee WVIDEO) to trap runtime errors.
// this really needs to be far!!!
_WCRTLINK extern int __EnterWVIDEO( char _WCFAR *string );

#ifdef __cplusplus
    };
#endif
#endif
