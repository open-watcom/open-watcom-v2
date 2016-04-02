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
* Description:  system dependent I/O support.
*
****************************************************************************/


#include "ftnstd.h"
#include "ftextvar.h"
#include "rundat.h"
#include "errcod.h"
#include "units.h"
#include "fio.h"
#include "posio.h"
#include "fapptype.h"
#include "rmemmgr.h"
#include "posopen.h"
#include "posget.h"
#include "posput.h"
#include "posseek.h"
#include "posrew.h"
#include "poserr.h"
#include "chrutils.h"
#include "rtsysutl.h"
#include "rtutls.h"
#include "ioerr.h"
#include "posflush.h"
#include "posdel.h"
#include "postrunc.h"
#include "posback.h"

#include <string.h>
#include <ctype.h>
#include <errno.h>
#if defined( __IS_WINDOWED__ )
  #ifndef __SW_BW
    #define  __SW_BW
    #include <wdefwin.h>
    #undef   __SW_BW
  #else
    #include <wdefwin.h>
  #endif
#endif


/* Forward declarations */
static  void    SysIOInfo( ftnfile *fcb );
static  void    ChkDisk( ftnfile *fcb );

/*
extern  file_handle     FStdIn;
extern  file_handle     FStdOut;
extern  file_handle     FStdErr;
*/

#define SYS_DFLT_RECSIZE        1024;

#define INFO_DEV                0x80    // indicates file is a device
#define INFO_VALID_DRIVE        0x40    // indicates valid drive letter
#define INFO_DRIVE              0x3f    // mask for drive number


void    GetSysIOInfo( ftnfile *fcb ) {
//====================================

// Get system file information for an open file.

    SysIOInfo( fcb );
    // if standard output device is carriage control (/cc option)
    if( fcb->cctrl == CC_YES ) {
        ((a_file *)(fcb->fileptr))->attrs |= CARRIAGE_CONTROL;
    }
}


void    GetSysFileInfo( ftnfile *fcb ) {
//======================================

// Get system file information for a file name.

    if( access( fcb->filename, F_OK ) == 0 ) {
        fcb->flags |= FTN_FSEXIST;
    } else {
        fcb->flags &= ~FTN_FSEXIST;
    }
    SysIOInfo( fcb );
}


bool    IsDevice( ftnfile *fcb ) {
//================================

// Make sure that the file is a disk file.

    return( ( fcb->device & INFO_DEV ) != 0 );
}


static  char    *GetSysName( ftnfile *fcb ) {
//===========================================

// Return a system file name given a user name and a file structure.

    char        buff[_MAX_PATH];
    char        *p;

#if defined( __DOS__ ) || defined( __WINDOWS__ ) || (defined( __OS2__ ) && defined( _M_I86 ))
    p = JmpBlanks( fcb->filename );
    if( IsDevice( fcb ) ) {
        strcpy( buff, p );
    } else
#endif
    {
        p = _fullpath( buff, fcb->filename, _MAX_PATH );
    }
    if( p != NULL ) {
        p = RMemAlloc( strlen( buff ) + sizeof( char ) );
        strcpy( p, buff );
    }
    return( p );
}


static  void    SysIOInfo( ftnfile *fcb ) {
//=========================================

// Get system file information.

    struct stat         info;
    char                *sys_name;
    bool                exist = TRUE;

    if( fcb->bufflen == 0 ) {
        fcb->bufflen = SYS_DFLT_RECSIZE;
    }
    if( fcb->blocksize == 0 ) {
        fcb->blocksize = IO_BUFFER;
    }
    fcb->device = 0;
    if( fcb->fileptr != NULL ) { // file is open
#if defined( __NETWARE__ )
        if( ( ((a_file *)(fcb->fileptr))->handle == STDIN_FILENO ) ||
            ( ((a_file *)(fcb->fileptr))->handle == STDOUT_FILENO ) ||
            ( ((a_file *)(fcb->fileptr))->handle == STDERR_FILENO ) ) {
            fcb->device |= INFO_DEV;
        } else {
#endif
        // for stdin, don't use file name "CON" since information will always
        // indicate it's a device even if stdin is redirected
            if( fstat( ((a_file *)(fcb->fileptr))->handle, &info ) == -1 ) {
                FSetSysErr( fcb->fileptr );
                IOErr( IO_FILE_PROBLEM );
                return;
            }
            if( S_ISCHR( info.st_mode ) ) {
                fcb->device |= INFO_DEV;
#if defined( __DOS__ ) || defined( __WINDOWS__ )
            } else {
                fcb->device |= INFO_VALID_DRIVE;
#endif
            }
#if defined( __NETWARE__ )
        }
#endif
    } else {
        if( stat( fcb->filename, &info ) == -1 ) {
            // if we are trying to open a file in a non-existent
            // directory we don't want to issue an error
            if( fcb->flags & FTN_FSEXIST ) {
                FSetSysErr( fcb->fileptr );
                IOErr( IO_FILE_PROBLEM );
                return;
            }
            exist = FALSE;
        } else if( S_ISCHR( info.st_mode ) ) {
            fcb->device |= INFO_DEV;
            // devices always exist
            fcb->flags |= FTN_FSEXIST;
#if !defined( __UNIX__ )
        } else {
            fcb->device |= INFO_VALID_DRIVE;
#endif
        }
    }
    if( ( fcb->flags & FTN_FSEXIST ) && !IsDevice( fcb ) ) {
#if !defined( __UNIX__ )
        // Assume the two most significant bits contain no useful information
        fcb->device = INFO_DRIVE & info.st_dev; // save drive letter
#endif
        if( ( info.st_mode & S_IRUSR ) && ( info.st_mode & S_IWUSR ) ) {
            fcb->action = ACTION_RW;
        } else if( info.st_mode & S_IRUSR ) {
            fcb->action = ACTION_READ;
        } else if( info.st_mode & S_IWUSR ) {
            fcb->action = ACTION_WRITE;
        } else {
            // if none of the above are set,
            // assume read/write
            fcb->action = ACTION_RW;
        }
    }
    sys_name = GetSysName( fcb );
    if( sys_name == NULL ) {
        if( exist ) {
            FSetSysErr( fcb->fileptr );
            IOErr( IO_FILE_PROBLEM );
        }
        return;
    }
    RMemFree( fcb->filename );
    fcb->filename = sys_name;
}


void    OpenAction( ftnfile *fcb ) {
//==================================

// Open a file.

    SetIOBufferSize( fcb->blocksize );
    fcb->fileptr = Openf( fcb->filename, _FileAttrs( fcb ) );
    if( fcb->fileptr != NULL ) {
        if( ((a_file *)(fcb->fileptr))->attrs & CHAR_DEVICE ) {
            // In dos box under NT we do not get correct information
            // about a device until we actually open it (a bug in the NT
            // dos box
            fcb->device = INFO_DEV;
        }
    }
}


int     DfltRecType( ftnfile *fcb ) {
//===================================

    if( fcb->formatted == FORMATTED_IO ) {
        return( REC_TEXT );
    } else {
        if( fcb->accmode == ACCM_DIRECT ) {
            return( REC_FIXED );
        } else {
            return( REC_VARIABLE );
        }
    }
}


int     _FileAttrs( ftnfile *fcb ) {
//==================================

    int     attrs;

    attrs = 0;
    if( fcb->recfm == RECFM_DEFAULT ) {
        attrs |= DfltRecType( fcb );
    } else if( fcb->recfm == RECFM_VARIABLE ) {
        attrs |= REC_VARIABLE;
    } else if( fcb->recfm == RECFM_TEXT ) {
        attrs |= REC_TEXT;
    } else {
        attrs |= REC_FIXED;
    }
    if( fcb->share == SHARE_DENYRW ) {
        attrs |= S_DENYRW;
    } else if( fcb->share == SHARE_DENYWR ) {
        attrs |= S_DENYWR;
    } else if( fcb->share == SHARE_DENYRD ) {
        attrs |= S_DENYRD;
    } else if( fcb->share == SHARE_DENYNO ) {
        attrs |= S_DENYNO;
    }
    if( fcb->cctrl == CC_YES ) {
        attrs |= CARRIAGE_CONTROL;
    }
    if( (fcb->accmode == ACCM_SEQUENTIAL) || (fcb->accmode == ACCM_APPEND) ) {
        if( !IsDevice( fcb ) ) {
            attrs |= TRUNC_ON_WRITE;
        }
    }
    if( fcb->accmode == ACCM_APPEND ) {
        attrs |= APPEND;
    } else if( fcb->accmode == ACCM_DIRECT ) {
        attrs |= SEEK;
    }
    if( fcb->action == ACTION_READ ) {
        attrs |= RDONLY;
    } else if( fcb->action == ACTION_WRITE ) {
        attrs |= WRONLY;
    } else {
        attrs |= RDWR;
    }
    return( attrs );
}

//
// The above "open" routines correspond to WATFOR-77 "ACTION=" specifiers.
//


void    CloseFile( ftnfile *fcb ) {
//=================================

// Close a 'ftnfile'.

    file_handle         fh;

    if( fcb->fileptr == NULL ) return;
    if( fcb->fileptr == FStdIn ) return;
    if( fcb->fileptr == FStdOut ) {
        // check if standard output device redirected to a disk file
        if( ((a_file *)(fcb->fileptr))->attrs & BUFFERED ) {
            FlushBuffer( fcb->fileptr );
            ChkIOErr( fcb );
        }
        return;
    }
    if( fcb->fileptr == FStdErr ) return;
    Closef( fcb->fileptr );
    // save file handle
    fh = fcb->fileptr;
    // set file handle in fcb to NULL - we don't want
    // to get the i/o status from the file handle since
    // it will no longer be valid if the close succeeded
    fcb->fileptr = NULL;
    if( GetIOErr( fcb ) ) {
        // close failed so restore file handle in fcb
        fcb->fileptr = fh;
        IOErr( IO_FILE_PROBLEM );
    }
}


bool    Scrtched( ftnfile *fcb ) {
//================================

// Erase specified file.

    Scratchf( fcb->filename );
    return( Errorf( NULL ) == IO_OK );
}


void    CloseDeleteFile( ftnfile *fcb ) {
//=======================================

// Close and delete 'ftnfile'.

    int win_con = FALSE;

#if defined( __IS_WINDOWED__ )
    if( fcb->fileptr ) {

        win_con = _dwDeleteOnClose( ((a_file *)(fcb->fileptr))->handle );
    }
#endif
    CloseFile( fcb );
    if( win_con ) return;
    if( Scrtched( fcb ) ) return;
    IOErr( IO_FILE_PROBLEM );
}


bool    Errf( ftnfile *fcb ) {
//============================

// Determine if an i/o error exists.

    bool    err;

    err = Errorf( fcb->fileptr );
    if( err == IO_EOF ) {
        SetEOF();
        err = IO_OK;
    }
    return( err != IO_OK );
}


void    FPutBuff( ftnfile *fcb ) {
//================================

// Write a record to a file.

    if( IOCB->flags & IOF_NOCR ) {
        ((a_file *)fcb->fileptr)->attrs |= CC_NOCR;
    }
    if( fcb->flags & FTN_LOGICAL_RECORD ) {
        ((a_file *)fcb->fileptr)->attrs |= LOGICAL_RECORD;
    }
    FPutRec( fcb->fileptr, fcb->buffer, fcb->col );
    if( IOCB->flags & IOF_NOCR ) {
        ((a_file *)fcb->fileptr)->attrs &= ~CC_NOCR;
    }
    ((a_file *)fcb->fileptr)->attrs &= ~( TRUNC_ON_WRITE | LOGICAL_RECORD );
}


void    FGetBuff( ftnfile *fcb ) {
//================================

// Read a record from a file.

    if( _NoRecordOrganization( fcb ) ) {
        fcb->len = FGetRec( fcb->fileptr, fcb->buffer, fcb->len );
    } else {
        fcb->len = FGetRec( fcb->fileptr, fcb->buffer, fcb->bufflen );
    }
}


void    SeekFile( ftnfile *fcb ) {
//================================

// Position file to specified record.

    FSeekRec( fcb->fileptr, fcb->recnum - 1, fcb->bufflen );
}


bool    NoEOF( ftnfile *fcb ) {
//=============================

// Determine if file has an EOF (SERIAL, TERMINAL).

    if( fcb->fileptr == NULL ) return( FALSE );
    return( IsDevice( fcb ) );
}


void    SysClearEOF( ftnfile *fcb ) {
//===================================

// Clear EOF on file with no EOF (SERIAL, TERMINAL).

    IOOk( fcb->fileptr );
#if defined( __DOS__ )
    if( ( fcb->fileptr == FStdIn ) && IsDevice( fcb ) ) {
        // DOS bug: if a read from stdin causes eof, all subsequent reads
        // will also cause eof unless we write to stdout
        write( ((a_file *)FStdOut)->handle, 0, 0 );
    }
#endif
}


bool    SameFile( char *fn1, char *fn2 ) {
//========================================

// Determine if file specifications are for the same file.

#if defined( __UNIX__ )
    return( strcmp( fn1, fn2 ) == 0 );
#else
    return( stricmp( fn1, fn2 ) == 0 );
#endif
}


void    Rewindf( ftnfile *fcb ) {
//===============================

// System dependent rewind.

    if( fcb->fileptr != NULL ) {
        ChkDisk( fcb );
        FRewind( fcb->fileptr );
        ((a_file *)fcb->fileptr)->attrs |= TRUNC_ON_WRITE;
    }
}


void    SysCreateFile( ftnfile *fcb ) {
//=====================================

// Cause the file to exist in the file system.

    fcb->fileptr = Openf( fcb->filename, REC_TEXT | WRONLY );
    if( fcb->fileptr != NULL ) {
        Closef( fcb->fileptr );
        fcb->fileptr = NULL;
        fcb->flags |= FTN_FSEXIST;
    } else {
        IOErr( IO_FILE_PROBLEM );
    }
}


bool    CheckLogicalRecord( ftnfile *fcb ) {
//=========================================

    int         rc;

    rc = FCheckLogical( fcb->fileptr );
    ChkIOErr( fcb );
    return( rc );
}


void    SkipLogicalRecord( ftnfile *fcb ) {
//=========================================

    if( fcb->fileptr != NULL ) {
        FSkipLogical( fcb->fileptr );
        ChkIOErr( fcb );
    }
}


void    BackSpacef( ftnfile *fcb ) {
//==================================

// System dependent file backspace.

    if( fcb->fileptr != NULL ) {
        ChkDisk( fcb );
        FBackspace( fcb->fileptr, fcb->bufflen );
        ((a_file *)fcb->fileptr)->attrs |= TRUNC_ON_WRITE;
    }
}


void    EndFilef( ftnfile *fcb ) {
//================================

// Chop the file off at it current position.

    if( fcb->fileptr != NULL ) {
        FTruncate( fcb->fileptr );
        ((a_file *)fcb->fileptr)->attrs &= ~TRUNC_ON_WRITE;
    }
}


static  void    ChkDisk( ftnfile *fcb ) {
//=======================================

// Make sure that the file is a disk file.

    if( IsDevice( fcb ) ) {
        FSetErr( IO_BAD_OPERATION, fcb->fileptr );
    }
}


void    GetIOErrMsg( ftnfile *fcb, char *buff ) {
//===============================================

// Get i/o error message.

    file_handle fp;

    if( fcb == NULL ) {
        fp = NULL;
    } else {
        fp = fcb->fileptr;
    }
    strcpy( buff, ErrorMsg( fp ) );
}


void    ReportNExist( ftnfile *fcb ) {
//====================================

// Set i/o error condition to "file not found".

    errno = ENOENT;
    FSetSysErr( fcb->fileptr );
}


void    ReportEOF( ftnfile *fcb ) {
//=================================

// Set i/o error condition to "end-of-file".

    FSetEof( fcb->fileptr );
}


void    WaitForEnter( void ) {
//======================

    while( GetStdChar() != '\n' ) {
    }
}
