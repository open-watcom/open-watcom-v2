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


#include <string.h>
#include "pgvars.h"


static unsigned short   _DefaultStyle[ 16 ] = {
                            0x0000, 0xffff, 0xf0f0, 0xf060,
                            0xcccc, 0xc8c8, 0xeeee, 0xeaea,
                            0xf6de, 0xf6f6, 0xf56a, 0xcece,
                            0xa8a8, 0xaaaa, 0xe4e4, 0xc88c
};


_WCRTLINK void _WCI86FAR _CGRAPH _pg_resetstyleset( void )
//=========================================

/*  Initializes the global styleset variable.   */

{
    memcpy( _Style, _DefaultStyle, sizeof( styleset ) );
}

Entry( _PG_RESETSTYLESET, _pg_resetstyleset ) // alternate entry-point


_WCRTLINK void _WCI86FAR _CGRAPH _pg_getstyleset( unsigned short _WCI86FAR *style )
//=============================================================

/*  Gets the contents of the global styleset variable. */

{
    MemoryCopy( style, _Style, sizeof( styleset ) );
}

Entry( _PG_GETSTYLESET, _pg_getstyleset ) // alternate entry-point


_WCRTLINK void _WCI86FAR _CGRAPH _pg_setstyleset( unsigned short _WCI86FAR *style )
//=============================================================

/*  Set the global styleset to that passed as a parameter. */

{
    MemoryCopy( _Style, style, sizeof( styleset ) );
}

Entry( _PG_SETSTYLESET, _pg_setstyleset ) // alternate entry-point
