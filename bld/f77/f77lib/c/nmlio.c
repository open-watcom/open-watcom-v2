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
* Description:  Run-time NAMELIST i/o processing.
*
****************************************************************************/


#include "ftnstd.h"
#include <string.h>
#include <ctype.h>
#include "rundat.h"
#include "pgmacc.h"
#include "errcod.h"
#include "symdefs.h"
#include "iotype.h"
#include "lgadv.h"
#include "nmlinfo.h"
#include "nmlio.h"
#include "chrutils.h"
#include "rtspawn.h"
#include "ioerr.h"
#include "substr.h"
#include "subscr.h"
#include "wrutils.h"
#include "freein.h"
#include "freeout.h"
#include "rdutils.h"
#include "unfmtutl.h"


static uint_32      NmlIn_count;
static PTYPE        NmlIn_ptyp;
static union {
    char            PGM *p;
    string          PGM *s;
} NmlIn_addr;

static char *Scanner( void )
//==========================
// Point to where we are currently scanning.
{
    ftnfile     *fcb;

    fcb = IOCB->fileinfo;
    return( &fcb->buffer[fcb->col] );
}


static bool     ScanChar( char chr )
//==================================
// Scan a character.
{
    if( *Scanner() == chr ) {
        IOCB->fileinfo->col++;
        return( true );
    }
    return( false );
}


static bool ScanEOL( void )
//=========================
// Check for end-of-line.
{
    return( *Scanner() == NULLCHAR );
}


static char     *ScanName( uint *len )
//====================================
// Scan a name.
{
    char        *name;
    char        *ptr;
    char        chr;

    name = Scanner();
    for( ptr = name; ; ++ptr ) {
        chr = *ptr;
        if( isalnum( chr ) )
            continue;
        if( chr == '_' )
            continue;
        if( chr == '$' )
            continue;
        break;
    }
    *len = ptr - name;
    IOCB->fileinfo->col += *len;
    return( name );
}


static bool     ScanSNum( int_32 PGM *num )
//=========================================
// Scan a signed number.
{
    char        *ptr;

    Blanks();
    ptr = Scanner();
    if( ( *ptr == '+' )
      || ( *ptr == '-' ) ) {
        ++ptr;
    }
    if( isdigit( *ptr ) ) {
        *num = GetNum();
        Blanks();
        return( true );
    } else {
        Blanks();
        return( false );
    }
}


static  intstar4        SubScr( int info, char PGM *adv_ss_ptr, int size )
//========================================================================
// Get a subscript list.
{
    int_32              ss[MAX_DIM];
    act_dim_list        dim_list;
    intstar4            PGM *bounds;
    int_32              PGM *ss_ptr;
    intstar4            lo;
    intstar4            offset;
    int                 dim_cnt;

    dim_list.dim_flags = 0;
    dim_cnt = _GetNMLSubScrs( info );
    _SetDimCount( dim_list.dim_flags, dim_cnt );
    dim_list.num_elts = 1;
    bounds = &dim_list.subs_1_lo;
    ss_ptr = ss;
    for( ;; ) {
        if( !ScanSNum( ss_ptr ) )
            return( false );
        ++ss_ptr;
        lo = *(intstar4 PGM *)adv_ss_ptr;
        adv_ss_ptr += sizeof( intstar4 );
        dim_list.num_elts *= *(uint PGM *)adv_ss_ptr;
        *bounds++ = lo;
        *bounds++ = lo + *(uint PGM *)adv_ss_ptr - 1;
        adv_ss_ptr += sizeof( uint );
        if( --dim_cnt == 0 )
            break;
        if( !ScanChar( ',' ) ) {
            break;
        }
    }
    if( !ScanChar( ')' ) )
        return( false );
    if( !DoSubscript( &dim_list, ss, &offset ) )
        return( false );
    NmlIn_addr.p = (char HPGM *)NmlIn_addr.p + offset * size;
    return( true );
}


static  bool    SubStr( string *scb )
//===================================
// Get a substring list.
{
    intstar4    ss1;
    intstar4    ss2;

    ss1 = 1;
    ss2 = scb->len;
    if( !ScanChar( ':' ) ) {
        if( !ScanSNum( &ss1 ) )
            return( false );
        if( !ScanChar( ':' ) ) {
            return( false );
        }
    }
    if( !ScanChar( ')' ) ) {
        if( !ScanSNum( &ss2 ) )
            return( false );
        if( !ScanChar( ')' ) ) {
            return( false );
        }
    }
    if( !DoSubstring( ss1, ss2, scb->len ) )
        return( false );
    scb->len = ss2 - ss1 + 1;
    scb->strptr = scb->strptr + ss1 - 1;
    return( true );
}


static  char PGM *FindNmlEntry( char *name, uint len )
//====================================================
// Scan NAMELIST information for given NAMELIST entry.
{
    byte        nml_len;
    char        PGM *nml;
    byte        info;

    nml = IOCB->u.ptr;
    nml_len = *nml++;
    for( nml += nml_len; (nml_len = *nml++) != 0; nml += sizeof( char PGM * ) ) {
        if( nml_len == len ) {
            if( pgm_memicmp( nml, name, len ) == 0 ) {
                return( nml + len );
            }
        }
        nml += nml_len;
        info = *nml++;
        if( _GetNMLSubScrs( info )
          && (info & NML_LG_ADV) == 0 ) {
            nml += sizeof( uint_32 ) + _GetNMLSubScrs( info ) * ( sizeof( uint_32 ) + sizeof( int ) );
            if( _GetNMLType( info ) == FPT_CHAR ) {
                nml += sizeof( int );
            }
        }
    }
    return( NULL );
}


static io_type_rtn  NmlIOType;
static PTYPE        NmlIOType( void )
//===================================
// Get the type of an input item.
{
    uint        save_col;
    uint        len;
    char        *ptr;

    // when collecting constants, make sure we haven't encountered the
    // next name in the NAMELIST as in the following case
    //     LOGICAL A(3)
    //     INTEGER T
    //     NAMELIST /NML/ A, T
    //     READ( 1, NML )
    // Unit 1:
    //     &NML
    //     A = T F
    //     T = 4
    //     &END
    // when we are scanning "T = 4", we have to make sure that the T is
    // not interpreted as the value "true" for the 3rd element of A

    if( IOCB->rptnum <= 1 ) {
        Blanks();
        CheckEor();
        BumpComma();    // skip the "," if present
        CheckEor();
        Blanks();
        save_col = IOCB->fileinfo->col;
        ptr = ScanName( &len );
        // Make sure we have seen a name so that we don't confuse the opening
        // parenthesis of a complex number as an array element indicator
        //     COMPLEX A(2)
        //     NAMELIST /NML/ A
        //     READ( 1, NML )
        // Unit 1:
        //     &NML
        //     A = (1,2) (3,4)
        //     &END
        if( len != 0 ) {
            Blanks();
            if( ScanChar( '=' ) )
                NmlIn_count = 0;
            if( ScanChar( '(' ) ) {
                NmlIn_count = 0; // may be an array element
            }
        }
        IOCB->fileinfo->col = save_col;
    }

    if( NmlIn_count == 0 )
        return( FPT_NOTYPE );
    --NmlIn_count;
    if( NmlIn_ptyp == FPT_CHAR ) {
        IORslt.string = *NmlIn_addr.s;
        NmlIn_addr.s->strptr = (char HPGM *)NmlIn_addr.s->strptr + NmlIn_addr.s->len;
    } else { // numeric or logical
        IORslt.pgm_ptr = NmlIn_addr.p;
        NmlIn_addr.p = (char HPGM *)NmlIn_addr.p + SizeVars[NmlIn_ptyp];
    }
    return( NmlIn_ptyp );
}


static  void    NmlIn( void )
//===========================
{
    char        PGM *nml;
    byte        nml_len;
    char        *ptr;
    uint        len;
    char        PGM *nml_entry;
    byte        info;
    string      scb;
    char        e_chr;
    lg_adv      PGM *adv_ptr;
    char        PGM *adv_ss_ptr;
    uint        size;

    IOTypeRtn = NmlIOType;
    IOCB->rptnum = -1;  // initialize for first call to NmlIOType()
    nml = IOCB->u.ptr;
    nml_len = *nml++; // get length of NAMELIST name
    e_chr = '&';    // assume '&' used
    for( ;; ) {     // find the start of the NAMELIST information
        NextRec();
        Blanks();
        if( !ScanChar( '&' ) ) {
            if( !ScanChar( '$' ) )
                continue;
            e_chr = '$'; // a '$' was used instead
        }
        ptr = ScanName( &len );
        if( nml_len != len )
            continue;
        if( pgm_memicmp( ptr, nml, len ) != 0 )
            continue;
        if( ScanEOL()
          || ScanChar( ' ' )
          || ScanChar( '\t' ) ) {
            break;
        }
    }
    for( ;; ) {
        Blanks();
        CheckEor();
        Blanks();
        ptr = Scanner();
        if( *ptr == e_chr ) {
            ++ptr;
            if( ( toupper( ptr[0] ) == 'E' )
              && ( toupper( ptr[1] ) == 'N' )
              && ( toupper( ptr[2] ) == 'D' ) ) {
                ptr += 3;
            }
            if( *JmpBlanks( ptr ) == NULLCHAR ) {
                break;
            }
        }
        ptr = ScanName( &len );
        nml_entry = FindNmlEntry( ptr, len );
        if( nml_entry == NULL ) {
            ptr[len] = NULLCHAR;
            IOErr( IO_NML_NO_SUCH_NAME, ptr );
            // never return
        }
        info = *nml_entry++;
        NmlIn_ptyp = _GetNMLType( info );
        if( _GetNMLSubScrs( info ) ) {  // array
            if( info & NML_LG_ADV ) {
                adv_ptr = *(lg_adv PGM * PGM *)nml_entry;
                NmlIn_addr.p = adv_ptr->origin;
                if( NmlIn_ptyp == FPT_CHAR ) {
                    scb.len = adv_ptr->elt_size;
                }
                NmlIn_count = adv_ptr->num_elts;
                adv_ss_ptr = (char PGM *)adv_ptr + ADV_BASE_SIZE;
            } else {
                NmlIn_count = *(uint_32 PGM *)nml_entry;
                nml_entry += sizeof( uint_32 );
                adv_ss_ptr = nml_entry;
                nml_entry += _GetNMLSubScrs( info ) * ( sizeof( uint_32 ) + sizeof( int ) );
                if( NmlIn_ptyp == FPT_CHAR ) {
                    scb.len = *(uint PGM *)nml_entry;
                    nml_entry += sizeof( uint );
                }
                NmlIn_addr.p = *(char PGM * PGM *)nml_entry;
            }
            if( ScanChar( '(' ) ) {
                if( NmlIn_ptyp == FPT_CHAR ) {
                    size = scb.len;
                } else {
                    size = SizeVars[NmlIn_ptyp];
                }
                if( !SubScr( info, adv_ss_ptr, size ) ) {
                    IOErr( IO_NML_BAD_SUBSCRIPT );
                    // never return
                }
            }
            if( NmlIn_ptyp == FPT_CHAR ) {
                scb.strptr = NmlIn_addr.p;
                if( ScanChar( '(' ) ) {
                    if( !SubStr( &scb ) ) {
                        IOErr( IO_NML_BAD_SUBSTRING );
                        // never return
                    }
                }
                NmlIn_addr.s = &scb;
            }
        } else { // variable
            NmlIn_count = 1;
            NmlIn_addr.p = *(char PGM * PGM *)nml_entry;
            if( NmlIn_ptyp == FPT_CHAR ) {
                scb = *NmlIn_addr.s;
                if( ScanChar( '(' ) ) {
                    if( !SubStr( &scb ) ) {
                        IOErr( IO_NML_BAD_SUBSTRING );
                        // never return
                    }
                }
                NmlIn_addr.s = &scb;
            }
        }
        IOCB->ptyp = NmlIn_ptyp;
        Blanks();
        if( !ScanChar( '=' ) ) {
            IOErr( IO_NML_BAD_SYNTAX );
            // never return
        }
        Blanks();
        if( RTSpawn( &DoFreeIn ) != 0 ) {
            // RTSuicide() was called - make sure it really was an error
            if( IOCB->flags & NML_CONTINUE ) {
                IOCB->flags &= ~NML_CONTINUE;
                continue;
            }
            // we got an error during NAMELIST input
            RTSuicide();
            // never return
        }
    }
    IOTypeRtn = &IOType;
}


static  void    NmlOut( void )
//============================
{
    char        PGM *nml;
    byte        len;
    byte        info;
    PTYPE       ptyp;
    uint_32     num_elts;
    char        PGM *data;
    string      scb;
    lg_adv      PGM *adv_ptr;

    nml = IOCB->u.ptr;
    len = *nml++; // get length of NAMELIST name
    Drop( ' ' );
    Drop( '&' );
    SendStr( nml, len );
    SendEOR();
    for( nml += len; (len = *nml++) != 0; nml += sizeof( char PGM * ) ) {
        Drop( ' ' );
        SendStr( nml, len );
        nml += len;
        SendWSLStr( " = " );
        info = *nml++;
        ptyp = _GetNMLType( info );
        IOCB->ptyp = ptyp;
        if( _GetNMLSubScrs( info ) ) {
            if( info & NML_LG_ADV ) {
                adv_ptr = *(lg_adv PGM * PGM *)nml;
                num_elts = adv_ptr->num_elts;
                if( ptyp == FPT_CHAR ) {
                    scb.len = adv_ptr->elt_size;
                    scb.strptr = adv_ptr->origin;
                } else {
                    data = adv_ptr->origin;
                }
            } else {
                num_elts = *(uint_32 PGM *)nml;
                nml += sizeof( uint_32 ) + _GetNMLSubScrs( info ) * ( sizeof( uint_32 ) + sizeof( int ) );
                if( ptyp == FPT_CHAR ) {
                    scb.len = *(uint PGM *)nml;
                    nml += sizeof( uint );
                    scb.strptr = *(char PGM * PGM *)nml;
                } else {
                    data = *(char PGM * PGM *)nml;
                }
            }
            while( num_elts-- > 0 ) {
                if( ptyp == FPT_CHAR ) {
                    IORslt.string = scb;
                    OutRtn[ptyp]();
                    Drop( ' ' );
                    scb.strptr += scb.len;
                } else {
                    pgm_memget( &IORslt, data, SizeVars[ptyp] );
                    OutRtn[ptyp]();
                    data += SizeVars[ptyp];
                }
            }
        } else {
            switch( ptyp ) {
            case FPT_LOG_1:
                IORslt.logstar4 = **(logstar1 PGM * PGM *)nml;
                break;
            case FPT_LOG_4:
                IORslt.logstar4 = **(logstar4 PGM * PGM *)nml;
                break;
            case FPT_INT_1:
                IORslt.intstar4 = **(intstar1 PGM * PGM *)nml;
                break;
            case FPT_INT_2:
                IORslt.intstar4 = **(intstar2 PGM * PGM *)nml;
                break;
            case FPT_INT_4:
                IORslt.intstar4 = **(intstar4 PGM * PGM *)nml;
                break;
            case FPT_REAL_4:
                IORslt.single = **(single PGM * PGM *)nml;
                break;
            case FPT_REAL_8:
                IORslt.dble = **(double PGM * PGM *)nml;
                break;
            case FPT_REAL_16:
                IORslt.extended = **(extended PGM * PGM *)nml;
                break;
            case FPT_CPLX_8:
                IORslt.scomplex = **(scomplex PGM * PGM *)nml;
                break;
            case FPT_CPLX_16:
                IORslt.dcomplex = **(dcomplex PGM * PGM *)nml;
                break;
            case FPT_CPLX_32:
                IORslt.xcomplex = **(xcomplex PGM * PGM *)nml;
                break;
            case FPT_CHAR:
                IORslt.string = **(string PGM * PGM *)nml;
                break;
            }
            OutRtn[ptyp]();
        }
        SendEOR();
    }
    SendWSLStr( " &END" );
    SendEOR();
}


void    NmlExec( void )
//=====================
{
    if( IOCB->flags & IOF_OUTPT ) {
        NmlOut();
    } else {
        NmlIn();
    }
    IOCB->ptyp = FPT_NOTYPE;
}


void    NmlAddrs( va_list args )
//==============================
// Get addresses of NAMELIST symbols.
{
    char        PGM *nml;
    byte        len;
    byte        info;

    nml = IOCB->u.ptr;
    len = *nml++;
    for( nml += len; (len = *nml++) != 0; nml += sizeof( char PGM * ) ) {
        nml += len;
        info = *nml++;
        if( _GetNMLSubScrs( info )
          && (info & NML_LG_ADV) == 0 ) {
            nml += sizeof( uint_32 ) + _GetNMLSubScrs( info ) * ( sizeof( uint_32 ) + sizeof( int ) );
            if( _GetNMLType( info ) == FPT_CHAR ) {
                nml += sizeof( int );
            }
        }
        *(char PGM * PGM *)nml = va_arg( args, char PGM * );
    }
}
