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


#ifndef DWABORT_H_INCLUDED
#define DWABORT_H_INCLUDED

#include <setjmp.h>

#define _Abort( __reason )      longjmp( cli->exception_handler,  __reason )

enum _Abort_reasons {
    ABORT_INVALID_LANGUAGE = 1,
    ABORT_ILLEGAL_LOC_OP,
    ABORT_EMPTY_INCLUDE_STACK,
    ABORT_INVALID_POINTER_TYPE,
    ABORT_INVALID_ARRAY_BOUND,
    ABORT_LOC_EXPR_TOO_LONG,
    ABORT_INVALID_DEFAULT_VALUE,
    ABORT_LOC_JUMP_OUT_OF_RANGE,
    ABORT_TOO_MANY_HANDLES,
    ABORT_SEG_SIZE_NOT_SET
};

#endif
