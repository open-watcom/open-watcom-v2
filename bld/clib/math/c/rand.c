/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2017 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Implementation of rand() and srand().
*
****************************************************************************/


#include "variety.h"
#include <stdlib.h>
#if defined( __NT__ )
    #include <windows.h>
#elif defined( __OS2__ )
    #include <wos2.h>
#elif defined( __RDOS__ )
    #include <rdos.h>
#endif
#include "rtdata.h"
#include "randnext.h"
#include "thread.h"

#ifndef __RDOS__

static unsigned long *initrandnext( void )
{
    _INITRANDNEXT( RETURN_ARG( unsigned long *, NULL ) );
    return( (unsigned long *)&_RWD_randnext );
}

#endif

_WCRTLINK int rand( void )
/************************/
{
#if defined( __RDOS__ )
    return( (int)( RdosGetLongRandom() & 0x7FFF ) );
#else
    unsigned long   *randptr;

    randptr = initrandnext();
    if( randptr == NULL ) {
        return( 0 );
    }
    *randptr = *randptr * 1103515245 + 12345;
    return( (int)( (*randptr >> 16) & 0x7FFF ) );
#endif
}


_WCRTLINK void srand( unsigned int seed )
/***************************************/
{
#ifndef __RDOS__
    unsigned long   *randptr;

    randptr = initrandnext();
    if( randptr != NULL ) {
        *randptr = seed;
    }
#else

    /* unused parameters */ (void)seed;

#endif
}
