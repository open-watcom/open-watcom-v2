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


/* data types */

/* the client should not use any of the fields of dr_context_stack or
 * dr_search_context
 */

typedef struct {
    int         size;   /* available room */
    int         free;   /* next free entry */
    uint_32 *   stack;  /* values */
} dr_context_stack;

typedef struct {
    dr_cu_handle        compunit;       /* current compile unit */
    dr_handle           start;          /* die to start from */
    dr_handle           end;            /* end of compile unit */
    dr_handle           functionhdl;    /* containing function */
    dr_handle           classhdl;       /* containing class */
    dr_context_stack    stack;          /* stack of classes / functions */
} dr_search_context;

typedef struct {
    dr_handle           handle;         /* symbol's handle */
    dr_sym_type         type;           /* symbol's type */
    char *              name;           /* symbol's name */
    dr_search_context * context;        /* context to resume search */
} dr_sym_context;


/* function prototypes */

extern bool DRSymSearch( dr_search, dr_depth, void *, void *,
                         int (*)(dr_sym_context *,void*));
extern bool DRResumeSymSearch( dr_search_context *, dr_search,
                               dr_depth, void *, void *,
                               int (*)(dr_sym_context *,void*));

extern dr_search_context * DRDuplicateSearchContext( dr_search_context * );
extern void                DRFreeSearchContext( dr_search_context * );
