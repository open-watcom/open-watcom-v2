/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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


#include <stddef.h>
#include "asmglob.h"
#include "asmalloc.h"
#include "omfcarve.h"
#include "myassert.h"

typedef struct blk blk_t;
struct blk {
    blk_t   *next;
    char    data[1];
};

typedef struct {
    size_t      elm_size;
    size_t      blk_size;
    size_t      blk_top;
    char        *top_elm;
    blk_t       *blk_list;
    void        **free_list;
} cv_t;

static void newBlk( cv_t *cv )
{
    blk_t   *newblk;

    newblk = AsmAlloc( sizeof( blk_t ) - 1 + cv->blk_top );
    newblk->next = cv->blk_list;
    cv->blk_list = newblk;
    cv->top_elm = newblk->data + cv->blk_top;
}

carve_t CarveCreate( size_t elm_size, size_t blk_size )
/*****************************************************/
{
    cv_t    *cv;

    cv = AsmAlloc( sizeof( *cv ) );
    cv->elm_size = elm_size;
    cv->blk_size = blk_size;
    cv->blk_top = blk_size * elm_size;
    cv->blk_list = NULL;
    cv->free_list = NULL;
    newBlk( cv );
    return( cv );
}

void CarveDestroy( carve_t caller_cv )
/************************************/
{
    cv_t    *cv;
    blk_t   *cur;
    blk_t   *next;

/**/myassert( caller_cv != NULL );
    cv = (cv_t *)caller_cv;
    for( cur = cv->blk_list; cur != NULL; cur = next ) {
        next = cur->next;
        AsmFree( cur );
    }
    AsmFree( cv );
}

void *CarveAlloc( carve_t caller_cv )
/***********************************/
{
    cv_t    *cv;
    void    **new;

/**/myassert( caller_cv != NULL );
    cv = (cv_t *)caller_cv;
    if( cv->free_list != NULL ) {
        new = cv->free_list;
        cv->free_list = *new;
        return( new );
    }
/**/myassert( cv->top_elm != NULL );
    if( cv->top_elm == cv->blk_list->data ) {
        newBlk( cv );
    }
    cv->top_elm -= cv->elm_size;
    return( cv->top_elm );
}

void CarveFree( carve_t caller_cv, void *elm )
/********************************************/
{
    cv_t    *cv;

/**/myassert( caller_cv != NULL );
    cv = (cv_t *)caller_cv;
    *(void**)elm = cv->free_list;
    cv->free_list = (void **)elm;
}
