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
* Description:  INQUIRE statement processor
*
****************************************************************************/

#include "ftnstd.h"
#include "ftextfun.h"
#include "undef.h"
#include "rundat.h"
#include "pgmacc.h"
#include "rtsysutl.h"
#include "rtutls.h"

#include <string.h>

/* Forward declarations */
static  void    RTFill( void PGM *dest, int size );
static  void    StrFill( string PGM *scb );
static  void    RTCopy( char *ptr, string PGM *str );

static  char            *ModeAcc[] = {
        "",              // dummy entry
        "SEQUENTIAL",    // ACCM_SEQUENTIAL
        "DIRECT",        // ACCM_DIRECT
        "SEQUENTIAL"     // ACCM_APPEND
        };

static  char            *ModeSeq[] = {
        "UNKNOWN",       // ACCM DEFAULT
        "YES",           // ACCM_SEQUENTIAL
        "NO"             // ACCM_DIRECT
        };

static  char            *ModeDir[] = {
        "UNKNOWN",       // ACCM DEFAULT
        "NO",            // ACCM_SEQUENTIAL
        "YES"            // ACCM_DIRECT
        };

static  char            *Form[] = {
        "",              // dummy entry
        "FORMATTED",     // FORMATTED_IO
        "UNFORMATTED"    // UNFORMATTED_IO
        };

static char             *Fmtd[] = {
        "UNKNOWN",       // FORMATTED_DFLT
        "YES",           // FORMATTED_IO
        "NO"             // UNFORMATTED_IO
         };

static  char            *UnFmtd[] = {
        "UNKNOWN",       // FORMATTED_DFLT
        "NO",            // FORMATTED_IO
        "YES"            // UNFORMATTED_IO
        };

static  char            *Blank[] = {
        "",              // dummy entry
        "NULL",          // BLANK_NULL
        "ZERO"           // BLANK_ZERO
        };

static  char            *CCtrl[] = {
        "",
        "YES",          // CC_YES
        "NO"            // CC_NO
        };

static  char            *RecType[] = {
        "UNKNOWN",      // RECFM_DEFAULT
        "FIXED",        // RECFM_FIXED
        "VARIABLE"      // RECFM_VARIABLE
        "TEXT"          // RECFM_TEXT
        };

static  char            *Action[] = {
        "UNKNOWN",      // ACT_DEFAULT
        "READ",         // ACTION_READ
        "WRITE",        // ACTION_WRITE
        "READWRITE"     // ACTION_RW
        };

static  char            *Share[] = {
        "UNKNOWN",      // SHARE_DEFAULT
        "COMPAT",       // SHARE_COMPAT
        "DENYRW",       // SHARE_DENYRW
        "DENYWR"        // SHARE_DENYWR
        "DENYRD"        // SHARE_DENYRD
        "DENYNO"        // SHARE_DENYNO
        };


bool    FindFName( void ) {
//===================

    ftnfile     *fcb;
    char        *fname;

    ConnectFile();
    fname = IOCB->fileinfo->filename;
    fcb = Files;
    for(;;) {
        if( fcb == NULL )
            return( false );
        if( ( IOCB->fileinfo != fcb ) && SameFile( fname, fcb->filename ) )
            break;
        fcb = fcb->link;
    }
    // file already connected
    DiscoFile( IOCB->fileinfo );
    IOCB->fileinfo = fcb;
    return( true );
}


void    DfltInq( void ) {
//=================

    if( IOCB->flags & UNDCHK_IO ) {
        if( IOCB->set_flags & SET_NUMBPTR ) {
            RTFill( IOCB->numbptr, sizeof( intstar4 ) );
        }
        if( IOCB->set_flags & SET_NMDPTR ) {
            RTFill( IOCB->nmdptr, sizeof( logstar4 ) );
        }
        if( IOCB->set_flags & SET_NAMEPTR ) {
            StrFill( IOCB->nameptr );
        }
        if( IOCB->set_flags & SET_ACCPTR ) {
            StrFill( IOCB->accptr );
        }
        if( IOCB->set_flags & SET_SEQPTR ) {
            StrFill( IOCB->seqptr );
        }
        if( IOCB->set_flags & SET_DIRPTR ) {
            StrFill( IOCB->dirptr );
        }
        if( IOCB->set_flags & SET_FORMPTR ) {
            StrFill( IOCB->formptr );
        }
        if( IOCB->set_flags & SET_FMTDPTR ) {
            StrFill( IOCB->fmtdptr );
        }
        if( IOCB->set_flags & SET_UFMTDPTR ) {
            StrFill( IOCB->ufmtdptr );
        }
        if( IOCB->set_flags & SET_BSIZEPTR ) {
            RTFill( IOCB->bsizeptr, sizeof( intstar4 ) );
        }
        if( IOCB->set_flags & SET_LENPTR ) {
            RTFill( IOCB->lenptr, sizeof( intstar4 ) );
        }
        if( IOCB->set_flags & SET_NRECPTR ) {
            RTFill( IOCB->nrecptr, sizeof( intstar4 ) );
        }
        if( IOCB->set_flags & SET_BLNKPTR ) {
            StrFill( IOCB->blnkptr );
        }
        if( IOCB->set_flags & SET_RECFMPTR ) {
            StrFill( IOCB->recfmptr );
        }
        if( IOCB->set_flags & SET_CCTRLPTR ) {
            StrFill( IOCB->cctrlptr );
        }
        if( IOCB->set_flags & SET_ACTPTR ) {
            StrFill( IOCB->actptr );
        }
        if( IOCB->set_flags & SET_SHARE ) {
            StrFill( IOCB->shareptr );
        }
    }
    if( IOCB->set_flags & SET_IOSPTR ) {
        // INQUIRE by unit may not have a connected file
        if( ( IOCB->fileinfo != NULL ) && ( IOCB->fileinfo->error != -1 ) ) {
            *IOCB->iosptr = IOCB->fileinfo->error;
        } else {
            *IOCB->iosptr = 0;
        }
    }
}


void    InqExList( void ) {
//===================

    ftnfile     *fcb;

    fcb = IOCB->fileinfo;
    if( IOCB->set_flags & SET_NMDPTR ) {
        *IOCB->nmdptr = fcb->filename != NULL;
    }
    if( ( IOCB->set_flags & SET_NAMEPTR ) && ( fcb->filename != NULL ) ) {
        RTCopy( fcb->filename, IOCB->nameptr );
    }
    if( IOCB->set_flags & SET_SEQPTR ) {
        RTCopy( ModeSeq[ InqSeq( fcb ) ], IOCB->seqptr );
    }
    if( IOCB->set_flags & SET_DIRPTR ) {
        RTCopy( ModeDir[ InqDir( fcb ) ], IOCB->dirptr );
    }
    if( IOCB->set_flags & SET_FMTDPTR ) {
        RTCopy( Fmtd[ InqFmtd( fcb ) ], IOCB->fmtdptr );
    }
    if( IOCB->set_flags & SET_UFMTDPTR ) {
        RTCopy( UnFmtd[ InqUnFmtd( fcb ) ], IOCB->ufmtdptr );
    }
}


void    InqOdList( void ) {
//===================

    ftnfile     *fcb;
    byte        spec;

    fcb = IOCB->fileinfo;
    if( IOCB->set_flags & SET_NUMBPTR ) {
        *IOCB->numbptr = fcb->unitid;
    }
    if( IOCB->set_flags & SET_RECFMPTR ) {
        spec = fcb->recfm;
        if( spec == RECFM_DEFAULT ) {
            spec = DfltRecType( fcb );
        }
        RTCopy( RecType[ spec ], IOCB->recfmptr );
    }
    if( IOCB->set_flags & SET_CCTRLPTR ) {
        spec = fcb->cctrl;
        if( spec == CC_DEFAULT ) {
            spec = CC_NO;
        }
        RTCopy( CCtrl[ spec ], IOCB->cctrlptr );
    }
    if( IOCB->set_flags & SET_ACTPTR ) {
        spec = fcb->action;
        if( spec == ACT_DEFAULT ) {
            spec = ACTION_RW;
        }
        RTCopy( Action[ spec ], IOCB->actptr );
    }
    if( IOCB->set_flags & SET_SHARE ) {
        spec = fcb->share;
        if( spec == SHARE_DEFAULT ) {
            spec = SHARE_COMPAT;
        }
        RTCopy( Share[ spec ], IOCB->shareptr );
    }
    if( IOCB->set_flags & SET_ACCPTR ) {
        spec = fcb->accmode;
        if( spec == ACCM_DEFAULT ) {
            spec = ACCM_SEQUENTIAL;
        }
        RTCopy( ModeAcc[ spec ], IOCB->accptr );
    }
    spec = fcb->formatted;
    if( spec == FORMATTED_DFLT ) {
        spec = FORMATTED_IO;
        if( fcb->accmode == ACCM_DIRECT ) {
            spec = UNFORMATTED_IO;
        }
    }
    if( IOCB->set_flags & SET_FORMPTR ) {
        RTCopy( Form[ spec ], IOCB->formptr );
    }
    if( IOCB->set_flags & SET_BSIZEPTR ) {
        *IOCB->bsizeptr = fcb->blocksize;
    }
    if( IOCB->set_flags & SET_LENPTR ) {
        *IOCB->lenptr = fcb->bufflen;
    }
    if( fcb->accmode == ACCM_DIRECT ) {
        if( IOCB->set_flags & SET_NRECPTR ) {
            *IOCB->nrecptr = fcb->recnum;
        }
    }
    // At this point, "spec" still contains the format.
    if( ( IOCB->set_flags & SET_BLNKPTR ) && ( spec == FORMATTED_IO ) ) {
        spec = fcb->blanks;
        if( spec == BLANK_DEFAULT ) {
            spec = BLANK_NULL;
        }
        RTCopy( Blank[ spec ], IOCB->blnkptr );
    }
}


static  void    RTCopy( char *ptr, string PGM *str ) {
//====================================================

    uint        len;
    uint        scb_len;

    len = strlen( ptr );
    scb_len = str->len;
    if( len <= scb_len ) {
        pgm_memput( str->strptr, ptr, len );
        pgm_memset( str->strptr + len, ' ', scb_len - len );
    } else {
        pgm_memput( str->strptr, ptr, scb_len );
    }
}


static  void    RTFill( void PGM *dest, int size ) {
//==================================================

    pgm_memset( dest, UNDEF_CHAR, size );
}


static  void    StrFill( string PGM *scb ) {
//==========================================

    pgm_memset( scb->strptr, UNDEF_CHAR, scb->len );
}
