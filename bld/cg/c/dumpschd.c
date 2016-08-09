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
* Description:  Dump instruction scheduling information.
*
****************************************************************************/



#include "cgstd.h"
#include "coderep.h"
#include "cgdefs.h"
#include "indvars.h"
#include "model.h"
#include "memout.h"
#include "procdef.h"
#include "freelist.h"
#include "inssched.h"
#include "dumpio.h"
#include "dmpinc.h"
#include "blktrim.h"

extern  void            DumpIns(instruction *);

extern data_dag         *DataDag;


static void DumpADataDag( data_dag *dag )
/********************************/
{
    int             i;
    dep_list_entry  *dep;

    DumpLiteral( "DAG: " );
    DumpPtr( dag );
    DumpLiteral( " ins=" );
    DumpPtr( dag->ins );
    DumpLiteral( " height=" );
    DumpInt( dag->height );
    DumpLiteral( " anc=" );
    DumpInt( dag->anc_count );
    DumpLiteral( " stallable=" );
    DumpInt( dag->stallable );
    DumpLiteral( " vis=" );
    DumpInt( dag->visited );
    DumpLiteral( " sched=" );
    DumpInt( dag->scheduled );
    DumpNL();
    DumpIns( dag->ins );
    DumpLiteral( "Deps:" );
    for( dep = dag->deps, i = 0; dep != NULL; dep = dep->next, ++i ) {
        if( i >= 8 ) {
            DumpNL();
            DumpLiteral( "     " );
            i = 0;
        }
        DumpChar( ' ' );
        DumpPtr( dep->dep );
    }
    DumpNL();
    DumpNL();
}

static void ReverseList( void )
/*****************************/
{

    data_dag    *head;
    data_dag    *dag;
    data_dag    *prev;

    head = NULL;
    for( dag = DataDag; dag != NULL; dag = prev ) {
        prev = dag->prev;
        dag->prev = head;
        head = dag;
    }
    DataDag = head;
}

void DumpDataDag( void )
/**********************/
{
    data_dag    *dag;

    ReverseList();
    for( dag = DataDag; dag != NULL; dag = dag->prev ) {
        DumpADataDag( dag );
    }
    ReverseList();
}
