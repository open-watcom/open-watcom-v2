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
* Description:  Generic autodependency information processing.
*
****************************************************************************/


#include "make.h"
#include "mcache.h"
#include "mmemory.h"
#include "mrcmsg.h"
#include "msg.h"
#include "mupdate.h"
#include "mautodep.h"

#include "mautoomf.h"
#include "mautoorl.h"
#include "mautores.h"

static auto_dep_info const *AutoDepTypes[] = {
    &OMFAutoDepInfo,
#if !defined( BOOTSTRAP )
  #if !defined( _M_I86 )
    &ORLAutoDepInfo,
  #endif
    &RESAutoDepInfo,
#endif
    NULL
};


void AutoDepInit( void )
/**********************/
{
    const auto_dep_info * const *pcurr;

    for( pcurr = &AutoDepTypes[0]; *pcurr != NULL; pcurr++ ) {
        if( (*pcurr)->init != NULL ) {
            (*pcurr)->init();
        }
    }
}


static bool isTargObsolete( char const *name, time_t stamp,
    bool (*chk)(time_t,time_t), time_t *pmax_time,
    auto_dep_info const *curr, void *dep )
/************************************************************/
{
    char        *dep_name;
    time_t      auto_dep_time;  // time stamp in auto-depend info
    time_t      curr_dep_time;  // time stamp in dependent file (if it exists)
    bool        exists;
    bool        obsolete;

    exists = true, obsolete = false;
    curr->trans_dep( dep, &dep_name, &auto_dep_time );
    if( CacheTime( dep_name, &curr_dep_time ) != RET_SUCCESS ) {
        exists = false, obsolete = true;
    } else {
        if( !IdenticalAutoDepTimes( auto_dep_time, curr_dep_time ) ||
                (*chk)( stamp, curr_dep_time ) ) {
            obsolete = true;
        }
        if( curr_dep_time > *pmax_time ) {
            *pmax_time = curr_dep_time; // Glob.all should not affect comparison
        }
    }
    if( Glob.debug ) {
        char        time_buff[32] = "?";// for date + flag
        struct tm   *tm;

        if( exists ) {
            tm = localtime( &curr_dep_time );
            FmtStr( time_buff, "%D-%s-%D  %D:%D:%D",
                    tm->tm_mday, MonthNames[tm->tm_mon], tm->tm_year,
                    tm->tm_hour, tm->tm_min, tm->tm_sec );
        }
        strcat( time_buff, ( obsolete ) ? "*" : " " );
        PrtMsg( DBG | INF | GETDATE_MSG, time_buff, dep_name );
    } else if( obsolete && Glob.show_offenders ) {
        PrtMsg( INF | WILL_BE_BUILT_BECAUSE_OF, name, dep_name );
    }
    return( obsolete );
}


bool AutoDepCheck( char *name, time_t stamp,
    bool (*chk)(time_t,time_t), time_t *pmax_time )
/*******************************************************/
{
    bool                        quick_logic;
    auto_dep_info const * const *pcurr;
    auto_dep_info const         *curr;
    void                        *hdl;
    void                        *dep;
    bool                        obs;

    quick_logic = !( Glob.rcs_make || Glob.debug || Glob.show_offenders );
    obs = false;

    for( pcurr = &AutoDepTypes[0]; (curr = *pcurr) != NULL; pcurr++ ) {
        if( (hdl = curr->init_file( name )) != NULL ) {
            dep_handle (* const first_dep)( handle )    = curr->first_dep;
            dep_handle (* const next_dep)( dep_handle ) = curr->next_dep;

            for( dep = first_dep( hdl ); dep != NULL; dep = next_dep( hdl ) ) {
                obs |= isTargObsolete(name, stamp, chk, pmax_time, curr, dep );
                if( obs && quick_logic ) {
                    break; // No need to calculate real max time
                }
            }
            curr->fini_file( hdl );
            return( obs );
        }
    }
    return( false );
}


void AutoDepFini( void )
/**********************/
{
    const auto_dep_info * const *pcurr;

    for( pcurr = &AutoDepTypes[0]; *pcurr != NULL; pcurr++ ) {
        if( (*pcurr)->fini != NULL ) {
            (*pcurr)->fini();
        }
    }
}
