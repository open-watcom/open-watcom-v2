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
* Description:  Run-time OPEN statement processor
*
****************************************************************************/

#include "ftnstd.h"
#include "ftextfun.h"
#include "ftextvar.h"
#include "errcod.h"
#include "rundat.h"
#include "rtenv.h"
#include "runmain.h"


//
// AccModTab - access mode table
//

char *AccModTab[] = {
        "sequential",
        "direct",
        "append",
        NULL };

//
// FormTab - formatted or unformatted table
//

char *FormTab[] = {
        "formatted",
        "unformatted",
        NULL };

//
// OpStatTab - open status table
//

char *OpStatTab[] = {
        "unknown",
        "old",
        "new",
        "scratch",
        NULL };

//
// BlnkTab - blank table
//

char *BlnkTab[] = {
        "null",
        "zero",
        NULL };

//
// RecFmTab - record format table
//

char *RecFmTab[] = {
        "fixed",
        "variable",
        "text",
        NULL };

//
// CCtrlTab - carriage control table
//

char *CCtrlTab[] = {
        "yes",
        "no",
        NULL };

//
// ActionTab - action table
//

char *ActionTab[] = {
        "read",
        "write",
        "readwrite",
        NULL };

//
// ShareTab - share table
//

char *ShareTab[] = {
        "compat",
        "denyrw",
        "denywr",
        "denyrd",
        "denynone",
        NULL };

static  void    ExOpen( void ) {
//========================

    ftnfile     *fcb;
    byte        accmode;
    byte        form;
    byte        status;
    byte        blanks;
    byte        recfm;
    byte        cctrl;
    byte        action;
    byte        share;
    bool        log;
    bool        connected;
    void        *temp;

    ChkUnitId();
    FindFtnFile();
    fcb = IOCB->fileinfo;
    status = FindKWord( OpStatTab, STAT_SPEC, STATUS_UNKNOWN,
        ( IOCB->set_flags & SET_STATPTR ) ? IOCB->statptr : NULL );
    accmode = FindKWord( AccModTab, ACC_SPEC, ACCM_DEFAULT,
        ( IOCB->set_flags & SET_ACCPTR ) ? IOCB->accptr : NULL );
    form = FindKWord( FormTab, FORM_SPEC, FORMATTED_DFLT,
        ( IOCB->set_flags & SET_FORMPTR ) ? IOCB->formptr : NULL );
    blanks = FindKWord( BlnkTab, BLNK_SPEC, BLANK_DEFAULT,
        ( IOCB->set_flags & SET_BLNKPTR ) ? IOCB->blnkptr : NULL );
    recfm = FindKWord( RecFmTab, RECFM_SPEC, RECFM_DEFAULT,
        ( IOCB->set_flags & SET_RECFMPTR ) ? IOCB->recfmptr : NULL );
    cctrl = FindKWord( CCtrlTab, CCTRL_SPEC, CC_DEFAULT,
        ( IOCB->set_flags & SET_CCTRLPTR ) ? IOCB->cctrlptr : NULL );
    action = FindKWord( ActionTab, ACTION_SPEC, ACT_DEFAULT,
        ( IOCB->set_flags & SET_ACTPTR ) ? IOCB->actptr : NULL );
    share = FindKWord( ShareTab, SHARE_SPEC, SHARE_DEFAULT,
        ( IOCB->set_flags & SET_SHARE ) ? IOCB->shareptr : NULL );
    if( IOCB->flags & BAD_RECL ) {
        IOErr( IO_IRECL );
    }
    if( IOCB->flags & BAD_BLOCKSIZE ) {
        IOErr( IO_IBLOCKSIZE );
    }
    if( (status != STATUS_SCRATCH) || !(IOCB->set_flags & SET_FILENAME) ) {
        if( (status == STATUS_OLD) || (status == STATUS_NEW) ) {
            if( !(IOCB->set_flags & SET_FILENAME) ) {
                IOErr( IO_SNAME );
            }
        }
    } else {
        IOErr( IO_SNAME );
    }
    if( !(IOCB->set_flags & SET_RECL) ) {
        if( accmode == ACCM_DIRECT ) {
            IOErr( IO_RACCM );
        }
    }
    F_Connect();
    // Set the status since VAX/VMS needs the information to create a new
    // version of the file.
    IOCB->fileinfo->status = status;
    GetFileInfo();
    log = FALSE;
    connected = FALSE;
    if( fcb == NULL ) {
        // file not connected
        fcb = IOCB->fileinfo;
    } else {
        connected = TRUE;
        log = (fcb->flags & FTN_LOG_IO) != 0;
        if( fcb->accmode == ACCM_DEFAULT ) {
            // OPEN of a preconnected file
            if( IOCB->set_flags & SET_FILENAME ) {
                // use the given name
                DiscoFile( fcb );
                fcb = IOCB->fileinfo;
            } else {
                // use the pre-connected name
                DiscoFile( IOCB->fileinfo );
                IOCB->fileinfo = fcb;
            }
        } else if( (IOCB->set_flags & SET_FILENAME) &&
                   !SameFile( IOCB->fileinfo->filename, fcb->filename ) ) {
            // if OPENing a new file, close the old one
            temp = IOCB->fileinfo;
            IOCB->fileinfo = fcb;
            CloseFile( fcb );
            if( (fcb->status == STATUS_SCRATCH) &&
                (fcb->flags & FTN_FSEXIST) && !Scrtched( fcb ) ) {
                DiscoFile( fcb );
                IOErr( IO_FILE_PROBLEM );
            }
            DiscoFile( fcb );
            fcb = temp;
            IOCB->fileinfo = temp;
        } else {
            // OPENing an open file, check only BLANK= has changed
            DiscoFile( IOCB->fileinfo );
            if( status == STATUS_UNKNOWN ) {
                status = fcb->status;
            }
            if( accmode == ACCM_DEFAULT ) {
                accmode = fcb->accmode;
            }
            if( form == FORMATTED_DFLT ) {
                form = fcb->formatted;
            }
            if( !(IOCB->set_flags & SET_RECL) ) {
                IOCB->recl = fcb->bufflen;
                IOCB->set_flags |= SET_RECL;
            }
            if( !(IOCB->set_flags & SET_BLOCKSIZE) ) {
                IOCB->blocksize = fcb->blocksize;
                IOCB->set_flags |= SET_BLOCKSIZE;
            }
            if( blanks == BLANK_DEFAULT ) {
                blanks = fcb->blanks;
            }
            if( recfm == RECFM_DEFAULT ) {
                recfm = fcb->recfm;
            }
            if( cctrl == CC_DEFAULT ) {
                cctrl = fcb->cctrl;
            }
            if( action == ACT_DEFAULT ) {
                action = fcb->action;
            }
            if( share == SHARE_DEFAULT ) {
                share = fcb->share;
            }
            if( status != fcb->status ) {
                IOErr( IO_SUBSEQUENT_OPEN, SpecId[ STAT_SPEC ] );
            } else if( accmode != fcb->accmode ) {
                IOErr( IO_SUBSEQUENT_OPEN, SpecId[ ACC_SPEC ] );
            } else if( form != fcb->formatted ) {
                IOErr( IO_SUBSEQUENT_OPEN, SpecId[ FORM_SPEC ] );
            } else if( IOCB->recl != fcb->bufflen ) {
                IOErr( IO_SUBSEQUENT_OPEN, SpecId[ RECL_SPEC ] );
            } else if( IOCB->blocksize != fcb->blocksize ) {
                IOErr( IO_SUBSEQUENT_OPEN, SpecId[ BLOCKSIZE_SPEC ] );
            } else if( recfm != fcb->recfm ) {
                IOErr( IO_SUBSEQUENT_OPEN, SpecId[ RECFM_SPEC ] );
            } else if( cctrl != fcb->cctrl ) {
                IOErr( IO_SUBSEQUENT_OPEN, SpecId[ CCTRL_SPEC ] );
            } else if( action != fcb->action ) {
                IOErr( IO_SUBSEQUENT_OPEN, SpecId[ ACTION_SPEC ] );
            } else if( share != fcb->share ) {
                IOErr( IO_SUBSEQUENT_OPEN, SpecId[ SHARE_SPEC ] );
            }
            fcb->blanks = blanks;
            IOCB->fileinfo = fcb;
            if( fcb->fileptr == NULL ) {
                DoOpen();
            }
            return;
        }
    }
    // set up defaults for new file
    if( blanks == BLANK_DEFAULT ) {
        blanks = BLANK_NULL;
    }
    if( accmode == ACCM_DEFAULT ) {
        accmode = ACCM_SEQUENTIAL;
    }
    if( form == FORMATTED_DFLT ) {
        if( accmode == ACCM_DIRECT ) {
            form = UNFORMATTED_IO;
        } else {
            form = FORMATTED_IO;
        }
    }
    if( form == UNFORMATTED_IO ) {
        if( IOCB->set_flags & SET_BLNKPTR ) {
            if( !connected ) {
                DiscoFile( fcb );
            }
            IOErr( IO_BLNK_FMT );
        } else if( IOCB->set_flags & SET_CCTRLPTR ) {
            if( !connected ) {
                DiscoFile( fcb );
            }
            IOErr( IO_CC_FORM );
        }
    }
    if( cctrl == CC_DEFAULT ) {
        if( IsDevice( fcb ) && __DevicesCC() ) {
            cctrl = CC_YES;
        } else {
            cctrl = CC_NO;
        }
    }
    if( action == ACT_DEFAULT ) {
        action = ACTION_RW;
    }
    if( share == SHARE_DEFAULT ) {
        share = SHARE_COMPAT;
    }
    // check for status/file existence errors
    if( ((status == STATUS_OLD) && !(fcb->flags & FTN_FSEXIST)) ||
        ((status == STATUS_NEW) && (fcb->flags & FTN_FSEXIST)) ) {
        if( !connected ) {
            DiscoFile( fcb );
        }
        IOErr( IO_SFILE );
    }
    // if record length was given, use it
    if( IOCB->set_flags & SET_RECL ) {
        fcb->bufflen = IOCB->recl;
    }
    // if block size was given, use it
    if( IOCB->set_flags & SET_BLOCKSIZE ) {
        fcb->blocksize = IOCB->blocksize;
    }
    // if action was given or the file does not exist, use it
    // if the file exists, fcb->action will have been set by GetSysFileInfo()
    if( ( IOCB->set_flags & SET_ACTPTR ) || !(fcb->flags & FTN_FSEXIST) ) {
        fcb->action = action;
    }
    // Set up the new ftnfile structure - must be done before SysCreateFile()
    // as VAX/VMS needs to know the access mode to create the file.
    fcb->formatted = form;
    fcb->accmode = accmode;
    fcb->blanks = blanks;
    fcb->flags |= FTN_EXIST;
    if( log ) {
        fcb->flags |= FTN_LOG_IO;
    }
    fcb->recfm = recfm;
    fcb->cctrl = cctrl;
    fcb->share = share;
    // if the status is NEW, create the file and set status to OLD
    if( status == STATUS_NEW ) {
        SysCreateFile( fcb );
        status = STATUS_OLD;
    }
    fcb->status = status;
    // _NoRecordOrganization() assumes all fields have been set
    if( _NoRecordOrganization( fcb ) ) {
        fcb->recnum = 0;
    } else {
        fcb->recnum = 1;
    }
    if( fcb->fileptr == NULL ) {
        DoOpen();
    }
}


int     IOOpen( void ) {
//================

    IOCB->iostmt = IO_OPEN;
    return( IOMain( &ExOpen ) );
}
