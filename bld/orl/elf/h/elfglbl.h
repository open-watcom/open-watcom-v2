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

#include "orlintnl.h"
#include "elftype.h"

#define _ClientRead( a, b )             ((a)->elf_hnd->funcs->read( a->file, b ))
#define _ClientSeek( a, b, c )          ((a)->elf_hnd->funcs->seek( a->file, b, c ))
#define _ClientAlloc( a, b )            ((a)->elf_hnd->funcs->alloc( b ))
#define _ClientFree( a, b )             ((a)->elf_hnd->funcs->free( b ))
#define _ClientReAlloc( a, b, c )       ((a)->elf_hnd->funcs->realloc( b, c ))

#define _ClientSecRead( a, b, c )       ((a)->elf_file_hnd->elf_hnd->funcs->read( a->elf_file_hnd->file, b, c ))
#define _ClientSecSeek( a, b, c )       ((a)->elf_file_hnd->elf_hnd->funcs->seek( a->elf_file_hnd->file, b, c ))
#define _ClientSecAlloc( a, b )         ((a)->elf_file_hnd->elf_hnd->funcs->alloc( b ))
#define _ClientSecFree( a, b )          ((a)->elf_file_hnd->elf_hnd->funcs->free( b ))

#endif
