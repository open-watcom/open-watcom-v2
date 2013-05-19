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
* Description:  Executable dumper globals.
*
****************************************************************************/


#include <setjmp.h>
#include <stdio.h>
#include "wdglb.h"


int                     Handle;
int                     Lhandle;
unsigned_16             Sizeleft;
unsigned_16             WSize;
unsigned_16             Num_buff;
char                    Read_buff[BSIZE];
char                    Write_buff[BSIZE];
char                    *Wbuff;
char                    *Lang_lst;
int                     Num_read;
unsigned_32             Data_count;
unsigned_32             Curr_sectoff;
unsigned_32             Coff_off;
unsigned_32             New_exe_off;
unsigned_32             Exp_off;
unsigned_32             Imp_off;
unsigned_32             Fix_off;
unsigned_32             Res_off;
unsigned_32             Data_off;
struct segment_record   *Int_seg_tab;
char                    **Int_mod_ref_tab;
unsigned_16             Segspec;
unsigned long           Hexoff;
Elf32_Ehdr              Elf_head;
lmf_header              Qnx_head;
extended_header         Phar_ext_head;
simple_header           Phar_head;
extended_nlm_header     Nlm_ext_head;
nlm_header              Nlm_head;
pe_header               Pe_head;
struct os2_flat_header  Os2_386_head;
struct os2_exe_header   Os2_head;
struct dos_exe_header   Dos_head;
unsigned_16             Resrc_shift_cnt;
unsigned_16             Options_dmp;
unsigned_8              Debug_options;
char                    *Name;
unsigned_32             Load_len;
unsigned_32             Resrc_end;
jmp_buf                 Se_env;
int                     Form;
bool                    Byte_swap;
struct                  section_data Sections[DR_DEBUG_NUM_SECTS];
