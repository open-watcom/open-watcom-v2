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


#include "malloc.h"
#include "unistd.h"
#include "fcntl.h"

extern void *ProfAlloc(size_t size);
extern void ProfFree(void *ptr);

#define SMTabIntervalGet()              8

#define _SMAlloc( pointer, size )       (pointer) = ProfAlloc( size )
#define _SMFree( pointer )              ProfFree( pointer )

#define sm_file_handle                  int
#define sm_mod_handle                   int
#define sm_cue_file_id                  int

#define SM_NO_MOD                       ((sm_mod_handle)-1)
#define SM_BUF_SIZE                     512

#define SMSeekStart( fp )               lseek( fp, 0L, SEEK_CUR )
#define SMSeekOrg( fp, offset )         lseek( fp, offset, SEEK_SET )
#define SMSeekEnd( fp )                 lseek( fp, 0L, SEEK_END );

#define SMOpenRead( name )              open( name, O_RDONLY|O_BINARY )
#define SMNilHandle( fp)                ( fp == -1 )
#define SMClose( fp )                   close( fp )

#define SMReadStream( fp, buff, len )   read( fp, buff, len )

#define SMFileRemote( fp )              0
