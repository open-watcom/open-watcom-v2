/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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
#include "widechar.h"
#include <string.h>
#include <time.h>
#include <dos.h>
#include <windows.h>
#include "ntext.h"
#include "ntinfo.h"
#include "_dtaxxx.h"


void __GetNTFindInfo( FINDT_TYPE *findt, LPWIN32_FIND_DATA ffd )
{
    DTAXXX_TSTAMP_OF( findt->reserved ) = __NT_filetime_to_timet( &ffd->ftLastWriteTime );
    __MakeDOSDT( &ffd->ftLastWriteTime, &findt->wr_date, &findt->wr_time );
    findt->attrib = NT2DOSATTR( ffd->dwFileAttributes );
    findt->size = ffd->nFileSizeLow;
    __F_NAME(strncpy,wcsncpy)( findt->name, ffd->cFileName, NAME_MAX );
    findt->name[NAME_MAX] = 0;
}
