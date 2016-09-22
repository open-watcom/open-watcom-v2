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


#ifndef ELF_GLOBAL_INCLUDED
#define ELF_GLOBAL_INCLUDED

#include "orl.h"
#include "orlintnl.h"
#include "elftype.h"

#define _ClientRead( a, b )             ORL_CLI_READ((a)->elf_hnd->funcs, (a)->file, b )
#define _ClientSeek( a, b, c )          ORL_CLI_SEEK((a)->elf_hnd->funcs, (a)->file, b, c )
#define _ClientAlloc( a, b )            ORL_CLI_ALLOC((a)->elf_hnd->funcs, b )
#define _ClientFree( a, b )             ORL_CLI_FREE((a)->elf_hnd->funcs, b )

#define _ClientSecRead( a, b, c )       ORL_CLI_READ((a)->elf_file_hnd->elf_hnd->funcs, (a)->elf_file_hnd->file, b, c )
#define _ClientSecSeek( a, b, c )       ORL_CLI_SEEK((a)->elf_file_hnd->elf_hnd->funcs, (a)->elf_file_hnd->file, b, c )
#define _ClientSecAlloc( a, b )         ORL_CLI_ALLOC((a)->elf_file_hnd->elf_hnd->funcs, b )
#define _ClientSecFree( a, b )          ORL_CLI_FREE((a)->elf_file_hnd->elf_hnd->funcs, b )

#endif
