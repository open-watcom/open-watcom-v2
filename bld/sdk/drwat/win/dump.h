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


#ifndef __DUMP_INCLUDED__
#define __DUMP_INCLUDED__
#include "pmdbg.h"

#define MAGIC_COOKIE    0xdeadbeefL

typedef struct {
unsigned long   cookie;
long            exe_time;
char            exe_name[144];
registers       regs;
int             num_dir_ents;
int             num_mod_ents;
long            dir_offset;
long            mod_offset;
unsigned long   winflags;
unsigned short  pid;
} dump_header;

typedef struct {
int             seg_num;
int             module_index;
int             sel;
long            byte_count;
long            offset;
descriptor      desc;
} dir_ent;

#define MAX_EXPATH      255
typedef struct {
HANDLE          module_handle;
char            exepath[MAX_EXPATH + 1];
} mod_ent;
#endif
