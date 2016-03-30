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
* Description:  File I/O error handling routines
*
****************************************************************************/

#include "ftnstd.h"
#include "ftextfun.h"
#include "posio.h"
#include "poserr.h"

#include <string.h>
#include <errno.h>

static  byte    Stat = { IO_OK };

static  char    * const __FAR ErrMsgs[] = {
    NULL,
    "disk full",
    "invalid record",
    "out of memory",
    "bad operation",
    "end of file",
    "record truncated",
    NULL
};


int     Errorf( b_file *io ) {
// Check for i/o error condition.

    int         err;

    if( io == NULL ) {
        err = Stat;
    } else {
        err = io->stat;
    }
    if( err == IO_EOR ) {
        err = IO_OK;
    }
    return( err );
}


bool    EOFile( b_file *io ) {
// Check if end-of-file.

    if( io == NULL ) {
        return( Stat == IO_EOF );
    } else {
        return( io->stat == IO_EOF );
    }
}


char    *ErrorMsg( b_file *io ) {
// Get i/o error message.

    int         err;

    if( io == NULL ) {
        err = Stat;
    } else {
        err = io->stat;
    }
    if( err == IO_SYS_ERROR ) {
        return( strerror( errno ) );
    } else {
        return( ErrMsgs[ err ] );
    }
}


void    FSetTrunc( b_file *io ) {
// Set "truncated" condition.

    Stat = IO_EOR;
    io->stat = IO_EOR;
}


void    FSetSysErr( b_file *io ) {
// Set system i/o error condition.

    if( io != NULL ) {
        io->stat = IO_SYS_ERROR;
    }
    Stat = IO_SYS_ERROR;
}


void    FSetErr( int error, b_file *io ) {
// Set i/o error condition.

    if( io != NULL ) {
        io->stat = error;
    }
    Stat = error;
}

void    FSetEof( b_file *io ) {
// Set end-of-file condition.

    if( io != NULL ) {
        io->stat = IO_EOF;
    }
    Stat = IO_EOF;
}


void    IOOk( b_file *io ) {
// Clear i/o error conditions.

    if( io != NULL ) {
        io->stat = IO_OK;
    }
    Stat = IO_OK;
}
