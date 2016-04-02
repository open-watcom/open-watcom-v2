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
* Description:  Run-time formatted I/O utility routines
*
****************************************************************************/


#include "ftnstd.h"
#include "ftextfun.h"
#include "rundat.h"
#include "errcod.h"
#include "fmtdef.h"
#include "intcnv.h"
#include "target.h"
#include "pgmacc.h"
#include "undefrtn.h"
#include "fltcnv.h"
#include "fmath.h"
#include "rmemmgr.h"
#include "runmain.h"
#include "hexcnv.h"
#include "rtutls.h"
#include "ioerr.h"
#include "cctrl.h"
#include "wrutils.h"

#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <string.h>


/* Forward declarations */
static  void    HexFlip( char *src, int len );
static  void    OutInt( uint width, uint min );
void    ChkBuffLen( uint width );
void    R_FOHex( void );
    
#if defined( _M_IX86 ) || defined( __AXP__ ) || defined( __PPC__ )

extern  const double __FAR      P1d100;
extern  const double __FAR      P1d_99;

typedef union bit_extended {
    byte            pattern[16];
    extended        value;
} bit_extended;

typedef union bit_double {
    byte            pattern[8];
    double          value;
} bit_double;

typedef union bit_float {
    byte            pattern[4];
    float           value;
} bit_float;

static  const bit_extended __FAR XInfinity =
                            {   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                                0xf0,0x7f,0x00,0x00,0x00,0x00,0x00,0x00 };
static  const bit_double __FAR  DInfinity =
                            { 0x00,0x00,0x00,0x00,0x00,0x00,0xf0,0x7f };
static  const bit_float __FAR   SInfinity = { 0x00,0x00,0x80,0x7f };

#endif

static  const byte __FAR        DataSize[] = {
            0,
            sizeof( logstar1 ),
            sizeof( logstar4 ),
            sizeof( intstar1 ),
            sizeof( intstar2 ),
            sizeof( intstar4 ),
            sizeof( single ),
            sizeof( double ),
            sizeof( extended ),
            sizeof( single ),
            sizeof( double ),
            sizeof( extended ) };


void    R_ChkType( PTYPE lower, PTYPE upper ) {
//===========================================

    if( ( IOCB->typ < lower ) || ( IOCB->typ > upper ) ) {
        IOErr( IO_FMT_MISMATCH );
    }
}


void    R_NewRec( void ) {
//==================

    if( IOCB->flags & IOF_OUTPT ) {
        IOCB->fileinfo->col = IOCB->fmtlen;
        CheckCCtrl();
        SendEOR();
        IOCB->fmtlen = IOCB->fileinfo->col;
    } else {
        NextRec();
    }
}


void    R_ChkIType( void ) {
//====================

// Check if type can be formatted using I format.

    if( ( IOCB->typ == PT_REAL_4 ) && ( IOCB->flags & IOF_EXTEND_FORMAT ) ) {
        // allow REAL variable containing integer data to be
        // formatted using I edit descriptor
        IOCB->typ = PT_INT_4;
    }
    R_ChkType( PT_INT_1, PT_INT_4 );
}


void    R_ChkFType( void ) {
//====================

// Check if type can be formatted using F, E, or D format.

    if( ( IOCB->typ == PT_INT_4 ) && ( IOCB->flags & IOF_EXTEND_FORMAT ) ) {
        // allow INTEGER variable containing floating-point data to be
        // formatted using F, E, or D edit descriptors
        IOCB->typ = PT_REAL_4;
    }
    R_ChkType( PT_REAL_4, PT_CPLX_32 );
}


void    R_ChkRecLen( void ) {
//=====================

    ftnfile     *fcb;

    fcb = IOCB->fileinfo;
    if( fcb->col > IOCB->fmtlen ) {
        IOCB->fmtlen = fcb->col;
    }
}


static uint GetLen( void ) {
//==========================

    uint        len;

    if( IOCB->typ == PT_CHAR ) {
        len = IORslt.string.len;
    } else {
        len = DataSize[ IOCB->typ ];
    }
    return( len );
}


static void FOString( uint width ) {
//==================================

    ftnfile     *fcb;
    uint        length;

    fcb = IOCB->fileinfo;
    if( IOCB->typ != PT_CHAR ) {
        length = GetLen();
    } else {
        length = IORslt.string.len;
    }
    if( width == 0 ) {
        width = length;
    }
    if( fcb->col + width > fcb->bufflen ) {
        IOErr( IO_BUFF_LEN );
    }
    if( width > length ) {
        SendChar( ' ', width - length );
        width = length;
    }
    if( IOCB->typ == PT_CHAR ) {
        SendStrRtn( IORslt.string.strptr, width );
    } else {
        SendStrRtn( (char *)&IORslt, width );
    }
}


void    R_FOStr( void ) {
//=================

    FOString( IOCB->fmtptr->fmt4.fld1 );
}


void    R_FIStr( void ) {
//=================

    uint        width;
    byte        blanks;
    uint        length;
    ftnfile     *fcb;
    char        *src;
    char        PGM *ptr;

    fcb   = IOCB->fileinfo;
    width = IOCB->fmtptr->fmt4.fld1;
    if( IOCB->typ != PT_CHAR ) {
        ptr = IORslt.pgm_ptr;
        length = GetLen();
    } else {
        ptr = IORslt.string.strptr;
        length = IORslt.string.len;
    }
    if( width == 0 ) {
        width = length;
    }
    if( width >= length ) {
        fcb->col += width - length;
        width = length;
    }
    ChkBuffLen( width );
    blanks = length - width;
    src = &fcb->buffer[ fcb->col ];
    pgm_memput( ptr, src, width );
    pgm_memset( ptr + width, ' ', blanks );
    fcb->col += width;
}


void    R_FOLog( void ) {
//=================

    if( UndefLogRtn() ) return;
    R_FmtLog( IOCB->fmtptr->fmt1.fld1 );
}


static  void    SetLogValue( logstar4 value ) {
//=============================================

    if( IOCB->typ == PT_LOG_1 ) {
        *(logstar1 PGM *)(IORslt.pgm_ptr) = value;
    } else {
        *(logstar4 PGM *)(IORslt.pgm_ptr) = value;
    }
}


void    R_FILog( void ) {
//=================

    ftnfile     *fcb;
    uint        width;
    char        ch;

    fcb = IOCB->fileinfo;
    width = IOCB->fmtptr->fmt1.fld1;
    ChkBuffLen( width );
    for(;;) {
        if( fcb->buffer[ fcb->col ] != ' ' ) break;
        fcb->col++;
        if( --width == 0 ) break;
    }
    if( fcb->buffer[ fcb->col ] == '.' ) {
         --width;
         fcb->col++;
    }
    if( width == 0 ) {
        IOErr( IO_BAD_CHAR );
    } else {
        ch = toupper( fcb->buffer[ fcb->col ] );
        if( ch == 'F' ) {
            SetLogValue( _LogValue( FALSE ) );
        } else if( ch == 'T' ) {
            SetLogValue( _LogValue( TRUE ) );
        }
    }
    if( __AllowCommaSep() ) { // don't flush but search for comma separator
        fcb->col++;
        width--;
        while( width > 0 ) {
            if( fcb->buffer[ fcb->col ] == ',' ) {
                fcb->col++;
                break;
            }
            fcb->col++;
            width--;
        }
    } else {
        fcb->col += width; // flush over other stuff
    }
}


void    R_FIFloat( void ) {
//===================

// Input an real or complex variable in D, E, F, G format.

    extended     value;
    fmt2 PGM    *fmtptr;
    ftnfile     *fcb;
    PTYPE       typ;
    int         prec;
    int         status;
    bool        comma;
    int         width;

    fcb = IOCB->fileinfo;
    fmtptr = &IOCB->fmtptr->fmt2;
    typ = IOCB->typ;
    ChkBuffLen( fmtptr->fld1 );
    switch( typ ) {
    case( PT_REAL_8 ):
    case( PT_CPLX_16 ):
        prec = PRECISION_DOUBLE;
        break;
    case( PT_REAL_16 ):
    case( PT_CPLX_32 ):
        prec = PRECISION_EXTENDED;
        break;
    default:
        prec = PRECISION_SINGLE;
    };
    comma = __AllowCommaSep();
    status = FmtS2F( &fcb->buffer[ fcb->col ], fmtptr->fld1,
                     fmtptr->fld2, ( fcb->blanks == BLANK_ZERO ),
                     IOCB->scale, prec, &value, comma, &width, FALSE );
    if( status == FLT_OK ) {
        if( comma && ( fmtptr->fld1 != width ) ) {
            fcb->col += width;
            if( fcb->buffer[ fcb->col ] == ',' ) {
                fcb->col++;
            } else {
                IOErr( IO_BAD_CHAR );
            }
        } else {
            fcb->col += fmtptr->fld1;
        }
        if( typ  == PT_REAL_4 ) {
            *(single PGM *)(IORslt.pgm_ptr) = value;
        } else if( typ  == PT_REAL_8 ) {
            *(double PGM *)(IORslt.pgm_ptr) = value;
        } else if( typ  == PT_REAL_16 ) {
            *(extended PGM *)(IORslt.pgm_ptr) = value;
        } else if( typ  == PT_CPLX_8 ) {
            if( IOCB->flags & IOF_FMTREALPART ) {
                ((scomplex PGM *)(IORslt.pgm_ptr))->realpart = value;
            } else {
                ((scomplex PGM *)(IORslt.pgm_ptr))->imagpart = value;
            }
        } else if( typ  == PT_CPLX_16 ) {
            if( IOCB->flags & IOF_FMTREALPART ) {
                ((dcomplex PGM *)(IORslt.pgm_ptr))->realpart = value;
            } else {
                ((dcomplex PGM *)(IORslt.pgm_ptr))->imagpart = value;
            }
        } else {
            if( IOCB->flags & IOF_FMTREALPART ) {
                ((xcomplex PGM *)(IORslt.pgm_ptr))->realpart = value;
            } else {
                ((xcomplex PGM *)(IORslt.pgm_ptr))->imagpart = value;
            }
        }
    } else {
        if( status == FLT_INVALID ) {
            IOErr( IO_BAD_CHAR );
        } else { // FLT_RANGE_EXCEEDED
            IOErr( IO_FRANGE_EXCEEDED );
        }
    }
}


bool    GetReal( extended *value ) {
//================================

    PTYPE       typ;
    single      *short_flt;
    bool        defined;

    typ = IOCB->typ;
    if( ( typ == PT_REAL_4  ) || ( typ == PT_CPLX_8 ) ) {
        SetMaxPrec( MAX_SP );
        if( typ  == PT_REAL_4 ) {
            short_flt = &IORslt.single;
        } else if( IOCB->flags & IOF_FMTREALPART ) {     // PT_CPLX_8
            short_flt = &IORslt.scomplex.realpart;
        } else {
            short_flt = &IORslt.scomplex.imagpart;
        }
        *value = *short_flt;
        defined = !UndefRealRtn( short_flt );
    } else if( ( typ == PT_REAL_8 ) || ( typ == PT_CPLX_16 ) ) {
        SetMaxPrec( MAX_DP );
        if( typ == PT_REAL_8 ) {
            *value = IORslt.dble;
        } else if( IOCB->flags & IOF_FMTREALPART ) {    // PT_CPLX_16
            *value = IORslt.dcomplex.realpart;
        } else {
            *value = IORslt.dcomplex.imagpart;
        }
        defined = !UndefDoubleRtn( value );
    } else {
        SetMaxPrec( MAX_XP );
        if( typ == PT_REAL_16 ) {
            *value = IORslt.extended;
        } else if( IOCB->flags & IOF_FMTREALPART ) {    // PT_CPLX_32
            *value = IORslt.xcomplex.realpart;
        } else {
            *value = IORslt.xcomplex.imagpart;
        }
        defined = !UndefExtendedRtn( value );
    }
    return( defined );
}


void    R_FOF( void ) {
//===============

    ftnfile     *fcb;
    fmt2 PGM    *fmt;
    char        *buf;
    uint        wid;
    extended    val;

    fcb = IOCB->fileinfo;
    buf = &fcb->buffer[ fcb->col ];
    fmt = &IOCB->fmtptr->fmt2;
    wid = fmt->fld1;
    if( GetRealRtn( &val, wid ) ) {
        R_F2F( val, buf, wid, fmt->fld2, (IOCB->flags & IOF_PLUS) != 0,
               IOCB->scale );
    }
    fcb->col += wid;
}


void    R_FOE( int exp, char ch ) {
//=================================

    ftnfile     *fcb;
    fmt3 PGM    *fmt;
    char        *buf;
    uint        wid;
#if defined( _M_IX86 ) || defined( __AXP__ ) || defined( __PPC__ )
    extended      absvalue;
#endif
    extended      value;
    extended      bot;
    extended      top;

    fcb = IOCB->fileinfo;
    buf = &fcb->buffer[ fcb->col ];
    fmt = &IOCB->fmtptr->fmt3;
    wid = fmt->fld1;
    if( GetRealRtn( &value, wid ) ) {
        // if Ew.d or Dw.d format, exp == 0
        // (i.e. exponent width not specified)
        if( exp == 0 ) {
#if defined( _M_IX86 ) || defined( __AXP__ ) || defined( __PPC__ )
            absvalue = value;
            if( value < 0.0 ) {
                absvalue = -value;
            }

            top = P1d100;
            if( IOCB->scale != 0 ) { // we must shift the range of our check;
                bot = pow( 10, IOCB->scale );
                top *= bot;
                bot *= P1d_99;
            } else {
                bot = P1d_99;
            }

            if( ( ( absvalue <= bot ) || ( absvalue >= top ) ) &&
                ( absvalue != 0.0 ) ) {
                ch = NULLCHAR;   // no exponent letter
                exp = 3;
            } else {
#endif
                exp = 2;
#if defined( _M_IX86 ) || defined( __AXP__ ) || defined( __PPC__ )
            }
#endif
        }
        R_F2E( value, buf, wid, fmt->fld2, (IOCB->flags & IOF_PLUS) != 0,
               IOCB->scale, exp, ch );
    }
    fcb->col += wid;
}


static bool FmtH2B( char *src, uint width, char PGM *dst, int len, PTYPE typ ) {
//==============================================================================

    char        ch1;
    byte        ch2;
    bool        valid;
    char        *stop;

#if defined( _M_IX86 ) || defined( __AXP__ ) || defined( __PPC__ )
#else
    typ = typ;
#endif
    pgm_memset( dst, 0, len );
    if( width >= 2 * len ) {
        len *= 2;
        src += width - len;
    } else {
        len = width;
    }
    stop = src + len;
    ch1 = '0';
    if( ( len & 1 ) == 0 ) {
        ch1 = *src;
        src++;
    }
    ch2 = *src;
    src++;
#if defined( _M_IX86 ) || defined( __AXP__ ) || defined( __PPC__ )
    if( typ != PT_CHAR ) {
        ++len;
        len &= ~1;
        len /= 2;
        dst += len - 1;
    }
#endif
    for(;;) {
        valid = FALSE;
        if( !isxdigit( ch1 ) ) {
            if( ch1 != ' ' ) break;
            ch1 = '0';
        }
        if( !isxdigit( ch2 ) ) {
            if( ch2 != ' ' ) break;
            ch2 = '0';
        }
        valid = TRUE;
        *dst = ( Hex( ch1 ) << 4 ) + Hex( ch2 );
#if defined( _M_IX86 ) || defined( __AXP__ ) || defined( __PPC__ )
        if( typ == PT_CHAR ) {
            ++dst;
        } else {
            --dst;
        }
#else
        dst++;
#endif
        if( src == stop ) break;
        ch1 = *src;
        src++;
        ch2 = *src;
        src++;
    }
    return( valid );
}


void    R_FIHex( void ) {
//=================

    uint        width;
    int         len;
    ftnfile     *fcb;
    PTYPE       typ;
    void        PGM *ptr;

    fcb = IOCB->fileinfo;
    width = IOCB->fmtptr->fmt1.fld1;
    len =  GetLen();
    typ = IOCB->typ;
    ChkBuffLen( width );
    if( typ == PT_CHAR ) {
        ptr = IORslt.string.strptr;
    } else {
        ptr = IORslt.pgm_ptr;
        if( typ == PT_CPLX_8 ) {
            if( IOCB->flags & IOF_FMTREALPART ) {
                ptr = &((scomplex *)ptr)->realpart;
            } else {
                ptr = &((scomplex *)ptr)->imagpart;
            }
        } else if( typ == PT_CPLX_16 ) {
            if( IOCB->flags & IOF_FMTREALPART ) {
                ptr = &((dcomplex *)ptr)->imagpart;
            } else {
                ptr = &((dcomplex *)ptr)->imagpart;
            }
        }
    }
    if( !FmtH2B( &fcb->buffer[ fcb->col ], width, ptr, len, typ ) ) {
        IOErr( IO_BAD_CHAR );
    }
    fcb->col += width;
}


static void FOHex( uint width ) {
//===============================

    uint        len;
    int         trunc;
    ftnfile     *fcb;
    PTYPE       typ;
    char        *buff;

    fcb = IOCB->fileinfo;
    typ = IOCB->typ;
    len = GetLen();
    trunc = 0;

//  Use this method when real and imaginary parts are formatted using
//  one edit descriptor:
/*
    if( ( IOCB->typ == PT_CPLX_8 ) || ( IOCB->typ == PT_CPLX_16 ) ) {
        len *= 2;
        IOCB->flags &= ~IOF_FMTREALPART;  // we'll print both parts at once
    }
*/
//  Use this method when real and imaginary parts each require an
//  edit descriptor:

    if( IOCB->typ == PT_CPLX_8 ) {
        if( !(IOCB->flags & IOF_FMTREALPART) ) {
            IORslt.scomplex.realpart = IORslt.scomplex.imagpart;
        }
    } else if( IOCB->typ == PT_CPLX_16 ) {
        if( !(IOCB->flags & IOF_FMTREALPART) ) {
            IORslt.dcomplex.realpart = IORslt.dcomplex.imagpart;
        }
    } else if( IOCB->typ == PT_CPLX_32 ) {
        if( !(IOCB->flags & IOF_FMTREALPART) ) {
            IORslt.xcomplex.realpart = IORslt.xcomplex.imagpart;
        }
    }
    if( width == 0 ) {
        width = 2*len;
    }

    trunc = ( len * 2 ) - width;
    if( trunc < 0 ) {
        SendChar( ' ', -trunc );
    }
    if( trunc <= 0 ) {
        trunc = 0;
    }

    if( typ != PT_CHAR ) {
        len *= 2;
        HexFlip( (char *)&IORslt, len );
        BToHS( (char *)&IORslt , len, IOCB->buffer );
        strupr( IOCB->buffer );
        SendStr( IOCB->buffer + trunc, len - trunc );
    } else {
        buff = IOCB->buffer;
        len *= 2;
        if( len > IO_FIELD_BUFF ) {
            buff = RChkAlloc( len + 1 );
        }
        pgm_BToHS( IORslt.string.strptr, len, buff );
        strupr( buff );
        SendStr( buff + trunc, len - trunc );
        if( len > IO_FIELD_BUFF ) {
            RMemFree( buff );
        }
    }
}


void    R_FOHex( void ) {
//=================

    FOHex( IOCB->fmtptr->fmt1.fld1 );
}


static  void    HexFlip( char *src, int len ) {
//=============================================

// Convert number to hex string.

    char        *ptr;
    char        *last;
    int         num_len;
    char        tmp;

    ptr = src;
    num_len = len / 2;
    last = src + num_len - 1;
    while( num_len > 0 ) {
        tmp = *ptr;
        *ptr = *last;
        *last = tmp;
        ptr++;
        last--;
        num_len -= 2;
    }
}


void    R_FIInt( void ) {
//=================

    intstar4    value;
    uint        width;
    int         new_width;
    int         status;
    ftnfile     *fcb;
    bool        comma;

    fcb = IOCB->fileinfo;
    width = IOCB->fmtptr->fmt2.fld1;
    ChkBuffLen( width );
    comma = __AllowCommaSep();
    status = FmtS2I( &fcb->buffer[ fcb->col ], width,
                     ( fcb->blanks == BLANK_ZERO ), &value, comma, &new_width );
    if( status == INT_INVALID ) {
        IOErr( IO_BAD_CHAR );
    } else if( status == INT_OVERFLOW ) {
        IOErr( IO_IOVERFLOW );
    } else {
        if( comma && ( width != new_width ) ) { // use comma as field separator
            fcb->col += new_width;
            if( fcb->buffer[ fcb->col ] == ',' ) {
                fcb->col++;
            } else {
                IOErr( IO_BAD_CHAR );
            }
        } else {
            fcb->col += width;
        }
        if( IOCB->typ == PT_INT_1 ) {
            *(intstar1 PGM *)(IORslt.pgm_ptr) = value;
        } else if( IOCB->typ == PT_INT_2 ) {
            *(intstar2 PGM *)(IORslt.pgm_ptr) = value;
        } else {
            *(intstar4 PGM *)(IORslt.pgm_ptr) = value;
        }
    }
}


void    R_FOInt( void ) {
//=================

    OutInt( IOCB->fmtptr->fmt2.fld1, IOCB->fmtptr->fmt2.fld2 );
}


static  void    OutInt( uint width, uint min ) {
//==============================================

    char        *number;
    uint        length;
    uint        space;
    bool        minus;
    intstar4    iorslt;

    if( UndefIntRtn( width ) ) return;
    iorslt = IORslt.intstar4;
    if( ( iorslt == 0 ) && ( min == 0 ) ) {
        SendChar( ' ', width );
    } else {
        minus = ( iorslt < 0 );
        number = IOCB->buffer;
        if( minus ) {
            number++; // skip the minus sign
        }
        ltoa( iorslt, IOCB->buffer, 10 );
        length = strlen( number );
        if( length > min ) {
            min = length;
        }
        if( min <= width ) {
            space = width - min;
            if( minus || ( IOCB->flags & IOF_PLUS ) ) {
                if( space != 0 ) {
                    SendChar( ' ', space - 1 );
                    if( minus ) {
                        Drop( '-' );
                    } else {
                        Drop( '+' );
                    }
                    SendChar( '0', min - length );
                    SendStr( number, length );
                } else {
                    SendChar( '*', width );
                }
            } else {
                SendChar( ' ', space );
                SendChar( '0', min - length );
                SendStr( number, length );
            }
        } else {
            SendChar( '*', width );
        }
    }
}


static  int     Div10S( real val ) {
//==================================

    int         retn;

    if( val < SInfinity.value ) {
        retn = -1;
        while( val >= 1 ) {
          retn++;
          val /= 10;
        }
        return( retn );
    } else {
        return( INT_MAX );
    }
}


    //  this function is called to determine if a number printed in G
    //  format is printed with the E format or the F format

static  int     Div10L( double val ) {
//====================================

    int         retn;

       //  check for a NaN or Inf value
    unsigned short int * ui = (unsigned short int *) & val;
    if ( (ui [3] & 0x7FF0) == 0x7FF0 ) {    /* NaN or Inf */
        return( INT_MAX );
    }

    if ( val < DInfinity.value ) {
        retn = -1;
        while( val >= 1 ) {
          retn++;
              //   check for runaway due to a NaN
          // if ( retn == 1000 )
             // return ( INT_MAX );
          val /= 10;
        }
        return( retn );
    } else {
        return( INT_MAX );
    }
}


static  int     Div10X( extended val ) {
//======================================

    int         retn;

    if( val < XInfinity.value ) {
        retn = -1;
        while( val >= 1 ) {
          retn++;
          val /= 10;
        }
        return( retn );
    } else {
        return( INT_MAX );
    }
}


void    R_FOG( void ) {
//===============

    int         width;
    int         dec;
    int         exp;
    int         logval;
    char        *buf;
    ftnfile     *fcb;
    char        ch;
    extended    value;
    extended    absvalue;

    width = IOCB->fmtptr->fmt3.fld1;
    dec = IOCB->fmtptr->fmt3.fld2;
    exp = IOCB->fmtptr->fmt3.fld3;
    fcb = IOCB->fileinfo;
    buf = &fcb->buffer[ fcb->col ];
    if( IOCB->typ <= PT_LOG_4 ) {
        R_FOLog();
    } else if( IOCB->typ <= PT_INT_4 ) {
        OutInt( width, 1 );
    } else {
        if( GetRealRtn( &value, width ) ) {
            absvalue = value;
            if( value < 0.0 ) {
                absvalue = -value;
            }
            /* round to "dec" digits */
            absvalue = absvalue + .5 * pow( 10, -dec );
            if( ( IOCB->typ == PT_REAL_4 ) || ( IOCB->typ == PT_CPLX_8 ) ) {
                logval = Div10S( absvalue );
            } else if((IOCB->typ == PT_REAL_8) || (IOCB->typ == PT_CPLX_16)) {
                logval = Div10L( absvalue );
            } else {
                logval = Div10X( absvalue );
            }
               //  use E format if less than 0.1 unless value is zero
               //  use E format if there are more digits than the width
            if( (( absvalue < 0.1 ) || ( logval >= dec )) &&
                 ( value != 0.0 ) ) {
                ch = 'E';
                if( exp == 0 ) { // if Gw.d
#if defined( _M_IX86 ) || defined( __AXP__ ) || defined( __PPC__ )
                    if( ( ( absvalue <= P1d_99 ) || ( absvalue >= P1d100 ) ) &&
                        ( absvalue != 0.0 ) ) {
                        ch = NULLCHAR;   // no exponent letter
                        exp = 3;
                    } else {
#endif
                        exp = 2;
#if defined( _M_IX86 ) || defined( __AXP__ ) || defined( __PPC__ )
                    }
#endif
                }
                R_F2E( value, buf, width, dec, (IOCB->flags & IOF_PLUS) != 0,
                       IOCB->scale, exp, ch );
                fcb->col += width;
            } else {
                if( exp == 0 ) { // if Gw.d
                    exp = 4;
                } else {
                    exp += 2;
                }
                width -= exp;
                if( width > 0 ) {
                    dec -= ( logval + 1 );
                    if( ( dec <= width ) && ( dec >= 0 ) ) {
                        R_F2F( value, buf, width, dec,
                               ( IOCB->flags & IOF_PLUS ) != 0, 0 );
                        fcb->col += width;
                        if( *buf == '*' ) {
                            // fill remaining field
                            SendChar( '*', exp );
                        } else {
                            SendChar( ' ', exp );
                        }
                    } else {
                        SendChar( '*', width + exp );
                    }
                } else {
                    SendChar( '*', width + exp );
                }
            }
        } else { // undefined chars will be filled in for value
            fcb->col += width;
        }
    }
}


void    ChkBuffLen( uint width ) {
//================================

    if( IOCB->fileinfo->col + width > IOCB->fileinfo->len ) {
        IOErr( IO_BUFF_LEN );
    }
}
