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


/*stuff for makeing a list of conatainging tags*/
enum scope_list{
    SCOPE_BLOCK_SIZE = 10
};

typedef struct scope_entry  scope_entry;
struct scope_entry {
    scope_entry *next;
    dr_handle      handle;
};
typedef struct scope_block  scope_block;
struct scope_block{
    scope_block *next;
    scope_entry  entries[SCOPE_BLOCK_SIZE];
};

typedef struct{
    scope_block *next;
    scope_entry *free;
    scope_block  first[1];
}scope_ctl;

typedef struct scope_trail  scope_trail;
struct scope_trail {
    dr_handle      target;
    scope_entry *head;  //don't return this list
    scope_ctl    ctl;
};


extern void DRGetScopeList( scope_trail *container, dr_handle  of );
extern void DREndScopeList( scope_trail *container );
