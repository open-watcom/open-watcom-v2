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
#include <dos.h>
#include <errno.h>

extern unsigned pascal _clib_dos_findfirst(const char *__path,unsigned __attr,struct find_t *__buf);
extern unsigned pascal _clib_dos_findnext( struct find_t *__buf);
extern int pascal _clib_errno( void );

_WCRTLINK unsigned _dos_findfirst(const char *__path,unsigned __attr,struct find_t *__buf)
{
    int rc;
    rc = _clib_dos_findfirst(__path,__attr,__buf);
    errno = _clib_errno();
    return( rc );
}

_WCRTLINK unsigned _dos_findnext( struct find_t *__buf)
{
    int rc;
    rc = _clib_dos_findnext( __buf);
    errno = _clib_errno();
    return( rc );
}

_WCRTLINK unsigned _dos_findclose( struct find_t *__buf)
{
    __buf = __buf;
    return( 0 );
}
