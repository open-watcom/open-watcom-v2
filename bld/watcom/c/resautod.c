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


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#include "pcobj.h"
#include "objautod.h"
#include "autodep.h"    //for res files

walk_status WalkRESAutoDep( char *file_name, rtn_status (*rtn)( time_t, char *, void * ), void *data )
/****************************************************************************************************/
{
    DepInfo* depends;
    DepInfo* p;
    walk_status wstatus;
    rtn_status rstatus;

    wstatus = ADW_NOT_AN_OBJ;
    depends = WResGetAutoDep( file_name );
    if( depends ) {
        wstatus = ADW_OK;
        for( p=depends; p->len != 0; ) {
            rstatus = (*rtn)( p->time, p->name, data );
            if( rstatus == ADR_STOP ) {
                wstatus = ADW_RTN_STOPPED;
                break;
            }
            p = (DepInfo *) ( (char *)p + sizeof( DepInfo ) + p->len - 1 );
        }
        free( depends );
    }
    return wstatus;
}

#ifdef TEST
rtn_status dumpDeps( time_t t, char *name, void *data )
{
    data = data;
    printf( "%s %s\n", name, ctime( &t ), name );
    return( ADR_CONTINUE );
}

rtn_status dump2Deps( time_t t, char *name, void *data )
{
    unsigned *pc = data;

    --*pc;
    if( *pc ) {
        printf( "%s %s\n", name, ctime( &t ), name );
        return( ADR_CONTINUE );
    }
    return( ADR_STOP );
}

int main()
{
    unsigned count;

    WalkRESAutoDep( "objautod.obj", NULL, NULL );
    WalkRESAutoDep( "objautod.obj", dumpDeps, NULL );
    count = 2;
    WalkRESAutoDep( "objautod.obj", dump2Deps, &count );
    return( 0 );
}
#endif
