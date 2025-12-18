/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Runtime formatted I/O processing routines
*
****************************************************************************/


#include "ftnstd.h"
#include "rundat.h"
#include "errcod.h"
#include "fmtdef.h"
#include "format.h"
#include "iotype.h"
#include "rterr.h"
#include "ioerr.h"
#include "wrutils.h"
#include "rfmtutil.h"
#include "rfmtexec.h"
#include "arrayio.h"


/* Forward declarations */
static  void    ExecCode( void );

static void     R_FEH( uint dummy1, char dummy2 )
//===============================================
{
    ftnfile     *fcb;
    uint        len;
    char        PGM *str;

    /* unused parameters */ (void)dummy1; (void)dummy2;

    fcb = IOCB->fileinfo;
    if( IOCB->flags & IOF_OUTPT ) {
        len = IOCB->u.fmtptr->fmtstring.fld1;
        if( fcb->col + len > fcb->bufflen ) {
            IOErr( IO_BUFF_LEN );
            // never return
        }
        str = IOCB->u.fmtptr->fmtstring.str;
        SendStr( str, len );
    } else {
        RTErr( FM_NOT_INP );
    }
    R_ChkRecLen();
    IOCB->u.ptr += sizeof( fmt4 ) + len;
}


static  void    R_FESlash( uint rep_spec, char dummy2 )
//=====================================================
{
    /* unused parameters */ (void)dummy2;

    for( ;; ) {
        R_NewRec();
        if( --rep_spec == 0 ) {
            break;
        }
    }
    IOCB->u.ptr += sizeof( fmt );
}


static  void    R_FEX( uint dummy1 , char dummy2 )
//================================================
{
    ftnfile     *fcb;

    /* unused parameters */ (void)dummy1; (void)dummy2;

    fcb = IOCB->fileinfo;
    fcb->col += IOCB->u.fmtptr->fmt4.fld1;
    IOCB->u.ptr += sizeof( fmt4 );
}


static  void    FmtIOType( void )
//===============================
{
    if( IOCB->flags & IOF_FMTREALPART ) {
        IOCB->flags &= ~IOF_FMTREALPART;
    } else {
        ArrayIOType();
        if( (IOCB->ptyp >= FPT_CPLX_8)
          && (IOCB->ptyp <= FPT_CPLX_32) ) {
            IOCB->flags |= IOF_FMTREALPART;
        }
    }
}


static  void    FmtPrepOp( void )
//===============================
{
    ftnfile     *fcb;
    byte        width;

    fcb = IOCB->fileinfo;
    width = IOCB->u.fmtptr->fmt1.fld1;
    if( width + fcb->col > fcb->bufflen ) {
        IOErr( IO_BUFF_LEN );
        // never return
    }
}


static  void    R_FEI( uint rep, char dummy2 )
//============================================
{
    /* unused parameters */ (void)dummy2;

    for( ;; ) {
        if( IOCB->ptyp == FPT_NOTYPE ) {
            IOCB->flags |= IOF_FMTDONE;
        }
        if( IOCB->ptyp == FPT_NOTYPE )
            break;
        FmtPrepOp();
        R_ChkIType();
        if( IOCB->flags & IOF_OUTPT ) {
            R_FOInt();
            R_ChkRecLen();
        } else {
            R_FIInt();
        }
        FmtIOType();
        if( --rep == 0 ) {
            break;
        }
    }
    IOCB->flags |= IOF_FMTREP;
    IOCB->u.ptr += sizeof( fmt2 );
}


static  void    R_FEColon( uint dummy1 , char dummy2 )
//====================================================
{
    /* unused parameters */ (void)dummy1; (void)dummy2;

    if( IOCB->ptyp == FPT_NOTYPE ) {
        IOCB->flags |= IOF_FMTDONE;
    }
    IOCB->u.ptr += sizeof( fmt );
}


static  void    R_FEA( uint rep , char dummy2 )
//=============================================
{
    /* unused parameters */ (void)dummy2;

    for( ;; ) {
        if( IOCB->ptyp == FPT_NOTYPE ) {
            IOCB->flags |= IOF_FMTDONE;
        }
        if( IOCB->ptyp == FPT_NOTYPE )
            break;
        if( IOCB->flags & IOF_OUTPT ) {
            R_FOStr();
            R_ChkRecLen();
        } else {
            R_FIStr();
        }
        FmtIOType();
        if( --rep == 0 ) {
            break;
        }
    }
    IOCB->flags |= IOF_FMTREP;
    IOCB->u.ptr += sizeof( fmt4 );
}


static  void    R_FET( uint dummy1 , char dummy2 )
//================================================
{
    ftnfile     *fcb;

    /* unused parameters */ (void)dummy1; (void)dummy2;

    fcb = IOCB->fileinfo;
    fcb->col = IOCB->u.fmtptr->fmt4.fld1 - 1;
    IOCB->u.ptr += sizeof( fmt4 );
}


static  void    R_FETL( uint dummy1 , char dummy2 )
//=================================================
{
    ftnfile     *fcb;
    uint        offset;

    /* unused parameters */ (void)dummy1; (void)dummy2;

    fcb = IOCB->fileinfo;
    offset = IOCB->u.fmtptr->fmt4.fld1;
    if( fcb->col < offset ) {
        fcb->col = 0;
    } else {
        fcb->col -= offset;
    }
    IOCB->u.ptr += sizeof( fmt4 );
}


static  void    R_FETR( uint dummy1 , char dummy2 )
//=================================================
{
    ftnfile     *fcb;

    /* unused parameters */ (void)dummy1; (void)dummy2;

    fcb = IOCB->fileinfo;
    fcb->col += IOCB->u.fmtptr->fmt4.fld1;
    IOCB->u.ptr += sizeof( fmt4 );
}


static  void    R_FES( uint dummy1 , char dummy2 )
//================================================
{
    /* unused parameters */ (void)dummy1; (void)dummy2;

    IOCB->flags &= ~IOF_PLUS;
    IOCB->u.ptr += sizeof( fmt );
}


static  void    R_FESP( uint dummy1 , char dummy2 )
//=================================================
{
    /* unused parameters */ (void)dummy1; (void)dummy2;

    IOCB->flags |= IOF_PLUS;
    IOCB->u.ptr += sizeof( fmt );
}


static  void    R_FESS( uint dummy1 , char dummy2 )
//=================================================
{
    /* unused parameters */ (void)dummy1; (void)dummy2;

    IOCB->flags &= ~IOF_PLUS;
    IOCB->u.ptr += sizeof( fmt );
}


static  void    R_FEBN( uint dummy1 , char dummy2 )
//=================================================
{
    /* unused parameters */ (void)dummy1; (void)dummy2;

    IOCB->fileinfo->blanks = BLANK_NULL;
    IOCB->u.ptr += sizeof( fmt );
}


static  void    R_FEBZ( uint dummy1 , char dummy2 )
//=================================================
{
    /* unused parameters */ (void)dummy1; (void)dummy2;

    IOCB->fileinfo->blanks = BLANK_ZERO;
    IOCB->u.ptr += sizeof( fmt );
}


static  void    R_FEL( uint rep, char dummy2 )
//============================================
{
    /* unused parameters */ (void)dummy2;

    for( ;; ) {
        if( IOCB->ptyp == FPT_NOTYPE ) {
            IOCB->flags |= IOF_FMTDONE;
        }
        if( IOCB->ptyp == FPT_NOTYPE )
            break;
        FmtPrepOp();
        R_ChkType( FPT_LOG_1, FPT_LOG_4 );
        if( IOCB->flags & IOF_OUTPT ) {
            R_FOLog();
            R_ChkRecLen();
        } else {
            R_FILog();
        }
        FmtIOType();
        if( --rep == 0 ) {
            break;
        }
    }
    IOCB->flags |= IOF_FMTREP;
    IOCB->u.ptr += sizeof( fmt1 );
}


static  void    R_FEF( uint rep, char dummy2 )
//============================================
{
    /* unused parameters */ (void)dummy2;

    for( ;; ) {
        if( IOCB->ptyp == FPT_NOTYPE ) {
            IOCB->flags |= IOF_FMTDONE;
        }
        if( IOCB->ptyp == FPT_NOTYPE )
            break;
        FmtPrepOp();
        R_ChkFType();
        if( IOCB->flags & IOF_OUTPT ) {
            R_FOF();
            R_ChkRecLen();
        } else {
            R_FIFloat();
        }
        FmtIOType();
        if( --rep == 0 ) {
            break;
        }
    }
    IOCB->flags |= IOF_FMTREP;
    IOCB->u.ptr += sizeof( fmt2 );
}


static  void    R_FED( uint rep, char dummy2 )
//============================================
{
    /* unused parameters */ (void)dummy2;

    for( ;; ) {
        if( IOCB->ptyp == FPT_NOTYPE ) {
            IOCB->flags |= IOF_FMTDONE;
        }
        if( IOCB->ptyp == FPT_NOTYPE )
            break;
        FmtPrepOp();
        R_ChkFType();
        if( IOCB->flags & IOF_OUTPT ) {
            R_FOE( 0, 'D' );
            R_ChkRecLen();
        } else {
            R_FIFloat();
        }
        FmtIOType();
        if( --rep == 0 ) {
            break;
        }
    }
    IOCB->flags |= IOF_FMTREP;
    IOCB->u.ptr += sizeof( fmt2 );
}


static  void    R_FEQ( uint rep, char dummy2 )
//============================================
{
    /* unused parameters */ (void)dummy2;

    for( ;; ) {
        if( IOCB->ptyp == FPT_NOTYPE ) {
            IOCB->flags |= IOF_FMTDONE;
        }
        if( IOCB->ptyp == FPT_NOTYPE )
            break;
        FmtPrepOp();
        R_ChkFType();
        if( IOCB->flags & IOF_OUTPT ) {
            R_FOE( 0, 'Q' );
            R_ChkRecLen();
        } else {
            R_FIFloat();
        }
        FmtIOType();
        if( --rep == 0 ) {
            break;
        }
    }
    IOCB->flags |= IOF_FMTREP;
    IOCB->u.ptr += sizeof( fmt2 );
}


static  void    R_FEE( uint rep, char ch )
//========================================
{
    for( ;; ) {
        if( IOCB->ptyp == FPT_NOTYPE ) {
            IOCB->flags |= IOF_FMTDONE;
        }
        if( IOCB->ptyp == FPT_NOTYPE )
            break;
        FmtPrepOp();
        R_ChkFType();
        if( IOCB->flags & IOF_OUTPT ) {
            R_FOE( IOCB->u.fmtptr->fmt3.fld3, ch );
            R_ChkRecLen();
        } else {
            R_FIFloat();
        }
        FmtIOType();
        if( --rep == 0 ) {
            break;
        }
    }
    IOCB->flags |= IOF_FMTREP;
    IOCB->u.ptr += sizeof( fmt3 );
}


static  void    R_FEG( uint rep, char dummy2 )
//============================================
{
    /* unused parameters */ (void)dummy2;

    for( ;; ) {
        if( IOCB->ptyp == FPT_NOTYPE ) {
            IOCB->flags |= IOF_FMTDONE;
        }
        if( IOCB->ptyp == FPT_NOTYPE )
            break;
        FmtPrepOp();
        R_ChkType( FPT_LOG_1, FPT_CPLX_32 );
        if( IOCB->flags & IOF_OUTPT ) {
            R_FOG();
            R_ChkRecLen();
        } else {
            R_FIFloat();
        }
        FmtIOType();
        if( --rep == 0 ) {
            break;
        }
    }
    IOCB->flags |= IOF_FMTREP;
    IOCB->u.ptr += sizeof( fmt3 );
}


static  void    R_FEP( uint dummy1 , char dummy2 )
//================================================
{
    /* unused parameters */ (void)dummy1; (void)dummy2;

    IOCB->scale = IOCB->u.fmtptr->fmt4.fld1;
    IOCB->u.ptr += sizeof( fmt4 );
}


static  void    R_FELParen( uint rep_spec, char dummy2 )
//======================================================
{
    char    PGM *revert;

    /* unused parameters */ (void)dummy2;

    IOCB->u.ptr += sizeof( fmt );
    revert = IOCB->u.ptr;
    for( ;; ) {
        IOCB->u.ptr = revert;
        for( ;; ) {
            if( (IOCB->u.fmtptr->fmt.code & ~EXTEND_FORMAT) == RP_FORMAT )
                break;
            if( IOCB->flags & IOF_FMTDONE )
                break;
            ExecCode();
        }
        if( IOCB->flags & IOF_FMTDONE )
            break;
        if( --rep_spec == 0 ) {
            break;
        }
    }
    IOCB->u.ptr += sizeof( fmt );
}


static  void    R_FERParen( uint dummy1 , char dummy2 )
//=====================================================
{
    /* unused parameters */ (void)dummy1; (void)dummy2;

    IOCB->u.ptr += sizeof( fmt );
}


static  void    R_FEnd( uint dummy1 , char dummy2 )
//=================================================
{
    int         revert;

    /* unused parameters */ (void)dummy1; (void)dummy2;

    if( IOCB->ptyp != FPT_NOTYPE ) {
        if( ( IOCB->flags & IOF_FMTREP ) == 0 ) {
            RTErr( FM_REP );
        }
        IOCB->flags &= ~IOF_FMTREP;
        revert = IOCB->u.fmtptr->fmt4.fld1;
        IOCB->u.ptr += sizeof( fmt ) - revert;
        R_NewRec();
    } else {
        IOCB->flags |= IOF_FMTDONE;
    }
}


static  void    R_FEZ( uint rep, char dummy2 )
//============================================
{
    /* unused parameters */ (void)dummy2;

    for( ;; ) {
        if( IOCB->ptyp == FPT_NOTYPE ) {
            IOCB->flags |= IOF_FMTDONE;
        }
        if( IOCB->ptyp == FPT_NOTYPE )
            break;
        FmtPrepOp();
        if( IOCB->flags & IOF_OUTPT ) {
            R_FOHex();
            R_ChkRecLen();
        } else {
            R_FIHex();
        }
        FmtIOType();
        if( --rep == 0 ) {
            break;
        }
    }
    IOCB->flags |= IOF_FMTREP;
    IOCB->u.ptr += sizeof( fmt1 );
}


static  void    R_FEM( uint dummy1 , char dummy2 )
//================================================
{
    /* unused parameters */ (void)dummy1; (void)dummy2;

    if( IOCB->flags & IOF_OUTPT ) {
        IOCB->flags |= IOF_NOCR;
    }
    IOCB->u.ptr += sizeof( fmt );
}


static  const void (* const __FAR FmtExec[])( uint , char ) = {
    &R_FEA,
    &R_FEBN,
    &R_FEBZ,
    &R_FED,
    &R_FEE,
    &R_FEE,
    &R_FEF,
    &R_FEG,
    &R_FEH,
    &R_FEI,
    &R_FEL,
    &R_FEP,
    &R_FES,
    &R_FESP,
    &R_FESS,
    &R_FET,
    &R_FETL,
    &R_FETR,
    &R_FEX,
    &R_FEColon,
    &R_FELParen,
    &R_FERParen,
    &R_FESlash,
    &R_FEM,
    &R_FEZ,
    &R_FEnd,
    NULL,
    &R_FEE,
    &R_FEQ
};


static  void    ExecCode( void )
//==============================
{
    byte        code;
    uint        repeat;
    char        ch;

    if( (IOCB->u.fmtptr->fmt4.code & ~EXTEND_FORMAT) == REP_FORMAT ) {
        repeat = IOCB->u.fmtptr->fmt4.fld1;
        IOCB->u.ptr += sizeof( fmt4 );
    } else {
        repeat = 1;
    }
    IOCB->flags &= ~IOF_EXTEND_FORMAT;
    code = IOCB->u.fmtptr->fmt.code;
    if( code & EXTEND_FORMAT ) {
        IOCB->flags |= IOF_EXTEND_FORMAT;
        code &= ~EXTEND_FORMAT;
    }
    if( code == E_FORMAT ) {
        ch = 'E';
    } else if( code == ED_FORMAT ) {
        ch = 'D';
    } else if( code == EQ_FORMAT ) {
        ch = 'Q';
    }
    FmtExec[ code ]( repeat, ch );
}


static  void    ExecInit( void )
//==============================
{
    ftnfile     *fcb;

    fcb = IOCB->fileinfo;
    IOCB->flags &= ~IOF_FMTDONE;
    IOCB->flags &= ~IOF_FMTREP;
    IOCB->flags &= ~IOF_FMTREALPART;
    IOCB->fmtlen = fcb->col;
    IOCB->scale = 0;
    FmtIOType();
    if( ( IOCB->flags & IOF_OUTPT ) == 0 ) {
        R_NewRec();
    }
}


void    R_FExec( void )
//=====================
{
    ExecInit();
    for( ;; ) {
        ExecCode();
        if( IOCB->flags & IOF_FMTDONE ) {
            break;
        }
    }
    if( IOCB->flags & IOF_OUTPT ) {
        R_NewRec();
    }
}
