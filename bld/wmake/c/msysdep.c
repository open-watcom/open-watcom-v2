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
#include "msysdep.h"
#include "mupdate.h"

#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#if defined( __DOS__ )
#include <dos.h>
#include <limits.h>

#include "tinyio.h"
#endif
#include "pcobj.h"

#if defined( __DOS__ )

int __far critical_error_handler( unsigned deverr,
                                  unsigned errcode,
                                  unsigned far *devhdr )
{
    deverr=deverr;errcode=errcode;devhdr=devhdr;
    return( _HARDERR_FAIL );
}

void InitHardErr( void )
{
    _harderr( critical_error_handler );
}

#else

void InitHardErr( void )
{
}

#endif

#if defined( __DOS__ )
extern char             DOSSwitchChar(void);
#pragma aux             DOSSwitchChar = \
        "mov ax,3700h"  \
        "int 21h"       \
        parm caller     [] \
        value           [dl] \
        modify          [ax dx];
#endif

extern int SwitchChar( void )
/***************************/
{
#if defined( __DOS__ )
    return( DOSSwitchChar() );
#elif   defined( __WINDOWS__ )
    return( '/' );
#elif   defined( __OS2__ ) || defined( __NT__ )
    return( '/' );
#elif   defined( __QNX__ )
    return( '-' );

#endif
}

#if defined( __DOS__ )
/* see page 90-91 of "Undocumented DOS" */

extern void far *       _DOS_list_of_lists( void );
#pragma aux             _DOS_list_of_lists = \
        "mov ax,5200h"  \
        "int 21h"       \
        parm caller     [] \
        value           [es bx] \
        modify          [ax es bx];

extern int OSCorrupted( void )
/****************************/
{
    struct mcb {
        uint_8  id;
        uint_16 owner;
        uint_16 len;
    } far *chain;
    uint_16 far *first_MCB;
    uint_16 chain_seg;
    uint_16 new_chain_seg;

    first_MCB = _DOS_list_of_lists();
    if( FP_OFF( first_MCB ) == 1 ) {    /* next instr will hang! */
        /* list of lists DOS call may have been interrupted */
        return( 1 );
    }
    chain_seg = first_MCB[-1];
    for(;;) {
        chain = MK_FP( chain_seg, 0 );
        if( chain->id == 'Z' ) {
            break;
        }
        if( chain->id != 'M' ) {
            return( 1 );
        }
        new_chain_seg = chain_seg + ( chain->len + 1 );
        if( new_chain_seg <= chain_seg ) {
            return( 1 );
        }
        chain_seg = new_chain_seg;
    }
    return( 0 );
}
#else
extern int OSCorrupted( void )
/****************************/
{
    return( 0 );
}
#endif

#if defined( __DOS__ )
extern RET_T TouchFile( const char *name )
/****************************************/
{
    tiny_date_t     dt;
    tiny_time_t     tm;
    tiny_ftime_t    p_hms;
    tiny_fdate_t    p_ymd;
    tiny_ret_t      ret;

    ret = TinyOpen( name, TIO_WRITE );
    if( TINY_OK( ret ) ) {
        dt = TinyGetDate();
        p_ymd.year = dt.year + ( 1900 - 1980 );
        p_ymd.month = dt.month;
        p_ymd.day = dt.day_of_month;

        tm = TinyGetTime();
        p_hms.hours = tm.hour;
        p_hms.minutes = tm.minutes;
        p_hms.twosecs = tm.seconds / 2;

        TinySetFileStamp( TINY_INFO( ret ), p_hms, p_ymd );
        TinyClose( TINY_INFO( ret ) );
    } else {
        ret = TinyCreate( name, TIO_NORMAL );
        if( TINY_OK( ret ) ) {
            TinyClose( TINY_INFO( ret ) );
        } else {
            return( RET_ERROR );
        }
    }
    return( RET_SUCCESS );
}
#else

#include <sys/stat.h>
#include <sys/types.h>
#if defined(__QNX__)
 #include <utime.h>
#else
 #include <sys/utime.h>
#endif

extern RET_T TouchFile( const char *name )
/****************************************/
{
    int fh;

    if( utime( name, 0 ) < 0 ) {
        fh = creat( name, 0 );
        if( fh < 0 ) {
            return( RET_ERROR );
        }
        close( fh );
    }
    return( RET_SUCCESS );
}
#endif

#if 0
static BOOLEAN verifyOBJFile( int fh )
/************************************/
{
    auto struct {
        obj_record      header;
        obj_name        name;
    } theadr;

    if( lseek( fh, 0, SEEK_SET ) < 0 ) {
        return( FALSE );
    }
    if( read( fh, &theadr, sizeof(theadr) ) != sizeof(theadr) ) {
        return( FALSE );
    }
    if( theadr.header.command != CMD_THEADR ) {
        return( FALSE );
    }
    if(( theadr.name.len + 2 ) != theadr.header.length ) {
        return( FALSE );
    }
    if( lseek( fh, 0, SEEK_SET ) < 0 ) {
        return( FALSE );
    }
    return( TRUE );
}
#endif

#define FUZZY_DELTA     60      /* max allowed variance from stored time-stamp */

BOOLEAN IdenticalAutoDepTimes( time_t in_obj, time_t stamp )
/**********************************************************/
{
    time_t diff_time;

    if( in_obj == stamp ) {
        return( TRUE );
    }
    if( in_obj < stamp ) {
        /* stamp is newer than time in .OBJ file */
        if( Glob.fuzzy ) {
            /* check for a "tiny" difference in times (almost identical) */
            diff_time = stamp - in_obj;
            if( diff_time <= FUZZY_DELTA ) {
                return( TRUE );
            }
        }
    }
#if 0
    // 9.5 C/C++ Windows/NT compilers have bad fstat() functions
    // so allow differences that may be timezone problems
    // (assume timezones are units of 30min)
    if((( in_obj - stamp ) % (30L*60L) ) == 0 ) {
        return( TRUE );
    }
#endif
    return( FALSE );
}

#if 0
autodep_ret_t SysOBJAutoDep( char *name, time_t stamp, BOOLEAN (*chk)(time_t,time_t), time_t *pmax_time )
/*******************************************************************************************************/
{
    autodep_ret_t rc;
    time_t max_time;
    time_t dep_time;
    time_t DOS_stamp_time;
    char *buff;
    int fh;
    size_t len;
    auto obj_record header;
    auto struct {
        uint_8          bits;
        uint_8          type;
        uint_16         dos_time;
        uint_16         dos_date;
        uint_8          name_len;
    } comment;
    auto struct {
        unsigned out_of_date : 1;
        unsigned exists : 1;
    } flag;

    rc = SOA_NOT_AN_OBJ;
    fh = open( name, O_RDONLY|O_BINARY );
    if( fh < 0 ) {
        return( rc );
    }
    max_time = *pmax_time;
    flag.out_of_date = FALSE;
    flag.exists = FALSE;
    if( verifyOBJFile( fh ) ) {
        buff = name;
        for(;;) {
            if( read( fh, &header, sizeof(header) ) != sizeof(header) ) {
                break;
            }
            if( header.command != CMD_COMENT ) {
                if( header.command == CMD_LNAMES ) {
                    /* first LNAMES record means object file doesn't have deps */
                    break;
                }
                lseek( fh, header.length, SEEK_CUR );
                continue;
            }
            if( read( fh, &comment, sizeof(comment) ) != sizeof(comment) ) {
                break;
            }
            if( comment.type != CMT_DEPENDENCY ) {
                lseek( fh, (fpos_t)header.length - sizeof(comment), SEEK_CUR );
                continue;
            }
            if( header.length < sizeof( comment ) ) {
                /* null dependency ends our search */
                break;
            }
            len = comment.name_len;
            /* read in the checksum byte to stay in synch */
            ++len;
            if( read( fh, buff, len ) != len ) {
                break;
            }
            buff[len-1] = '\0';
            if( CacheTime( buff, &dep_time ) != RET_SUCCESS ) {
                /* doesn't exist anymore so rebuild the file */
                flag.out_of_date = TRUE;
            } else {
                flag.exists = TRUE;
                DOS_stamp_time = SysDOSStampToTime( comment.dos_date,
                                                    comment.dos_time );
                if( ! IdenticalAutoDepTimes( DOS_stamp_time, dep_time ) ) {
                    /* time-stamp isn't identical so rebuild */
                    flag.out_of_date = TRUE;
                } else if( (*chk)( stamp, dep_time ) ) {
                    flag.out_of_date = TRUE;
                }
                /* we don't want Glob.all affecting the comparison */
                if( dep_time > max_time ) {
                    max_time = dep_time;
                }
            }
            if( Glob.debug ) {
                char        time_buff[ 20 ];    /* large enough for date */
                struct tm   *tm;

                if( ! flag.exists ) {
                    time_buff[0] = '?';
                    time_buff[1] = '\0';
                } else {
                    tm = localtime( &DOS_stamp_time );
                    FmtStr( time_buff, "%D-%s-%D  %D:%D:%D",
                            tm->tm_mday, MonthNames[ tm->tm_mon ], tm->tm_year,
                            tm->tm_hour, tm->tm_min, tm->tm_sec
                        );
                }
                PrtMsg( DBG|INF| GETDATE_MSG, time_buff, buff );
            }
            if( flag.out_of_date && !Glob.rcs_make ) {
                /* we don't need to calculate the real maximum time */
                break;
            }
        }
        if( flag.out_of_date ) {
            rc = SOA_BUILD_OBJ;
        } else {
            rc = SOA_UP_TO_DATE;
        }
    }
    *pmax_time = max_time;
    close( fh );
    return( rc );
}
#endif

enum {
    TIME_SEC_B  = 0,
    TIME_SEC_F  = 0x001f,
    TIME_MIN_B  = 5,
    TIME_MIN_F  = 0x07e0,
    TIME_HOUR_B = 11,
    TIME_HOUR_F = 0xf800
};

enum {
    DATE_DAY_B  = 0,
    DATE_DAY_F  = 0x001f,
    DATE_MON_B  = 5,
    DATE_MON_F  = 0x01e0,
    DATE_YEAR_B = 9,
    DATE_YEAR_F = 0xfe00
};

time_t SysDOSStampToTime( unsigned short date, unsigned short time )
/******************************************************************/
{
    struct tm tmbuf;

    tmbuf.tm_year = ( ( date & DATE_YEAR_F ) >> DATE_YEAR_B ) + 80;
    tmbuf.tm_mon  = ( ( date & DATE_MON_F ) >> DATE_MON_B ) - 1;
    tmbuf.tm_mday = ( date & DATE_DAY_F ) >> DATE_DAY_B;

    tmbuf.tm_hour = ( time & TIME_HOUR_F ) >> TIME_HOUR_B;
    tmbuf.tm_min  = ( time & TIME_MIN_F ) >> TIME_MIN_B;
    tmbuf.tm_sec  = ( ( time & TIME_SEC_F ) >> TIME_SEC_B ) * 2;

    tmbuf.tm_isdst= -1;

    return( mktime( &tmbuf ) );
}


#ifdef DLLS_IMPLEMENTED

#include "idedrv.h"

static DLL_CMD *dllCommandList;

void OSLoadDLL( char *cmd_name, char *dll_name, char *ent_name )
/**************************************************************/
{
    DLL_CMD *n;

    // we want newer !loaddlls to take precedence
    n = MallocSafe( sizeof( *n ) );
    n->cmd_name = StrDupSafe( cmd_name );
    n->next = dllCommandList;
    dllCommandList = n;
    IdeDrvInit( &n->inf
              , StrDupSafe( dll_name )
              , ( ent_name == NULL ) ? NULL : StrDupSafe( ent_name ) );
}

DLL_CMD* OSFindDLL( char const *cmd_name )
/****************************************/
{
    DLL_CMD* n;
    for( n = dllCommandList; n != NULL; n = n->next ) {
        if( 0 == stricmp( cmd_name, n->cmd_name ) ) break;
    }
    return n;
}

#define DLL_PREFIX "DLL:"
#define DLL_PSIZE sizeof( DLL_PREFIX ) - 1

int OSExecDLL( DLL_CMD* dll, char const* cmd_args )
/*************************************************
 * Returns the error code returned by IdeDrvDLL
 */
{
    int retcode = IdeDrvExecDLL( &dll->inf, cmd_args );
    setmode( STDOUT_FILENO, O_TEXT );
#if 0
    if( 0 != retcode ) {
        size_t size = strlen( dll->inf.dll_name ) + DLL_PSIZE;
        char*p = alloca( size );
        memcpy( p, DLL_PREFIX, DLL_PSIZE );
        memcpy( p + DLL_PSIZE, dll->inf.dll_name, size - DLL_PSIZE );
    }
#endif
    return retcode;
}
#else

DLL_CMD* OSFindDLL( char const *cmd_name )
/****************************************/
{
    cmd_name = cmd_name;
    return NULL;
}

void OSLoadDLL( char *cmd_name, char *dll_name, char *ent_name )
/**************************************************************/
{
    cmd_name = cmd_name;
    dll_name = dll_name;
    ent_name = ent_name;
}

int OSExecDLL( DLL_CMD* dll, char const* cmd_args )
/*************************************************/
{
    dll = dll;
    cmd_args = cmd_args;
    return( -1 );
}

#endif

#ifndef NDEBUG
STATIC void cleanDLLCmd ( void ) {
#ifdef DLLS_IMPLEMENTED
    DLL_CMD* n;
    DLL_CMD* temp;
    n  = dllCommandList;
    while ( n != NULL) {
        FreeSafe ((char*) n->cmd_name);
        if (n->inf.dll_name != NULL) {
            FreeSafe( (char*) n->inf.dll_name );
        }
        if (n->inf.ent_name != NULL ) {
            FreeSafe ( (char*) n->inf.ent_name );
        }
        temp = n;
        n = n->next;
        FreeSafe (temp);
    }
#endif
}
#endif



#ifndef NDEBUG
extern void DLLFini ( void ) {
    cleanDLLCmd();
}
#endif

static sig_atomic_t sig_count;

void CheckForBreak( void ) {
    if( sig_count > 0 ) {
        sig_count = 0;
        PrtMsg(ERR|USER_BREAK_ENCOUNTERED);
        ExitSafe(EXIT_ERROR);
    }
}

#ifndef NEC_98_BUG
static void passOnBreak( void ) {
#ifdef DLLS_IMPLEMENTED
    DLL_CMD* n;

    for( n = dllCommandList; n != NULL; n = n->next ) {
        IdeDrvStopRunning( &n->inf );
    }
#endif
}
static void breakHandler (int  sig_number) {
    sig_count = 1;
    passOnBreak();
}
#endif

extern void InitSignals( void ) {
    sig_count = 0;
    DoingUpdate= FALSE;
#ifndef NEC_98_BUG
    signal ( SIGBREAK, breakHandler);
    signal ( SIGINT, breakHandler);
#endif
}
