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
* Description:  _matherr() implementation.
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include "clibsupp.h"
#include "mathlib.h"
#include "mathcode.h"
#include "_matherr.h"

static const char * const Msgs[] = {
    0,
    "Domain error",
    "Argument singularity",
    "Overflow range error",
    "Underflow range error",
    "Total loss of significance",
    "Partial loss of significance"
};

static char *MathFuncNames[] = {
    #define pick(enum,name) name,
    #include "_mathfun.h"
    #undef pick
};

#if defined(_M_IX86)
int (*__matherr_handler)( struct _exception * ) = __matherr;
#else
int (*__matherr_handler)( struct _exception * ) = matherr;
#endif

_WMRTLINK void _set_matherr( int (*rtn)( struct _exception * ) )
{
    _RWD_matherr = rtn;
}

void __rterrmsg( int errcode, const char *funcname )
{
    FILE    *fp;

    fp = __get_std_stream( STDERR_FILENO );
    fputs( Msgs[errcode], fp );
    fputs( " in ", fp );
    fputs( funcname, fp );
    fputc( '\n', fp );
}

char *__rtmathfuncname( int funccode )
{
    return( MathFuncNames[funccode - 1] );
}

_WMRTLINK double _matherr( struct _exception *excp )
/**************************************************/
{
    if( (*_RWD_matherr)( excp ) == 0 ) {
        __rterrmsg( excp->type, excp->name );
        excp->type == DOMAIN ? __set_EDOM() : __set_ERANGE();
    }
    return( excp->retval );
}
