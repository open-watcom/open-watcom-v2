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
* Description:  Return the underlying file handle from a stream buffer 
*               block. Usually implemented as a macro in stdio.h unless the
*               stream internals are obscured. 
*               NOTE: We have to define special versions for Netware as the 
*               thin libraries obtain their stream support from the O/S.
*
****************************************************************************/

#include "variety.h"
#include <stdio.h>
#include "fileacc.h"

#if defined (__NETWARE__) && defined (_THIN_LIB)

#undef  _fileno
#undef  fileno
/*
 *  Thin libraries can't call _ValidFile as this is not supported by the Novell CLIB/LIBC
 */
_WCRTLINK int _fileno(FILE * pf)
{
    return(fileno(pf));
}

#else

_WCRTLINK int (fileno)( FILE *fp )
{
    _ValidFile( fp, -1 );
    return( fp->_handle );
}

#endif
