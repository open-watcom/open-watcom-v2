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
* Description:  Define FORTRAN 77 name list.
*
****************************************************************************/


typedef struct name_list {
    sym_id              link;                   // next name list definition
    struct grp_entry    *group_list;            // entries in namelist
    obj_addr            address;                // address of namelist data
    db_handle           dbh;                    // dwarf handle
    byte                name_len;               // length of namelist name
    char                name[STD_SYMLEN];       // namelist name
} name_list;

typedef struct grp_entry {
    struct grp_entry    *link;                  // next name list entry
//  struct name_list    *name_list;             // name list entry belongs to
    sym_id              sym;                    // sym_id of name list entry
} grp_entry;
