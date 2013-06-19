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
* Description:  Target update processing.
*
****************************************************************************/


#include <string.h>
#include <time.h>
#if defined( __WATCOMC__ ) || defined( __UNIX__ )
#include <utime.h>
#else
#include <sys/utime.h>
#endif

#include "make.h"
#include "macros.h"
#include "mcache.h"
#include "mmemory.h"
#include "mexec.h"
#include "mmisc.h"
#include "mlex.h"
#include "pathgrp.h"
#include "mpreproc.h"
#include "mrcmsg.h"
#include "msg.h"
#include "msuffix.h"
#include "mtarget.h"
#include "mupdate.h"
#include "mvecstr.h"
#include "mautodep.h"


STATIC BOOLEAN checkForAutoDeps( TARGET *targ, char *name, time_t *max_time );

/*
 *  exStack is used to stack the special macro pointers during ExecCList()
 *  calls.
 */
struct exStack {
    TARGET  *targ;
    DEPEND  *dep;
    DEPEND  *impDep;
};

#define MAX_EXSTACK 16
STATIC struct exStack   exStack[MAX_EXSTACK];
STATIC UINT8            exStackP;
STATIC BOOLEAN          doneBefore;     /* executed the .BEFORE commands? */
STATIC UINT32           cListCount;     /* number of CLISTs executed so far */

BOOLEAN DoingUpdate;

const char *MonthNames[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

#define USE_AUTO_DEP( x )   ((Glob.auto_depends != 0) || ((x)->attr.auto_dep != 0))

void exPush( TARGET *targ, DEPEND *dep, DEPEND *impDep )
/*************************************************************/
{
    if( exStackP == MAX_EXSTACK ) {
        PrtMsg( FTL| PERCENT_MAKE_DEPTH );
    }
    exStack[exStackP].targ   = targ;
    exStack[exStackP].dep    = dep;
    exStack[exStackP].impDep = impDep;
    ++exStackP;
}


void exPop( void )
/***********************/
{
    assert( exStackP > 0 );
    --exStackP;
}


STATIC signed int dateCmp( time_t targ, time_t dep )
/**************************************************/
{
    if( (targ < dep) || Glob.all ) {
        return( -1 );
    } else if( targ > dep ) {
        return( 1 );
    } else {
        return( 0 );
    }
}


STATIC void getStats( TARGET *targ )
/**********************************/
{
    if( targ->executed ) {
        targ->executed = FALSE;
        if( targ->touched ) {           /* used with symbolic, -t, -n, -q */
            targ->date = YOUNGEST_DATE;
            targ->existing = TRUE;
        } else if( targ->attr.symb ) {
            targ->existing = FALSE;
            targ->date = OLDEST_DATE;
        } else if( CacheTime( targ->node.name, &targ->date ) != RET_SUCCESS ) {
            /* if file doesn't exist make it old */
            targ->date = OLDEST_DATE;
            targ->existing = FALSE;
            /* useful in those rare cases when a file has been deleted */
        } else {
            targ->existing = TRUE;
        }
    } /* else: no changes in the status */
}


#ifdef __WATCOMC__
#pragma on (check_stack);
#endif
STATIC BOOLEAN targExists( TARGET *targ )
/***************************************/
{
    char    buffer[_MAX_PATH];

    getStats( targ );           /* get date stamp */
    if( targ->existing ) {
        return( TRUE );
    }
    if( targ->attr.symb ) {
        return( FALSE );
    }

    if( TrySufPath( buffer, targ->node.name, NULL, FALSE ) == RET_SUCCESS ) {
        RenameTarget( targ, buffer );
        targ->executed = TRUE;              /* force get date */
        getStats( targ );
        assert( targ->existing );
        return( TRUE );
    }
    return( FALSE );
}
#ifdef __WATCOMC__
#pragma off(check_stack);
#endif


STATIC void getDate( TARGET *targ )
/*********************************/
{
    getStats( targ );
    if( Glob.debug ) {
        char        buf[20];    /* large enough for date */
        struct tm   *tm;

        if( targ->date == YOUNGEST_DATE ) {
            FmtStr( buf, "%M", M_YOUNGEST_DATE );
        } else if( targ->date == OLDEST_DATE ) {
            FmtStr( buf, "%M", M_OLDEST_DATE );
        } else {
            tm = localtime( &targ->date );
            FmtStr( buf, "%D-%s-%D  %D:%D:%D",
                    tm->tm_mday, MonthNames[tm->tm_mon], tm->tm_year,
                    tm->tm_hour, tm->tm_min, tm->tm_sec
                );
        }
        PrtMsg( DBG | INF | GETDATE_MSG, buf, targ->node.name );
    }
}


#ifdef __WATCOMC__
#pragma on (check_stack);
#endif
STATIC RET_T carryOut( TARGET *targ, CLIST *clist, time_t max_time )
/******************************************************************/
{
    CLIST               *err;
    int                 i;
    struct utimbuf      times;
    char                msg[max( MAX_RESOURCE_SIZE, _MAX_PATH )];

    assert( targ != NULL && clist != NULL );

    ++cListCount;
    if( ExecCList( clist ) == RET_SUCCESS ) {
        if( Glob.rcs_make && !Glob.noexec && !Glob.touch ) {
            if( max_time != OLDEST_DATE ) {
                targ->date = max_time;
                targ->backdated = TRUE;
                if( TrySufPath( msg, targ->node.name, NULL, FALSE ) == RET_SUCCESS ) {
                    if( USE_AUTO_DEP( targ ) ) {
                        // target has auto dependency info
                        // result: max_time may be incorrect!
                        CacheRelease();
                        checkForAutoDeps( targ, msg, &max_time );
                    }
                    times.actime = max_time;
                    times.modtime = max_time;
                    utime( msg, &times );
                    CacheRelease();
                }
            }
        }
        targ->cmds_done = TRUE;
        return( RET_SUCCESS );
    }

    /*
     * ok - here is something I don't like.  In this portion of code here,
     * carryOut is ignoring the target passed to it, and digging the targets
     * out of the stack.  Be careful of changes.
     */
    PrtMsg( ERR | NEOL | LAST_CMD_MAKING_RET_BAD );
    for( i = exStackP - 1; i >= 0; --i ) {
        PrtMsg( ERR | NEOL | PRNTSTR, exStack[i].targ->node.name );
        if( i > 0 ) {
            PrtMsg( ERR | NEOL | PRNTSTR, ";" );
        }
    }
    MsgGetTail( LAST_CMD_MAKING_RET_BAD, msg );
    PrtMsg( ERR | PRNTSTR, msg );

    err = DotCList( DOT_ERROR );
    if( err != NULL ) {
        ++cListCount;
        if( ExecCList( err ) != RET_SUCCESS ) {
            PrtMsg( FTL | S_COMMAND_RET_BAD, DotNames[DOT_ERROR] );
        }
    } else if( !(targ->attr.prec || targ->attr.symb) ) {
        if( !Glob.hold && targExists( targ ) ) {
            if( Glob.erase || GetYes( SHOULD_FILE_BE_DELETED ) ) {
                if( unlink( targ->node.name ) != 0 ) {
                    PrtMsg( FTL | SYSERR_DELETING_FILE, targ->node.name );
                }
            }
        }
    }
    if( Glob.cont ) {
        return( RET_WARN );
    }
    return( RET_ERROR );
}
#ifdef __WATCOMC__
#pragma off(check_stack);
#endif


STATIC time_t maxDepTime( time_t max_time, DEPEND *dep )
/******************************************************/
{
    TLIST   *targets;

    for( targets = dep->targs; targets != NULL; targets = targets->next ) {
        if( targets->target->date <= max_time )
            continue;
        if( targets->target->date == YOUNGEST_DATE )
            continue;
        max_time = targets->target->date;
    }
    return( max_time );
}


STATIC time_t findMaxTime( TARGET *targ, DEPEND *imp_dep, time_t max_time )
/*************************************************************************/
{
    DEPEND  *dep;

    max_time = maxDepTime( max_time, imp_dep );
    for( dep = targ->depend; dep != NULL; dep = dep->next ) {
        max_time = maxDepTime( max_time, dep );
    }
    return( max_time );
}


STATIC RET_T perform( TARGET *targ, DEPEND *dep, time_t max_time )
/****************************************************************/
{
    CLIST   *clist;
    CLIST   *before;
    RET_T   ret;
    DEPEND  *depend;
    DEPEND  *impliedDepend;

    depend = NULL;
    impliedDepend = NULL;
    assert( targ != NULL && dep != NULL );

    if( Glob.query ) {
        ++cListCount;
        return( RET_WARN );
    }
    if( Glob.touch ) {
        ++cListCount;
        ResetExecuted();
        if( targ->attr.symb == FALSE ) {
            CacheRelease();
            if( TouchFile( targ->node.name ) != RET_SUCCESS ) {
                PrtMsg( ERR | COULD_NOT_TOUCH, targ->node.name );
                return( RET_ERROR );
            }
        }
        targ->touched = TRUE;
        return( RET_SUCCESS );
    }

    /* means that this is a sufsuf made implicit rule */
    if( dep->slistCmd == NULL ) {
        clist = dep->clist;
        depend = dep;
        impliedDepend = NULL;
    } else {
        clist = DupCList( dep->slistCmd->clist );
        depend = targ->depend;
        impliedDepend = dep;
    }
    if( clist == NULL ) {
        clist = DotCList( DOT_DEFAULT );
        if( clist == NULL ) {
            // No commands in Microsoft and POSIX mode is considered OK
            // and executed
            if( Glob.compat_nmake || Glob.compat_posix ) {
                targ->cmds_done = TRUE;
                return( RET_SUCCESS );
            }
            if( targ->attr.symb != FALSE ) {    /* 13-Dec-90 DJG */
                targ->cmds_done = TRUE;
                return( RET_SUCCESS );
            }
            if( targ->allow_nocmd ) {
                /* for UNIX folks: make target symbolic */
                targ->attr.symb = TRUE;
                return( RET_SUCCESS );
            }
            PrtMsg( FTL | NO_DEF_CMDS_FOR_MAKE,
                DotNames[DOT_DEFAULT], targ->node.name );
        }
    }
    if( !Glob.noexec ) {
        ResetExecuted();
    }
    if( !doneBefore ) {
        before = DotCList( DOT_BEFORE );
        if( before != NULL ) {
            ++cListCount;
            if( ExecCList( before ) != RET_SUCCESS ) {
                PrtMsg( FTL | S_COMMAND_RET_BAD, DotNames[DOT_BEFORE] );
            }
        }
        doneBefore = TRUE;
    }
    exPush( targ, depend, impliedDepend );
    ret = carryOut( targ, clist, findMaxTime( targ, dep, max_time ) );
    exPop();
    if( dep->slistCmd != NULL ) {
        FreeCList(clist);
    }
    if( ret == RET_ERROR ) {
        exit( ExitSafe( EXIT_ERROR ) );
    }
    return( ret );
}


RET_T MakeList( TLIST *tlist )
/***********************************/
{
    RET_T   ret;
    TARGET  *targ;

    ret = RET_SUCCESS;
    while( tlist != NULL ) {
        targ = tlist->target;
        tlist = tlist->next;
        if( targ->mentioned == FALSE ) {
            PrtMsg( WRN | TARGET_NOT_MENTIONED, targ->node.name );
            targ->mentioned = TRUE;
        }   /* warning suggested by John */
        if( Update( targ ) != RET_SUCCESS ) {
            ret = RET_ERROR;
        }
    }
    return( ret );
}


STATIC void implyDebugInfo( TARGET *targ, UINT32 startcount )
/***********************************************************/
{
    enum MsgClass   msg;

    if( Glob.debug ) {
        if( targExists( targ ) ) {
            if( startcount != cListCount ) {
                msg = M_HAD_TO_BE_UPDATED;
            } else {
                msg = M_IS_CLEAR_WITH;
            }
        } else {
            msg = M_COULD_NOT_BE_IMPLIED;
        }
        PrtMsg( DBG | INF | IMP_ENV_M, targ->node.name, msg );
    }
}

STATIC BOOLEAN autoDepCompare( time_t targ_time, time_t auto_time )
/*****************************************************************/
{
    if( dateCmp( targ_time, auto_time ) < 0 ) {
        return( TRUE );
    }
    return( FALSE );
}


STATIC BOOLEAN checkForAutoDeps( TARGET *targ, char *name, time_t *max_time )
/***************************************************************************/
{
    return( AutoDepCheck( name, targ->date, autoDepCompare, max_time ) );
}


STATIC BOOLEAN autoOutOfDate( TARGET *targ, time_t *max_time )
/************************************************************/
{
    char    buffer[_MAX_PATH];

    if( TrySufPath( buffer, targ->node.name, NULL, FALSE ) != RET_SUCCESS ) {
        return( FALSE );
    }
    return( checkForAutoDeps( targ, buffer, max_time ) );
}


STATIC RET_T isOutOfDate( TARGET *targ, TARGET *deptarg, BOOLEAN *outofdate )
/****************************************************************************
 * Checks if the current target is out of date
 */
{
    getDate( targ );
    if( targ->existing && targ->attr.existsonly ) {
        return( RET_SUCCESS );
    }
    getDate( deptarg );
    if( targ->existing && deptarg->existing && deptarg->attr.existsonly ) {
        return( RET_SUCCESS );
    }
    if( dateCmp( targ->date, deptarg->date ) < 0 ) {
        *outofdate = TRUE;
        if( Glob.show_offenders ) {
            PrtMsg( INF | WILL_BE_BUILT_BECAUSE_OF,
                targ->node.name, deptarg->node.name);
        }
    }
    if( deptarg->error ) {
       /* one of the targets had an error while being updated
        * abort now
        */
        return( RET_ERROR );
    }
    if( (!deptarg->attr.recheck && deptarg->cmds_done) || deptarg->backdated ) {
        *outofdate = TRUE;
    }
    return( RET_SUCCESS );
}


STATIC RET_T implyMaybePerform( TARGET *targ, TARGET *imptarg,
    TARGET *cretarg, BOOLEAN must, SLIST *slistCmd )
/*************************************************************
 * perform cmds if targ is older than imptarg || must
 *
 * targ     is the target to be updated
 * imptarg  is the dependent for target (ie: "targ : imptarg" )
 * cretarg  is the implicit rule to use
 * must     must we do it?
 */
{
    RET_T   ret;
    DEPEND  *newdep;
    TLIST   *newtlist;
    time_t  max_time;

    max_time = OLDEST_DATE;
    if( imptarg->error ) {
        /* there was an error making imptarg before, so just abort */
        return( RET_ERROR );
    }
    if( cretarg->attr.always ) {
        must = TRUE;
    }

    if( isOutOfDate( targ, imptarg, &must ) == RET_ERROR ) {
        return( RET_ERROR );
    }

    if( !must && USE_AUTO_DEP( cretarg ) ) {
        if( autoOutOfDate( targ, &max_time ) ) {
            must = TRUE;
        }
    }

    if( must ) {
        assert( cretarg->depend != NULL );

        /* construct a depend for perform */

        newdep           = DupDepend( cretarg->depend );
        newtlist         = NewTList();
        newtlist->target = imptarg;
        newdep->targs    = newtlist;
        newdep->slistCmd = slistCmd;

        /* handle implied attributes (.symb/.prec/.multi) */
        TargOrAttr( targ, cretarg->attr );

        ret = perform( targ, newdep, max_time );
        FreeDepend( newdep );           /* don't need depend any more */

        if( ret != RET_SUCCESS ) {
            return( RET_ERROR );
        }

        if( Glob.noexec || Glob.query ) {               /* 29-oct-90 */
            targ->executed = TRUE;
            targ->touched = TRUE;
        }
    }

    return( RET_SUCCESS );
}


STATIC RET_T imply( TARGET *targ, const char *drive, const char *dir,
    const char *fname, const char *ext, BOOLEAN must )
/********************************************************************
 * targ     is the target to be implied
 * drive    is the drive of the target
 * dir      is the path of the target
 * fname    is the portion of targ's name without the extension
 * ext      is the extension of targ's name
 * must     must we make this target?
 *
 * RET_SUCCESS - performed cmds,
 * RET_WARN unable to imply,
 * RET_ERROR - perform failed
 */
{
    SUFFIX      *srcsuf;
    CREATOR     *cur;
    SUFFIX      *cursuf;
    TARGET      *imptarg = NULL;
    RET_T       ret;
    BOOLEAN     newtarg;
    UINT32      startcount;
    char        *buf;
    SLIST       *curslist;
    SLIST       *slistCmd;  // Slist chosen for sufsuf
    SLIST       *slistDef;  // Slist that has dependent path = ""
    SLIST       *slistEmptyTargDepPath;
    BOOLEAN     UseDefaultSList;
    int         slistCount;

    srcsuf = FindSuffix( ext );
    if( srcsuf == NULL || srcsuf->creator == NULL ) {
        PrtMsg( DBG | INF | IMP_ENV_M, targ->node.name, M_HAS_NO_IMPLICIT );
        return( RET_WARN );
    }
    PrtMsg( DBG | INF | IMP_ENV_M, targ->node.name, M_CHECKING_IMPLICIT );
    startcount = cListCount;

    for( cur = srcsuf->creator; cur != NULL; cur = cur->next ) {
        cursuf = cur->suffix;

        /* allocate a buffer */
        buf = MallocSafe( _MAX_PATH );
        slistCmd = NULL;
        slistDef = NULL;
        slistEmptyTargDepPath = NULL;

        assert( cur->cretarg         != NULL );
        assert( cur->cretarg->depend != NULL );
        curslist = cur->cretarg->depend->slist;

        ret = RET_ERROR;

        UseDefaultSList = TRUE;
        slistCount = 0;
        /* find path in SLIST */
        while( curslist != NULL && ret != RET_SUCCESS ) {
            _makepath( buf, drive, dir, NULL, NULL );
            /*
             * note the path of the current target must match the
             * path as specified in the slist
             */
            if( strcmpi( buf, curslist->targ_path ) == 0 ) {
                /* build filename for implied target */
                _makepath( buf, NULL, curslist->dep_path,
                           fname, cursuf->node.name );
                /* try to find this file on path or in targets */
                ret = TrySufPath( buf, buf, &imptarg, FALSE );
                if( ret == RET_SUCCESS ) {
                    slistCmd = curslist;
                /* later on we need to check if implied target does not */
                /* exist we need to create it on the first directory we */
                /* see on the SLIST since                               */
                /* the first on the list is the one that was defined    */
                /* last in the makefile                                 */
                } else if( slistDef == NULL ) {
                    slistDef = curslist;
                }
            }
            if( curslist->targ_path[0] == NULLCHAR &&
                curslist->dep_path[0]  == NULLCHAR ) {
                slistEmptyTargDepPath = curslist;
            }

            if( slistCount > 0 && slistEmptyTargDepPath != NULL ) {
                UseDefaultSList = FALSE;
            }
            curslist = curslist->next ;
            ++slistCount;
        }

        if( UseDefaultSList && slistCmd  == NULL && !Glob.compat_nmake ) {
            _makepath( buf, NULL, NULL, fname, cursuf->node.name );
            /* try to find this file on path or in targets */
            ret = TrySufPath( buf, buf, &imptarg, FALSE );
            switch( ret ) {
            case RET_WARN:
                break;
            case RET_ERROR:
                if( !Glob.compat_nmake ) {
                    slistDef = slistEmptyTargDepPath;
                }
                break;
            case RET_SUCCESS:
                slistCmd = slistEmptyTargDepPath;
                break;
            }
        }


        if( (ret == RET_SUCCESS && imptarg == NULL) || ret == RET_ERROR ) {
            /* Either file doesn't exist, or it exists and we don't already
             * have a target for it.  Either way, we create a new target.
             */
            if( ret == RET_ERROR ) {
                /*
                 * No Default Slist found so must continue and find
                 * another slist
                 */
                if( slistDef == NULL ) {
                    FreeSafe( buf );
                    continue;
                } else {
                    slistCmd = slistDef;
                }
                    /* file doesn't exist, assume in directory */
                    /* pointed to by the slistDef              */
                _makepath( buf, NULL, slistCmd->dep_path,
                           fname, cursuf->node.name );

            }
            newtarg = TRUE;
            imptarg = NewTarget( buf );
            FreeSafe( buf );        /* don't need any more */
            getStats( imptarg );
            imptarg->busy = TRUE;   /* protect against recursion */
            if( imply( imptarg, NULL, slistCmd->dep_path,
                        fname, cursuf->node.name, FALSE ) ==
                RET_ERROR ) {
                imptarg->error = TRUE;
            }
            if( startcount != cListCount && (Glob.noexec || Glob.query) ) {
                imptarg->touched = TRUE;
                imptarg->executed = TRUE;       /* 29-oct-90 */
            }
            imptarg->updated = TRUE;
            imptarg->busy = FALSE;
        } else {
            /* We already know about imptarg, so just update it */
            assert( imptarg != NULL );
            FreeSafe( buf );        /* don't need any more */
            newtarg = FALSE;        /* this isn't a new target */
            Update( imptarg );
        }

        /* We've tried our best to make the imptarg, check if it exists
         * after our efforts.
         */
        if( targExists( imptarg ) ) {
            /* it exists - now we perform the implicit cmd list, and return */
            ret = implyMaybePerform( targ, imptarg, cur->cretarg, must,
                slistCmd );
            if( newtarg && !Glob.noexec ) {
                /* destroy the implied target, because the info in the target
                 * structure is nicely stored on disk (unless Glob.noexec)
                 */
                KillTarget( imptarg->node.name );
            }
            implyDebugInfo( targ, startcount );
            return( ret );
        } else if( newtarg ) {
            /* we created an unsuccessful target - so destroy it */
            KillTarget( imptarg->node.name );
        }

        /* We couldn't imply with this suffix... try next one */
    }
    implyDebugInfo( targ, startcount );
    return( RET_WARN );
}


STATIC RET_T tryImply( TARGET *targ, BOOLEAN must )
/*************************************************/
{
    PGROUP  pg;
    RET_T   ret;

    if( Glob.block ) {
        return( RET_WARN );
    }

    _splitpath2( targ->node.name, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );

    ret = imply( targ, pg.drive, pg.dir, pg.fname, pg.ext, must );

    return( ret );
}


STATIC void ExpandWildCards( TARGET *targ, DEPEND *depend )
/**********************************************************
 * Expand the wild cards now
 * also deMacroSpecial macros
 */
{
   TLIST    *tlist;
   TLIST    *currentEnd;
   TLIST    *outTList;
   TLIST    *temp;
   char     *NodeName;

   assert( depend != NULL );

   tlist = depend->targs;
   currentEnd = outTList = NULL;
   while( tlist != NULL ) {
       temp = NULL;
       // In Microsoft it is possible to have macros in the dependency.
       if( Glob.compat_nmake ) {
           exPush( targ, NULL, NULL );
           NodeName = DeMacroSpecial( tlist->target->node.name );
           exPop();
           WildTList( &temp, NodeName, TRUE, TRUE );
           FreeSafe( NodeName );
       } else {
           WildTList( &temp, tlist->target->node.name, TRUE, TRUE );
       }
       tlist = tlist->next;
       if( outTList != NULL ) {
           currentEnd->next = temp;
       } else {
           outTList = temp;
       }

       if( temp != NULL ) {
           currentEnd = temp;
           /* find the current end */
           while( currentEnd->next != NULL ) {
               currentEnd = currentEnd -> next;
           }
       }
   }
   FreeTList( depend->targs );
   depend->targs = outTList;
}


STATIC RET_T resolve( TARGET *targ, DEPEND *depend )
/***************************************************
 * If there are no dependents, then perform the associated commands, if req'd.
 * If there are dependents: update them all; compare the date of each dep
 * to the targ; if any of the deps had an error previously, abort; if targ
 * is out of date then perform the clist (or attempt to imply).
 */
{
    TLIST       *tlist;
    TARGET      *curtarg;
    RET_T       tmp;
    BOOLEAN     outofdate;
    BOOLEAN     exec_cmds;
    time_t      max_time;

    max_time = OLDEST_DATE;

    ExpandWildCards( targ, depend );

    if( depend->targs == NULL ) {
        /* 10-oct-90 AFS make the target if it doesn't exist or it's symbolic */
        /* 30-oct-90 AFS a "::" target with no dependents must be made */
        exec_cmds = FALSE;
        if( !targ->scolon || !targ->existing ) {
            exec_cmds = TRUE;
        }
        if( targ->attr.symb || targ->attr.always ) {
            exec_cmds = TRUE;
        }
        /* 11-sep-92 AFS if all targets must be made, so should this one */
        if( Glob.all ) {
            exec_cmds = TRUE;
        }
        if( !exec_cmds && USE_AUTO_DEP( targ ) ) {
            if( autoOutOfDate( targ, &max_time ) ) {
                exec_cmds = TRUE;
            }
        }
        if( exec_cmds ) {
            return( perform( targ, depend, max_time ) );
        }
        return( RET_SUCCESS );
    }

    MakeList( depend->targs );

    getDate( targ );

    outofdate = FALSE;
    tlist = depend->targs;
    while( tlist != NULL ) {    /* check if out of date with deps */
        curtarg = tlist->target;
        tlist = tlist->next;    /* advance the tlist */

        if( isOutOfDate( targ, curtarg, &outofdate ) == RET_ERROR ) {
            return( RET_ERROR );
        }
    }
    if( targ->attr.always ) {
        outofdate = TRUE;
    }
    if( !outofdate && USE_AUTO_DEP( targ ) ) {
        if( autoOutOfDate( targ, &max_time ) ) {
            outofdate = TRUE;
        }
    }
    if( outofdate ) {
        /* if we get this far, then none of the deps had a previous error,
         * and at least one of the deps is newer than targ, so we perform
         */
        if( depend->clist != NULL ) {
            return( perform( targ, depend, max_time ) );
        } else {
            tmp = tryImply( targ, TRUE );
            if( tmp == RET_WARN ) {
                /* couldn't imply - will do DEFAULT cmds */
                return( perform( targ, depend, max_time ) );
            }
            return( tmp );
        }
    }

    return( RET_SUCCESS );
}


RET_T Update( TARGET *targ )
/*********************************/
{
    DEPEND      *curdep;
    UINT32      startcount;
    BOOLEAN     target_exists;
    RET_T       ret;

    CheckForBreak();
    if( targ->error ) {
        return( RET_ERROR );
    }
    if( targ->updated ) {
        return( RET_SUCCESS );
    }
    if( targ->special ) {
        PrtMsg( FTL | ATTEMPT_MAKE_SPECIAL, targ->node.name );
    }
    if( targ->busy ) {
        PrtMsg( FTL | RECURSIVE_DEFINITION, targ->node.name );
    }
    PrtMsg( DBG | INF | NEOL | UPDATING_TARGET, targ->node.name );
    targ->busy = TRUE;
    targExists( targ );     /* find file using sufpath */
    startcount = cListCount;

    if( targ->depend == NULL ||
        (targ->depend->clist == NULL && targ->depend->targs == NULL) ) {
                    /* has no depend/explicit rules */
        PrtMsg( DBG | INF | M_EXPLICIT_RULE, M_NO );
        ret = tryImply( targ, FALSE );
        if( ret == RET_ERROR ) {
            targ->busy = FALSE;
            targ->error = TRUE;
            return( RET_ERROR );
        } else if( ret == RET_WARN ) {
            // If target with no commands is acceptable, consider it done
            if( targ->allow_nocmd ) {
                targ->cmds_done = TRUE;
            }
        }
    } else if( targ->scolon == FALSE ) {    /* double colon */
        PrtMsg( DBG | INF | M_EXPLICIT_RULE, M_DCOLON );
        curdep = targ->depend;
        while( curdep != NULL ) {
            if( resolve( targ, curdep ) != RET_SUCCESS ) {
                targ->busy = FALSE;
                targ->error = TRUE;
                return( RET_ERROR );
            }
            curdep = curdep->next;
        }
        if( !Glob.compat_nmake ) {
            if( tryImply( targ, FALSE ) == RET_ERROR ) {
                targ->busy = FALSE;
                targ->error = TRUE;
                return( RET_ERROR );
            }
        }
    } else {
        PrtMsg( DBG | INF | M_EXPLICIT_RULE, M_SCOLON );
        if( resolve( targ, targ->depend ) != RET_SUCCESS ) {
            targ->busy = FALSE;
            targ->error = TRUE;
            return( RET_ERROR );
        }
    }

    if( (targ->attr.symb || Glob.noexec || Glob.query)
        && startcount != cListCount ) {
        targ->existing = TRUE;
        targ->touched = TRUE;
        targ->executed = FALSE;
        targ->date = YOUNGEST_DATE;
    }

    target_exists = targExists( targ );                         /* 18-nov-91 */
    if( target_exists || targ->attr.symb || Glob.ignore ) {
        // Target exists or it is symbolic or we're ignoring errors,
        // therefore everyone's happy and we can charge forward
        PrtMsg( DBG | INF | TARGET_IS_UPDATED, targ->node.name );
    } else {
        // Target doesn't exist, we may be in trouble
        if( targ->cmds_done && Glob.nocheck ) {
            // Target doesn't exist even though we processed some commands,
            // but we're not checking existence of files. Consider it uptodate.
            targ->existing = TRUE;
            PrtMsg( DBG | INF | TARGET_FORCED_UPTODATE, targ->node.name );
        } else if( Glob.cont ) {
            // Target doesn't exist but we're forcibly continuing. Report
            // nonfatal error.
            targ->error = TRUE;
            targ->busy = FALSE;
            PrtMsg( ERR | UNABLE_TO_MAKE, targ->node.name );
            return( RET_ERROR );
        } else {
            // Target doesn't exist and we have no clue how to make it. Bomb out.
            PrtMsg( FTL | UNABLE_TO_MAKE, targ->node.name );
            return( RET_ERROR );
        }
    }

    targ->updated = targ->attr.multi == FALSE;
    targ->busy = FALSE;
    targ->error = FALSE;
    return( RET_SUCCESS );
}


static struct exStack exGetCurVars( void )
/*****************************************
 * Go up through the exStack, and return the most local non-null
 * target and dep pointers; or NULL if none exist.
 */
{
    int             walk;
    int             top;
    struct exStack  buf;
    DEPEND          *curdep;
    DEPEND          *curimpDep;
    TARGET          *curtarg;

    buf.dep = NULL;
    buf.targ = NULL;
    buf.impDep = NULL;
    curtarg = NULL;
    curdep = NULL;
    top = exStackP - 1;
    for( walk = top; walk >= 0; --walk ) {
        if( buf.dep == NULL ) {
            curdep = exStack[walk].dep;
            if( curdep != NULL && curdep->targs != NULL ) {
                buf.dep = curdep;
            }
        }
        if( buf.impDep == NULL ) {
            curimpDep = exStack[walk].impDep;
            if( curimpDep != NULL && curimpDep->targs != NULL ) {
                buf.impDep = curimpDep;
            }
        }
        if( buf.targ == NULL ) {
            curtarg = exStack[walk].targ;
            if( curtarg != NULL && ! curtarg->attr.symb ) {
                /* we want non-NULL and non-SYMBOLIC! (30-jan-92 AFS) */
                buf.targ = curtarg;
            }
        }
    }
    if( buf.targ == NULL ) {
        /* no non-symbolic targets? return current symbolic target */
        if( top >= 0 ) {
            buf.targ = exStack[top].targ;
        }
    }

    return( buf );
}


char *GetCurDeps( BOOLEAN younger, BOOLEAN isMacInf )
/**********************************************************/
{
    TLIST           *walk;
    VECSTR          vec;
    BOOLEAN         written;
    TARGET          *targ;
    struct exStack  cur;
    const char      *formattedString;
    char            *ret;

    cur = exGetCurVars();

    // This is for Glob.compat_nmake and Glob.compat_posix
    // $< and $** are different
    if( isMacInf ) {
        cur.dep = cur.impDep;
    } else {
        if( cur.dep == NULL ) {
            cur.dep = cur.impDep;
        }
    }

    if( (younger && cur.targ == NULL) ||
        cur.dep == NULL || cur.dep->targs == NULL ) {
        return( StrDupSafe( "" ) );
    }

    vec = StartVec();
    written = FALSE;

    walk = cur.dep->targs;
    while( walk != NULL ) {
        targ = walk->target;
        if( !younger || dateCmp( cur.targ->date, targ->date ) < 0 ) {
            if( written ) {
                WriteVec( vec, " " );
            }
            formattedString = procPath( targ->node.name );
            WriteVec( vec, formattedString );
            written = TRUE;
        }
        walk = walk->next;
    }
    ret = FinishVec( vec );

    return( ret );
}


const char *GetCurTarg( void )
/***********************************/
{
    struct exStack  cur;

    cur = exGetCurVars();
    if( cur.targ == NULL ) {
        return( NULL );
    }
    return( cur.targ->node.name );
}


const char *GetFirstDep( void )
/************************************/
{
    struct exStack  cur;

    cur = exGetCurVars();
    if( cur.impDep != NULL ) {
        cur.dep = cur.impDep;
    }
    if( cur.dep != NULL && cur.dep->targs != NULL ) {
        return( cur.dep->targs->target->node.name );
    }
    return( NULL );
}


const char *GetLastDep( void )
/***********************************/
{
    struct exStack  cur;
    TLIST           *walk;

    cur = exGetCurVars();
    if( cur.impDep != NULL ) {
        cur.dep = cur.impDep;
    }
    if( cur.dep != NULL && cur.dep->targs != NULL ) {
        walk = cur.dep->targs;
        while( walk->next != NULL ) {
            walk = walk->next;
        }
        return( walk->target->node.name );
    }
    return( NULL );
}


void UpdateInit( void )
/****************************/
{
    /* according to ANSI standard... this should be true */

    assert(     doneBefore == FALSE
            &&  cListCount == 0ul
            &&  exStackP == 0
    );
    DoingUpdate = TRUE;
}


void UpdateFini( void )
/****************************/
{
    CLIST   *after;

    assert( exStackP == 0 );

    after = DotCList( DOT_AFTER );
    if( doneBefore && after != NULL ) {
        ++cListCount;
        if( ExecCList( DotCList( DOT_AFTER ) ) != RET_SUCCESS ) {
            PrtMsg( ERR | S_COMMAND_RET_BAD, DotNames[DOT_AFTER] );
        }
    }
    DoingUpdate = FALSE;
}
