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


typedef struct {
    addr_off        map_offset;
    addr_seg        map_seg;
    dr_handle       sym;  /* dwarf handle */
}addrsym_info;

typedef bool (*WLKADDRSYM)( void *d, addrsym_info *info );

extern void AddAddrSym( seg_list *list, addrsym_info *new );
extern int  FindAddrSym( seg_list *addr_map, addr_ptr *mach, addrsym_info *ret );
extern void SortAddrSym( seg_list *ctl );
extern void InitAddrSym( seg_list *list );
extern bool WlkAddrSyms( seg_list *ctl, WLKADDRSYM fn, void *d );
extern void FiniAddrSym( seg_list *ctl );
