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
* Description:  __CVTBuffer() implementation.
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <stdlib.h>
#include "liballoc.h"
#include "rtdata.h"
#include "exitwmsg.h"
#include "xfloat.h"

#if defined(__SW_BM)
    #include "thread.h"
#else
    #include "maxchtyp.h"
    static MAX_CHAR_TYPE    cvt_buffer[ __FPCVT_BUFFERLEN + 1 ];
#endif

_WCRTLINK CHAR_TYPE *__CVTBuffer( void )
{
    return( (CHAR_TYPE *)_RWD_cvtbuf );
}
