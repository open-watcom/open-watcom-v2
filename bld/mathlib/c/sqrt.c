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
#include <stddef.h>
#include <math.h>
#include "ifprag.h"
#include "mathcode.h"
#include "rtdata.h"

#if defined(__386__)
 extern double __sqrtd(double);
 #pragma aux __sqrtd "*" parm [edx eax] value [edx eax];
 extern double  _sqrt87(double);
 #pragma aux    _sqrt87 = \
                0x55                    /* push ebp */\
                0x52 0x50               /* push edx,eax */\
                0x89 0xe5               /* mov ebp,esp */\
                0xdd 0x45 0x00          /* fld 0[ebp] */\
                0xd9 0xfa               /* fsqrt */\
                0xdd 0x5d 0x00          /* fstp 0[ebp] */\
                0x9b                    /* fwait */\
                0x58 0x5a               /* pop eax,edx */\
                0x5d                    /* pop ebp */\
                parm [edx eax] value [edx eax];
#elif defined(M_I86)
 extern double __sqrtd(double);
 #pragma aux __sqrtd "*" parm [ax bx cx dx] value [ax bx cx dx];
 extern double  _sqrt87(double);
 #pragma aux    _sqrt87 = \
                0x55                    /* push bp */\
                0x50 0x53 0x51 0x52     /* push ax,bx,cx,dx */\
                0x89 0xe5               /* mov bp,sp */\
                0xdd 0x46 0x00          /* fld 0[bp] */\
                0x9b                    /* fwait */\
                0xd9 0xfa               /* fsqrt */\
                0x9b                    /* fwait */\
                0xdd 0x5e 0x00          /* fstp 0[bp] */\
                0x9b                    /* fwait */\
                0x5a 0x59 0x5b 0x58     /* pop dx,cx,bx,ax */\
                0x5d                    /* pop bp */\
                parm [ax bx cx dx] value [ax bx cx dx];
#endif

_WMRTLINK extern double _IF_dsqrt( double );
#if defined(_M_IX86)
  #pragma aux (if_rtn) _IF_sqrt "IF@SQRT";
  #pragma aux (if_rtn) _IF_dsqrt "IF@DSQRT";
#endif

_WMRTLINK float _IF_sqrt( float x )
/*********************/
{
    return( _IF_dsqrt( x ) );
}

_WMRTLINK double (sqrt)( double x )
/***********************/
{
    return( _IF_dsqrt( x ) );
}


_WMRTLINK double _IF_dsqrt( double x )
/*************************/
{
    if( x < 0.0 ) {
//      x = _matherr( DOMAIN, "sqrt", &x, &x, 0.0 );
        x = __math1err( FUNC_SQRT | M_DOMAIN | V_ZERO, &x );
#if defined(_M_IX86)
    } else if( _RWD_real87 ) {
        x = _sqrt87( x );
#endif
    } else if( x != 0.0 ) {
        #if defined(_M_IX86)
            x = __sqrtd( x );
        #else
            register int        i;
            auto int    exp;
            auto double e;

            x = frexp( x, &exp );
            if( exp & 1 ) {     /* if odd */
                ++exp;
                x = x / 2.0;
                e = x * 0.828427314758301 + 0.297334909439087;
            } else {        /* even */
                e = x * 0.58578634262085 + 0.42049503326416;
            }
            i = 4;
            do {
                e = (x / e + e) / 2.0;
            } while( --i != 0 );
            x = ldexp( e, exp >> 1 );
        #endif
    }
    return( x );
}
