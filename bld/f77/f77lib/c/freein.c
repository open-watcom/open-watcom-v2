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
* Description:  Free-format input
*
****************************************************************************/

#include "ftnstd.h"
#include "ftextfun.h"
#include "rundat.h"
#include "errcod.h"
#include "intcnv.h"
#include "fltcnv.h"
#include "target.h"
#include "pgmacc.h"

#include <ctype.h>

/* Forward declarations */
static  void    RptNum( void );
static  void    InNumber( void );
static  void    InString( void );
static  void    GetInt( intstar4 *result );
static  void    GetFloat( extended *result, int prec );
static  void    GetString( void );
static  void    InCplx( void );
static  void    InLog( void );
static  void    FreeIOErr( uint err );

void    BumpComma( void );
void    Blanks( void );
void    CheckEor( void );


static  char    *GetDelim( char *start, char *buff_end ) {
//========================================================

    for(;;) {
        if( start >= buff_end ) break;
        switch( *start ) {
        case ' ':
        case '\t':
        case ',':
        case '/':
        case ')':
            return( start );
        }
        start++;
    }
    return( start );
}


signed_32       GetNum( void ) {
//=========================

    ftnfile     *fcb;
    char        ch;
    signed_32   value;
    bool        minus;

    fcb = IOCB->fileinfo;
    ch = fcb->buffer[ fcb->col ];
    minus = FALSE;
    if( ch == '+' ) {
        fcb->col++;
    } else if( ch == '-' ) {
        minus = TRUE;
        fcb->col++;
    }
    value = 0;
    for(;;) {
        ch = fcb->buffer[ fcb->col ];
        if( isdigit( ch ) == 0 ) break;
        value = value*10 + ( ch - '0' );
        fcb->col++;
    }
    if( minus ) {
        value = -value;
    }
    return( value );
}


static  void    FreeIOType( void ) {
//============================

    if( !(IOCB->flags & NML_DIRECTED) ) {
        ArrayIOType();
        return;
    }
    IOCB->typ = IOTypeRtn();
}


void    DoFreeIn( void ) {
//==================

    ftnfile     *fcb;
    char        ch;

    fcb = IOCB->fileinfo;
    FreeIOType();
    while( IOCB->typ != PT_NOTYPE ) {
        CheckEor();
        Blanks();
        RptNum();
        if( fcb->col >= fcb->len ) {
            while( IOCB->rptnum-- > 0 ) {
                FreeIOType();
                if( IOCB->typ == PT_NOTYPE ) break;
            }
        } else {
            ch = fcb->buffer[ fcb->col ];
            if( ch == '/' ) break;
            switch( ch ) {
            case ',':
            case ' ':
                for(;;) {
                    FreeIOType();
                    if( IOCB->typ == PT_NOTYPE ) break;
                    if( IOCB->rptnum-- <= 1 ) break;
                }
                fcb->col++;
                break;
            case '\'':
                InString();
                BumpComma();
                break;
            case '(':
                InCplx();
                BumpComma();
                break;
            case 't':
            case 'T':
            case 'f':
            case 'F':
                InLog();
                BumpComma();
                break;
            case '-':
            case '+':
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                InNumber();
                BumpComma();
                break;
            case '.':
                ch = toupper( fcb->buffer[ fcb->col + 1 ] );
                if( ( ch != 'T' ) && ( ch != 'F' ) ) {
                    InNumber();
                } else {
                    fcb->col++;
                    InLog();
                }
                BumpComma();
                break;
            default:
                FreeIOErr( IO_BAD_CHAR );
                break;
            }
        }
    }
}


void    FreeIn( void ) {
//================

    NextRec();
    DoFreeIn();
}


void    BumpComma( void ) {
//===================

    ftnfile     *fcb;

    fcb = IOCB->fileinfo;
    Blanks();
    if( fcb->buffer[ fcb->col ] == ',' ) {
        fcb->col++;
    }
}


void    Blanks( void ) {
//================

    ftnfile     *fcb;
    char        *buff;

    fcb = IOCB->fileinfo;
    buff = &fcb->buffer[ fcb->col ];
    fcb->col += JmpBlanks( buff ) - buff;
}


void    CheckEor( void ) {
//==================

    ftnfile     *fcb;

    fcb = IOCB->fileinfo;
    while( fcb->col >= fcb->len ) {
        NextRec();
        if( IOCB->typ != PT_CHAR ) {
            Blanks();
        }
    }
}


static  void    RptNum( void ) {
//========================

    ftnfile     *fcb;
    int col;
    signed_32   num;

    fcb = IOCB->fileinfo;
    col = fcb->col;
    num = GetNum();
    if( fcb->buffer[ fcb->col ] == '*' ) {
        if( num > 0 ) {
            fcb->col++;
        } else {
            IOErr( IO_BAD_CHAR );
        }
    } else {
        fcb->col = col;
        num = 0;
    }
    IOCB->rptnum = num;
}


static  void    InNumber( void ) {
//==========================

    extended    value;
    intstar4    intval;
    int         col;

    col = IOCB->fileinfo->col; // save position in case of repeat specifier
    for(;;) {
        IOCB->fileinfo->col = col;
        if( IOCB->typ >= PT_REAL_4 ) {
            GetFloat( &value, ( IOCB->typ - PT_REAL_4 ) );
            switch( IOCB->typ ) {
            case PT_REAL_4:
                *(single PGM *)(IORslt.pgm_ptr) = value;
                break;
            case PT_REAL_8:
                *(double PGM *)(IORslt.pgm_ptr) = value;
                break;
            case PT_REAL_16:
                *(extended PGM *)(IORslt.pgm_ptr) = value;
                break;
            default:
                IOErr( IO_FREE_MISMATCH );
                break;
            }
        } else {
            GetInt( &intval );
            switch( IOCB->typ ) {
            case PT_INT_1:
                *(intstar1 PGM *)(IORslt.pgm_ptr) = intval;
                break;
            case PT_INT_2:
                *(intstar2 PGM *)(IORslt.pgm_ptr) = intval;
                break;
            case PT_INT_4:
                *(intstar4 PGM *)(IORslt.pgm_ptr) = intval;
                break;
            default:
                IOErr( IO_FREE_MISMATCH );
                break;
            }
        }
        FreeIOType();
        if( ( IOCB->rptnum-- <= 1 ) || ( IOCB->typ == PT_NOTYPE ) ) break;
    }
}


static  void    InLog( void ) {
//=======================

    ftnfile     *fcb;
    logstar4    value;
    char        *chptr;

    fcb = IOCB->fileinfo;
    chptr = &fcb->buffer[ fcb->col ];
    if( toupper( *chptr ) == 'T' ) {
        value = _LogValue( TRUE );
    } else {
        value = _LogValue( FALSE );
    }
    for(;;) {
        chptr++;
        fcb->col++;
        if( fcb->col == fcb->len ) break;
        switch( *chptr ) {
        case ' ':
        case '\t':
        case '/':
        case ',':
            goto big_break;
        }
    }
big_break:
    for(;;) {
        switch( IOCB->typ ) {
        case PT_LOG_1:
            *(logstar1 PGM *)(IORslt.pgm_ptr) = value;
            break;
        case PT_LOG_4:
            *(logstar4 PGM *)(IORslt.pgm_ptr) = value;
            break;
        default:
            IOErr( IO_FREE_MISMATCH );
            break;
        }
        FreeIOType();
        if( ( IOCB->rptnum-- <= 1 ) || ( IOCB->typ == PT_NOTYPE ) ) break;
    }
}


static  void    InCplx( void ) {
//========================

    ftnfile     *fcb;
    uint        rpt;
    xcomplex    value;

    fcb = IOCB->fileinfo;
    fcb->col++;
    Blanks();
    GetFloat( &value.realpart, ( IOCB->typ - PT_CPLX_8 ) );
    Blanks();
    CheckEor();
    if( fcb->buffer[ fcb->col ] != ',' ) {
        IOErr( IO_BAD_CHAR );
    }
    fcb->col++;
    Blanks();
    CheckEor();
    GetFloat( &value.imagpart, ( IOCB->typ - PT_CPLX_8 ) );
    Blanks();
    if( fcb->buffer[ fcb->col ] != ')' ) {
        IOErr( IO_BAD_CHAR );
    }
    fcb->col++;
    rpt = IOCB->rptnum;
    for(;;) {
        switch( IOCB->typ ) {
        case PT_CPLX_8:
            ((scomplex PGM *)(IORslt.pgm_ptr))->realpart = value.realpart;
            ((scomplex PGM *)(IORslt.pgm_ptr))->imagpart = value.imagpart;
            break;
        case PT_CPLX_16:
            ((dcomplex PGM *)(IORslt.pgm_ptr))->realpart = value.realpart;
            ((dcomplex PGM *)(IORslt.pgm_ptr))->imagpart = value.imagpart;
            break;
        case PT_CPLX_32:
            ((xcomplex PGM *)(IORslt.pgm_ptr))->realpart = value.realpart;
            ((xcomplex PGM *)(IORslt.pgm_ptr))->imagpart = value.imagpart;
            break;
        default:
            IOErr( IO_FREE_MISMATCH );
            break;
        }
        FreeIOType();
        if( ( rpt-- <= 1 ) || ( IOCB->typ == PT_NOTYPE ) ) break;
    }
}


static  void    InString( void ) {
//===========================

    int         save_col;

    if( IOCB->rptnum == 0 ) {
        if( IOCB->typ != PT_CHAR ) {
            IOErr( IO_FREE_MISMATCH );
        }
        GetString();
        FreeIOType();
    } else {
        save_col = IOCB->fileinfo->col;
        for(;;) {
            if( IOCB->typ != PT_CHAR ) {
                IOErr( IO_FREE_MISMATCH );
            }
            IOCB->fileinfo->col = save_col;
            GetString();
            FreeIOType();
            if( ( IOCB->rptnum-- <= 1 ) || ( IOCB->typ == PT_NOTYPE ) ) break;
        }
    }
}


static  void    GetString( void ) {
//===========================

    ftnfile     *fcb;
    uint        len;
    uint        count;
    char        PGM *ptr;

    len = IORslt.string.len;
    ptr = IORslt.string.strptr;
    fcb = IOCB->fileinfo;
    count = 0;
    fcb->col++;
    while( count < len ) {
        if( fcb->col >= fcb->len ) {
            CheckEor();
        }
        if( fcb->buffer[ fcb->col ] == '\'' ) {
            fcb->col++;
            if( fcb->buffer[ fcb->col ] != '\'' ) break;
        }
        count++;
        *ptr = fcb->buffer[ fcb->col ];
        ptr = ptr + sizeof( char );
        fcb->col++;
    }
    if( count == len ) {
        for(;;) {
            if( fcb->buffer[ fcb->col ] == '\'' ) {
                fcb->col++;
                if( fcb->buffer[ fcb->col ] != '\'' ) break;
            }
            if( fcb->col >= fcb->len ) {
                CheckEor();
            }
            fcb->col++;
        }
    }
    pgm_memset( ptr, ' ', len - count );
}


static  void    GetInt( intstar4 *result ) {
//==========================================

    ftnfile     *fcb;
    char        *start;
    int         len;
    int         status;

    fcb = IOCB->fileinfo;
    start = &fcb->buffer[ fcb->col ];
    len  = GetDelim( start, &fcb->buffer[ fcb->len ] ) - start;
    status = FmtS2I( start, len, FALSE, result, FALSE, NULL );
    if( status == INT_OK ) {
        fcb->col += len;
    } else if( status == INT_INVALID ) {
        IOErr( IO_BAD_CHAR );
    } else {
        IOErr( IO_IOVERFLOW );
    }
}


static  void    GetFloat( extended *result, int prec ) {
//======================================================

    ftnfile     *fcb;
    char        *start;
    int         len;
    int         status;

    fcb = IOCB->fileinfo;
    start = &fcb->buffer[ fcb->col ];
    len  = GetDelim( start, &fcb->buffer[ fcb->len ] ) - start;
    status = FmtS2F( start, len, 0, FALSE, 0, prec, result, FALSE, NULL, FALSE );
    if( status == FLT_OK ) {
        fcb->col += len;
    } else {
        if( status == FLT_INVALID ) {
            IOErr( IO_BAD_CHAR );
        } else { // FLT_RANGE_EXCEEDED
            IOErr( IO_FRANGE_EXCEEDED );
        }
    }
}


static  void    FreeIOErr( uint err ) {
//=====================================

// Report error during list-directed or NAMELIST-directed i/o.

    if( IOCB->flags & NML_DIRECTED ) {
        IOCB->flags |= NML_CONTINUE;
        RTSuicide();
    }
    IOErr( err );
}
