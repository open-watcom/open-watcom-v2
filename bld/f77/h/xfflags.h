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


// NOTE: Be sure to keep this file up to date with "lg86\rt\asm\xfflags.inc".

// The following are for the load'n go compiler.
// Keep this list uptodate with the list in "lg86\rt\c\limiterr.c".

#define XF_ERR_MASK             0x000f  // mask off the error type
#define XF_GET_ERR( __xf )      ( (__xf) & XF_ERR_MASK )
#define XF_KO_INTERRUPT         0x0000  // LimitErr
#define XF_KO_FOVERFLOW         0x0001  // LimitErr
#define XF_KO_FUNDERFLOW        0x0002  // LimitErr
#define XF_KO_FDIV_ZERO         0x0003  // LimitErr
#define XF_LI_ARG_NEG           0x0004  // LimitErr
#define XF_KO_TIME_OUT          0x0005  // LimitErr
#define XF_KO_IOVERFLOW         0x0006  // LimitErr
#define XF_CP_TERMINATE         0x0007  // LimitErr

// Exception Flags

#define XF_IOVERFLOW            0x0010  // integer overflow
#define XF_FATAL_ERROR          0x0020  // fatal error
#define XF_LIMIT_ERR            0x0040  // indicates one of the LimitErrs
#define XF_IO_INTERRUPTED       0x0080  // user interrupt during I/O
#define XF_IO_INTERRUPTABLE     0x0100  // allow interrupts in I/O system

#if defined( __MT__ )

    #include "fthread.h"

    #define     __XcptFlags     (__FTHREADDATAPTR->__XceptionFlags)

#else

    extern      volatile unsigned short __XcptFlags;

#endif
