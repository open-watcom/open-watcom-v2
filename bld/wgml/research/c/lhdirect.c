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
* Description:  Implements the functions declared in dowlinux.h:
*                   get_file_size()
*
****************************************************************************/

#include "lhdirect.h"

#ifdef __LINUX__

/*  Function get_file_size().
 *  This should return the size of the file; however, someone who knows Linux
 *  will have to implement it.
 *
 *  Note: both the return value and the parameter type are negotiable, so long
 *  as the "other" version uses the same types so that all of the differences
 *  are handled here.
 */

long get_file_size( struct dirent * in_file )
{
    /* So the compiler sees the parameter used. */
    
    in_file = in_file; 

    return( 0 );
}

#else /* DOS, OS/2, Windows. */

/*  Function get_file_size().
 *  Returns in_file->d_size.
 */
 
long get_file_size( struct dirent * in_file )
{
    return( in_file->d_size );
}

#endif /* __LINUX__ */

