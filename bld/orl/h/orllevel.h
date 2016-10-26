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


#ifndef ORL_LEVEL_INCLUDED
#define ORL_LEVEL_INCLUDED

#include "orl.h"
#include "orlintnl.h"
#include "cofforl.h"
#include "elforl.h"
#include "omforl.h"

#define ORL_FILE_TYPE(x)    (((orli_file_handle)x)->type)
#define ORL_FILE_HND_ELF(x) (((orli_file_handle)x)->file_hnd.elf)
#define ORL_FILE_HND_OMF(x) (((orli_file_handle)x)->file_hnd.omf)
#define ORL_FILE_HND_COFF(x)(((orli_file_handle)x)->file_hnd.coff)

/* NB _handle = a type, _hnd = a variable */

typedef struct orli_handle_struct {
    orl_funcs *                         funcs;
    elf_handle                          elf_hnd;
    coff_handle                         coff_hnd;
    omf_handle                          omf_hnd;
    struct orli_file_handle_struct *    first_file_hnd;
    orl_return                          error;
} * orli_handle;

typedef struct orli_file_handle_struct {
    orli_handle                         orli_hnd;
    struct orli_file_handle_struct *    next;
    orl_file_format                     type;
    union {
        elf_file_handle                 elf;
        coff_file_handle                coff;
        omf_file_handle                 omf;
    } file_hnd;
} * orli_file_handle;

#endif
