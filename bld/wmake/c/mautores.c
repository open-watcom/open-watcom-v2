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
* Description:  Resource file autodependency information processing.
*
****************************************************************************/


#include "watcom.h"
#include "mtypes.h"
#include "mcache.h"
#include "msysdep.h"
#include "mautodep.h"

#ifndef BOOTSTRAP
#include "autodep.h"

#include "wresset2.h"

typedef struct res_info {
    DepInfo *first;
    DepInfo *curr;
}           res_info;

static res_info ResInfo;

STATIC handle RESInitFile( const char *name )
/*******************************************/
{
    DepInfo     *depends;
    res_info    *ret_val;
    long        old_shift;

    ret_val = NULL;
    old_shift = FileShift;
    FileShift = 0;
    depends = WResGetAutoDep( (char *)name );
    FileShift = old_shift;
    if( depends != NULL ) {
        ResInfo.first = depends;
        ResInfo.curr = depends;
        ret_val = &ResInfo;
    }
    return( ret_val );
}


STATIC dep_handle RESFirstDep( dep_handle file )
/**********************************************/
{
    return( file );
}


STATIC void RESTransDep( dep_handle f, char **name, time_t *stamp )
/*****************************************************************/
{
    DepInfo *curr = ((res_info *)f)->curr;

    *name = curr->name;
    *stamp = curr->time;
}


STATIC handle RESNextDep( dep_handle f )
/**************************************/
{
    DepInfo     *p;
    res_info    *file = f;

    p = (void *)file->curr;
    p = (void *)( (char *)p + sizeof( *p ) + p->len - 1 );
    if( p->len == 0 ) {
        file->curr = NULL;
        return( NULL );
    }
    file->curr = p;
    return( file );
}


STATIC void RESFiniFile( handle file )
/************************************/
{
    WResFreeAutoDep( ((res_info *)file)->first );
}


const auto_dep_info RESAutoDepInfo = {
    NULL,
    RESInitFile,
    RESFirstDep,
    RESTransDep,
    RESNextDep,
    RESFiniFile,
    NULL
};

#endif
