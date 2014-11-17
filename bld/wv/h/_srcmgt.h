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
* Description:  Source file management interface.
*
****************************************************************************/


#include <stdlib.h>
#include "dbgdefn.h"
#include "diptypes.h"
#include "dbgio.h"
#include "dbgmem.h"

#define SMTabIntervalGet()              TabIntervalGet()

#define _SMAlloc( pointer, size )       _Alloc( pointer, size )
#define _SMFree( pointer )              _Free( pointer )

#define sm_file_handle                  handle
#define sm_mod_handle                   mod_handle
#define sm_cue_fileid                   cue_fileid

#define SM_NO_MOD                       NO_MOD
#define SM_BUF_SIZE                     512

#define SMSeekStart( fp )               SeekStream( fp, 0L, DIO_SEEK_CUR )
#define SMSeekOrg( fp, offset )         SeekStream( fp, offset, DIO_SEEK_ORG )
#define SMSeekEnd( fp )                 SeekStream( fp, 0L, DIO_SEEK_END );

#define SMOpenRead( name )              FileOpen( name, OP_READ )
#define SMNilHandle( fp)                ( fp == NIL_HANDLE )
#define SMClose( fp )                   FileClose( fp )

#define SMReadStream( fp, buff, len )   ReadStream( fp, buff, len )

#define SMFileRemote( fp )              ( (FileHandleInfo( hndl->file_ptr ) & OP_REMOTE) != 0 )

extern int TabIntervalGet( void );
