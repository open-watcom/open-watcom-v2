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


#include "variety.h"
#include <math.h>
#include "rtdata.h"

extern  void    __fclex(), __fstsw();

#if defined(__386__)
#pragma aux     __fclex = 0xdb 0xe2;
#pragma aux     __fstsw = 0x36 0xdd 0x3f 0x9b parm caller [edi];
                        /* fstsw ss:[edi] */
                        /* fwait         */
#else
#pragma aux     __fclex = float 0x9b 0xdb 0xe2;
#pragma aux     __fstsw = 0x95                  /* xchg ax,bp */\
                          float 0x9b 0xdd 0x7e 0x00  /* fstsw 0[bp]*/\
                          float 0x9b 0xd9 0xd0  /* fnop       */\
                          0x95                  /* xchg ax,bp */\
                          parm caller [ax];
#endif


_WMRTLINK unsigned _status87()
/******************/
{
    auto int status;

    status = 0;
    if( _RWD_8087 ) {
        __fstsw( &status );
    }
    return( status );
}


_WMRTLINK unsigned _clear87()
/*****************/
{
    register int status;

    status = 0;
    if( _RWD_8087 ) {
        status = _status87();
        __fclex();
    }
    return( status );
}
