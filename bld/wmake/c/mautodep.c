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


#include "make.h"
#include "mcache.h"
#include "memory.h"
#include "mrcmsg.h"
#include "msg.h"
#include "mupdate.h"
#include "mautodep.h"

#include <malloc.h>
#include <assert.h>

extern auto_dep_info OMFAutoDepInfo;
extern auto_dep_info ORLAutoDepInfo;
extern auto_dep_info RESAutoDepInfo;

static const auto_dep_info *AutoDepTypes[] = {
    &OMFAutoDepInfo,
#if !defined( M_I86 )
    &ORLAutoDepInfo,
#endif
    &RESAutoDepInfo,
    NULL
};

void AutoDepInit( void )
/**********************/
{
    const auto_dep_info * const *pcurr;

    for( pcurr = &AutoDepTypes[ 0 ]; *pcurr != NULL; pcurr++ ) {
        if( (*pcurr)->init != NULL ) {
            (*pcurr)->init();
        }
    }
}

BOOLEAN AutoDepCheck( char *name, time_t stamp, BOOLEAN (*chk)(time_t,time_t), time_t *pmax_time )
/************************************************************************************************/
{
    const auto_dep_info *curr;
    const auto_dep_info * const *pcurr;
    char                *dep_name;
    time_t              dep_time;       /* time stamp for dependant file buried in auto-depends info */
    time_t              curr_dep_time;  /* current date on dependant file (if it exists) */
    time_t              max_time;
    void                *hdl;
    void                *dep_hdl;
    struct {
        unsigned out_of_date    : 1;
        unsigned exists         : 1;
        unsigned this_caused_it : 1;
    } flag;

    flag.out_of_date = FALSE;
    max_time = *pmax_time;
    for( pcurr = &AutoDepTypes[ 0 ]; *pcurr != NULL; pcurr++ ) {
        curr = *pcurr;
        hdl = curr->init_file( name );
        if( hdl != NULL ) {
            dep_hdl = curr->first_dep( hdl );
            while( dep_hdl != NULL ) {
                curr->trans_dep( dep_hdl, &dep_name, &dep_time );
                /* logic having to do with dep_name and dep_time */
                flag.exists = FALSE;
                flag.this_caused_it = FALSE;
                if( CacheTime( dep_name, &curr_dep_time ) != RET_SUCCESS ) {
                    /* doesn't exist anymore so rebuild file */
                    flag.out_of_date = TRUE;
                    flag.this_caused_it = TRUE;
                } else {
                    flag.exists = TRUE;
                    if( !IdenticalAutoDepTimes( curr_dep_time, dep_time ) ) {
                        flag.out_of_date = TRUE;
                        flag.this_caused_it = TRUE;
                    } else if( (*chk)( stamp, curr_dep_time ) ) {
                        flag.out_of_date = TRUE;
                        flag.this_caused_it = TRUE;
                    }
                    /* don't want Glob.all affecting the comparison */
                    if( curr_dep_time > max_time ) {
                        max_time = curr_dep_time;
                    }
                }
                if( Glob.debug ) {
                    char        time_buff[ 32 ];    /* large enough for date + flag */
                    struct tm   *tm;

                    if( !flag.exists ) {
                        time_buff[0] = '?';
                        time_buff[1] = ' ';
                        time_buff[2] = '\0';
                    } else {
                        tm = localtime( &curr_dep_time );
                        FmtStr( time_buff, "%D-%s-%D  %D:%D:%D ",
                                tm->tm_mday, MonthNames[ tm->tm_mon ], tm->tm_year,
                                tm->tm_hour, tm->tm_min, tm->tm_sec
                            );
                    }
                    if( flag.this_caused_it ) {
                        unsigned len = strlen( time_buff );
                        time_buff[len-1] = '*';
                    }
                    PrtMsg( DBG|INF| GETDATE_MSG, time_buff, dep_name );
                }
                /* may not need to calculate real max time */
                if( flag.out_of_date && !Glob.rcs_make ) break;
                dep_hdl = curr->next_dep( dep_hdl );
            }
            curr->fini_file( hdl );
            *pmax_time = max_time;
            assert( _heapchk() == _HEAPOK );
            return( flag.out_of_date );
        }
    }
    return( flag.out_of_date );
}

void AutoDepFini( void )
/**********************/
{
    const auto_dep_info * const *pcurr;

    for( pcurr = &AutoDepTypes[ 0 ]; *pcurr != NULL; pcurr++ ) {
        if( (*pcurr)->fini != NULL ) {
            (*pcurr)->fini();
        }
    }
}

#if 0
void heap_dump()
  {
    struct _heapinfo h_info;
    int heap_status;

    h_info._pentry = NULL;
    for(;;) {
      heap_status = _heapwalk( &h_info );
      if( heap_status != _HEAPOK ) break;
      if( h_info._useflag == _USEDENTRY ) continue;
      printf( "  %s block at %Fp of size %4.4X\n",
        (h_info._useflag == _USEDENTRY ? "USED" : "FREE"),
        h_info._pentry, h_info._size );
    }

    switch( heap_status ) {
    case _HEAPEND:
      printf( "OK - end of heap\n" );
      break;
    case _HEAPEMPTY:
      printf( "OK - heap is empty\n" );
      break;
    case _HEAPBADBEGIN:
      printf( "ERROR - heap is damaged\n" );
      break;
    case _HEAPBADPTR:
      printf( "ERROR - bad pointer to heap\n" );
      break;
    case _HEAPBADNODE:
      printf( "ERROR - bad node in heap\n" );
    }
  }
#endif
