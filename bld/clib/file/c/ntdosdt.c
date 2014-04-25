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
* Description:  Implementation of clib internal conversion functions for
*               DOS<->NT date format
*
****************************************************************************/


#include "variety.h"
#include <windows.h>
#include "ntex.h"

void __MakeDOSDT( FILETIME *NT_stamp, unsigned short *d, unsigned short *t )
{
    FILETIME local_ft;

    FileTimeToLocalFileTime( NT_stamp, &local_ft );
    FileTimeToDosDateTime( &local_ft, d, t );
}

void __FromDOSDT( unsigned short d, unsigned short t, FILETIME *NT_stamp )
{
    FILETIME local_ft;

    DosDateTimeToFileTime( d, t, &local_ft );
    LocalFileTimeToFileTime( &local_ft, NT_stamp );
}
