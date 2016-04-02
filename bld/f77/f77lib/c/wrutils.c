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
* Description:  Run-time utilities for WRITE
*
****************************************************************************/


#include <string.h>
#include <stdlib.h>

#include "ftnstd.h"
#include "rundat.h"
#include "errcod.h"
#include "csetinfo.h"
#include "ifile.h"
#include "charset.h"
#include "rtsysutl.h"
#include "rtutls.h"
#include "ioerr.h"


extern  void            UpdateRecNum(ftnfile *);

extern  char            NormalCtrlSeq[];

/* Forward declarations */
void    SendEOR( void );
void    SendWSLStr( char *str );
void    SendChar( char ch, int rep );
void    Drop( char ch );


void    F_SendData( char *str, uint width ) {
//===========================================
    int         blanks;
    ftnfile     *fcb;

    fcb = IOCB->fileinfo;
    if( fcb->col + width > fcb->bufflen ) {
        SendEOR();
    }
    blanks = width - strlen( str );
    if( blanks < 0 ) {
        SendChar( '*', width );
    } else {
        SendChar( ' ', blanks );
        SendWSLStr( str );
    }
}


void    SendLine( char *str ) {
//=============================

    SendWSLStr( str );
    SendEOR();
}


void    SendInt( intstar4 num ) {
//===============================

    char        num_buff[MAX_INT_SIZE+1];

    ltoa( num, num_buff, 10 );
    SendWSLStr( num_buff );
}


void    SendStr( char PGM *str, uint len ) {
//==========================================

    while( len > 0 ) {
        Drop( *str );
        str++;
        len--;
    }
}


void    SendWSLStr( char *str ) {
//===============================

    while( *str != NULLCHAR ) {
        Drop( *str );
        ++str;
    }
}


void    SendChar( char ch, int rep ) {
//====================================

    while( rep > 0 ) {
        Drop( ch );
        rep--;
    }
}


void    Drop( char ch ) {
//=======================

    ftnfile     *fcb;
    bool        save;
    int         chr_size;

    fcb = IOCB->fileinfo;
    chr_size = 1;
    if( IOCB->flags & DBLE_BYTE_CHAR ) {
        // must be processing second byte of double-byte character
        IOCB->flags &= ~DBLE_BYTE_CHAR;
    } else {
        if( CharSetInfo.is_double_byte_char( ch ) ) {
            chr_size = 2;
            IOCB->flags |= DBLE_BYTE_CHAR;
        }
    }
    if( fcb->col + chr_size > fcb->bufflen ) {
        save = ( IOCB->flags & IOF_NOCR ) != 0;
        IOCB->flags &= ~IOF_NOCR;
        SendEOR();
        if( save ) {
            IOCB->flags |= IOF_NOCR;
        }
        if( ( ( IOCB->flags & IOF_NOFMT ) == 0 ) &&
            ( ( IOCB->set_flags & SET_FMTPTR ) == 0 ) && IsCarriage() ) {
            strcpy( fcb->buffer, NormalCtrlSeq );
            fcb->col = strlen( NormalCtrlSeq );
        }
    }
    fcb->buffer[ fcb->col ] = ch;
    fcb->col++;
}


void    SendEOR( void ) {
//=================

    ftnfile     *fcb;
    int         len;
    bool        ifile;

    fcb = IOCB->fileinfo;
    ifile = fcb->internal != NULL;
    if( ifile ) {
        if( fcb->flags & FTN_EOF ) {
            IOErr( IO_IFULL );
        }
        if( fcb->recnum >= IOCB->elmts ) {
            fcb->flags |= FTN_EOF;
            SendIFBuff( fcb->buffer, fcb->bufflen, fcb->recnum, fcb->internal );
        } else {
            SendIFBuff( fcb->buffer, fcb->bufflen, fcb->recnum, fcb->internal );
            memset( fcb->buffer, ' ', fcb->bufflen );
        }
    } else {
        if( IsFixed() ) {
            fcb->col = fcb->bufflen;
        }
        if( fcb->fileptr != NULL ) {
            FPutBuff( fcb );
            ChkIOErr( fcb );
        }
    }
    // Write to the listing file after we've written to DB_STD_OUTPUT so
    // that if we get an error writing to the listing file, the buffer for
    // DB_STD_OUTPUT will be empty (i.e. when we report the error writing
    // to the listing file we will need to use DB_STD_OUTPUT's buffer).
    // Note that we have to set fcb->col to 0 so that the error message
    // will go at the beginning of the buffer.
    len = fcb->col;
    fcb->col = 0;
    if( ( IOCB->flags & IOF_NOCR ) == 0 ) {
        UpdateRecNum( fcb );
        // eofrecnum used to be updated in ExWrite().
        // We MUST set eofrecnum here in case we abort the WRITE and
        // suicide which will NOT return to ExWrite().
        // If we don't do this here, the next time a WRITE is executed
        // on this unit, we get IO_PAST_EOF error.
        if( !ifile && !NoEOF( fcb ) ) {
            if( fcb->accmode <= ACCM_SEQUENTIAL ) {
                fcb->eofrecnum = fcb->recnum;
            }
        }
        if( !ifile ) {
            memset( fcb->buffer, ' ', fcb->bufflen );
        }
    }
    IOCB->flags &= ~IOF_NOCR;
}


void    IOItemResult( char PGM *src, PTYPE typ ) {
//==============================================

    switch( typ ) {
    case PT_LOG_1:
        *(logstar4 *)(&IORslt) = *(logstar1 *)src;
        break;
    case PT_LOG_4:
        *(logstar4 *)(&IORslt) = *(logstar4 *)src;
        break;
    case PT_INT_1:
        *(intstar4 *)(&IORslt) = *(intstar1 *)src;
        break;
    case PT_INT_2:
        *(intstar4 *)(&IORslt) = *(intstar2 *)src;
        break;
    case PT_INT_4:
        *(intstar4 *)(&IORslt) = *(intstar4 *)src;
        break;
    case PT_REAL_4:
        *(single *)(&IORslt) = *(single *)src;
        break;
    case PT_REAL_8:
        *(double *)(&IORslt) = *(double *)src;
        break;
    case PT_REAL_16:
        *(extended *)(&IORslt) = *(extended *)src;
        break;
    case PT_CPLX_8:
        ((scomplex *)(&IORslt))->imagpart = ((scomplex *)src)->imagpart;
        ((scomplex *)(&IORslt))->realpart = ((scomplex *)src)->realpart;
        break;
    case PT_CPLX_16:
        ((dcomplex *)(&IORslt))->imagpart = ((dcomplex *)src)->imagpart;
        ((dcomplex *)(&IORslt))->realpart = ((dcomplex *)src)->realpart;
        break;
    case PT_CPLX_32:
        ((xcomplex *)(&IORslt))->imagpart = ((xcomplex *)src)->imagpart;
        ((xcomplex *)(&IORslt))->realpart = ((xcomplex *)src)->realpart;
        break;
    }
}
