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
* Description:  Run-time utilities.
*
****************************************************************************/


#include "ftnstd.h"
#include "ftextfun.h"
#include "ftextvar.h"
#include "units.h"
#include "rundat.h"
#include "errcod.h"
#include "rmemmgr.h"
#include "deffname.h"
#include "rtspawn.h"
#include "rterr.h"
#include "rtsysutl.h"
#include "rtutls.h"
#include "ioerr.h"

#include <string.h>


static ftnfile *SearchFtnFile( int unit ) {
//=========================================

    ftnfile     *ffile;

    ffile = Files;
    for(;;) {
        if( ffile == NULL ) break;
        if( unit == ffile->unitid ) {
            return( ffile );
        }
        ffile = ffile->link;
    }
    return( NULL );
}


bool    FindFtnFile( void ) {
//=====================

    ftnfile     *ffile;
    void        *handle;
    int         unit;

    ffile = SearchFtnFile( IOCB->unitid );
    if( ffile != NULL ) {
        IOCB->fileinfo = ffile;
        return( true );
    }
    unit = IOCB->unitid;
    IOCB->fileinfo = NULL;
    handle = LocUnit( IOCB->unitid );
    if( handle != NULL ) {
        F_Connect();
        ExtractInfo( handle, IOCB->fileinfo );
        ChkFileName();
        return( true );
    } else if( ((unit == STANDARD_INPUT) || (unit == PRE_STANDARD_INPUT)) &&
               (IOCB->iostmt == IO_READ) ) {
        IOCB->fileinfo = SearchFtnFile( STANDARD_INPUT );
        if( IOCB->fileinfo == NULL ) {
            IOCB->fileinfo = _InitStandardInput();
            // in case unit=PRE_STANDARD_INPUT
            IOCB->fileinfo->unitid = unit;
        }
        return( true );
    } else if( ((unit == STANDARD_OUTPUT) || (unit == PRE_STANDARD_OUTPUT)) &&
               (IOCB->iostmt == IO_WRITE) ) {
        IOCB->fileinfo = SearchFtnFile( STANDARD_OUTPUT );
        if( IOCB->fileinfo == NULL ) {
            IOCB->fileinfo = _InitStandardOutput();
            // in case unit=PRE_STANDARD_OUTPUT
            IOCB->fileinfo->unitid = unit;
        }
        return( true );
    } else {
        return( false );
    }
}


void    *RChkAlloc( uint size ) {
//===============================

    void        *ptr;

    ptr = RMemAlloc( size );
    if( ptr == NULL ) {
        RTErr( MO_DYNAMIC_OUT );
    }
    return( ptr );
}


void    F_Connect( void ) {
//===================

    ftnfile     *filecb;

    filecb = RChkAlloc( sizeof( ftnfile ) );
    memset( filecb, 0, sizeof( ftnfile ) );
    filecb->link     = Files;
    Files            = filecb;
    filecb->unitid   = IOCB->unitid;
    filecb->bufflen  = ( IOCB->set_flags & SET_RECL ) ? IOCB->recl : 0;
    filecb->recnum   = 1;               // for sequential files
    filecb->internal = ( IOCB->set_flags & SET_INTERNAL ) ? IOCB->internal:NULL;
    IOCB->fileinfo = filecb;
}


void    ConnectFile( void ) {
//=====================

    F_Connect();
    GetFileInfo();
}


void    GetFileInfo( void ) {
//=====================

    char        *fname;
    void        *handle;
    uint        unit;
    string      name;
    uint        i;

    if( ( IOCB->set_flags & SET_FILENAME ) == 0 ) {
        handle = LocUnit( IOCB->unitid );
        if( handle == NULL ) {
            fname = RChkAlloc( DFLT_FNAME_LEN );
            strcpy( fname, DefFName );
            unit = IOCB->unitid;
            for( i = 1; i <= MAX_UNIT_DIGITS; ++i ) {
                fname[ UNIT_OFFSET + MAX_UNIT_DIGITS - i ] = '0' + unit % 10;
                unit = unit / 10;
            }
        } else {
            ExtractInfo( handle, IOCB->fileinfo );
            fname = IOCB->fileinfo->filename;
        }
    } else {
        TrimStr( IOCB->filename, &name );
        fname = RChkAlloc( name.len + sizeof( char ) );
        GetStr( &name, fname );
    }
    IOCB->fileinfo->filename = fname;
    ChkFileName();
}

static  void    SysFileInfo( void ) {
//=============================

    GetSysFileInfo( IOCB->fileinfo );
}

void    DiscoFile( ftnfile *old ) {
//=================================

    ftnfile     *chaser;

    if( old == Files ) {
        Files = Files->link;
    } else {
        chaser = Files;
        while( chaser->link != old ) {
            chaser = chaser->link;
        }
        chaser->link = chaser->link->link;
    }
    if( old->internal == NULL ) {
        if( old->filename != NULL ) {
            RMemFree( old->filename );
        }
        if( old->buffer != NULL ) {
            RMemFree( old->buffer );
        }
    } else {
        RMemFree( old->buffer );
    }
    RMemFree( old );
}

void    ChkFileName( void ) {
//=====================

    ChkLogFile();
    if( RTSpawn( &SysFileInfo ) != 0 ) {
        DiscoFile( IOCB->fileinfo );
        RTSuicide();
    }
}


void    ChkLogFile( void ) {
//====================

    pointer     handle;

    handle = LocFile( IOCB->fileinfo->filename );
    if( handle != NULL ) {
        RMemFree( IOCB->fileinfo->filename );
        ExtractInfo( handle, IOCB->fileinfo );
    }
}



void    ChkIOOperation( ftnfile *fcb ) {
//======================================

    if( IOCB->iostmt == IO_READ ) {
        if( fcb->action == ACTION_WRITE ) {
            IOErr( IO_BAD_ACTION );
        }
    } else {    // must be WRITE or ENDFILE
        if( fcb->action == ACTION_READ ) {
            IOErr( IO_BAD_ACTION );
        }
    }
}


bool    GetIOErr( ftnfile *fcb ) {
//================================

    if( fcb->internal != NULL )
        return( false );
    return( Errf( fcb ) );
}


void    ChkIOErr( ftnfile *fcb ) {
//================================

    if( GetIOErr( fcb ) ) {
        IOErr( IO_FILE_PROBLEM );
    }
}


void    ChkUnitId( void ) {
//===================

    if( IOCB->flags & BAD_UNIT ) {
        IOErr( IO_IUNIT );
    } else if( IOCB->unitid > PREC_MAX_UNIT ) {
        IOErr( IO_UNIT_EXIST );
    }
}


void    ChkConnected( void ) {
//======================

    if( IOCB->fileinfo == NULL ) {
        IOErr( IO_NOT_CONNECTED );
    }
}


void    SetEOF( void ) {
//================

    IOCB->fileinfo->flags |= FTN_EOF;
    IOCB->fileinfo->error = -1;
    IOCB->status = -1;
}


void    ClearEOF( void ) {
//==================

    ftnfile     *fcb;

    fcb = IOCB->fileinfo;
    if( NoEOF( fcb ) || ( fcb->accmode == ACCM_DIRECT ) ) {
        fcb->flags &= ~FTN_EOF;
        fcb->error = 0;
        IOCB->status = 0;
        SysClearEOF( fcb );
    }
}


void    SysEOF( void ) {
//================

    SetEOF();
    if( ( IOCB->set_flags & (SET_EOFSTMT|SET_IOSPTR) ) == 0 ) {
        ReportEOF( IOCB->fileinfo );
        IOErr( IO_FILE_PROBLEM );
    }
    RTSuicide();
}


bool    IsFixed( void ) {
//=================

    return( IOCB->fileinfo->accmode == ACCM_DIRECT );
}


bool    IsCarriage( void ) {
//====================

    return( IOCB->fileinfo->cctrl == CC_YES );
}


void    ChkRecordStructure( void ) {
//============================

    if( _NoRecordOrganization( IOCB->fileinfo ) ) {
        IOErr( IO_NO_RECORDS );
    }
}


void    ChkSequential( int errmsg ) {
//===================================

    if( IOCB->fileinfo->accmode > ACCM_SEQUENTIAL ) {
        IOErr( errmsg );
    }
}


void    ChkExist( void ) {
//==================

    if( (IOCB->fileinfo->flags & FTN_FSEXIST) == 0 ) {
        ReportNExist( IOCB->fileinfo );
        IOErr( IO_FILE_PROBLEM );
    }
}


uint    StrItem( string PGM *strptr, char *buff, uint buff_len ) {
//================================================================

// Remove leading and trailing blanks from the string, copy the result
// into "buffer" and return the new length.

    string      str;

    TrimStr( strptr, &str );
    if( str.len > buff_len ) {
        str.len = buff_len;
    }
    GetStr( &str, buff );
    return( str.len );
}
