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
* Description:  Interface to low level ELF processing routines.
*
****************************************************************************/


#ifndef ELF_LWLV_INCLUDED
#define ELF_LWLV_INCLUDED

#include "elfglbl.h"

extern orl_return       ElfCreateRelocs( elf_sec_handle, elf_sec_handle );
extern orl_return       ElfCreateSymbolHandles( elf_sec_handle );
extern orl_return       ElfBuildSecNameHashTable( elf_file_handle );
extern orl_reloc_type   ElfConvertRelocType( elf_file_handle, elf_reloc_type );
extern orl_return       ElfParseDrectve( const char *, size_t len, orl_note_callbacks *, void * );

#endif
