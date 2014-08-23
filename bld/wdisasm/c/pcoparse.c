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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include "disasm.h"
#include "wdismsg.h"

#define NUM_BYTES   ( sizeof( rec_type ) * ( CHAR_MAX / CHAR_BIT ) )

void  ParseObjectOMF()
/********************/
{
    uint_8  msgs[ NUM_BYTES  ]; /* bit fields for message flags */
    int     i;
    div_t   res;

    for( i = 0; i < NUM_BYTES; ++i ) {
        msgs[ i ] = 0;
    }

    Mod = NULL;
    GetObjRec();
    if( RecType == 0 ) {
        return;
    }
    if( RecType != CMD_THEADR ) {
        Error( ERR_THEADR_NOT_1ST, true );
    }
    THeadr();
    do {
        ExchangeBuffers();
        GetObjRec();
        switch( RecType ) {
        case CMD_LNAMES:
        case CMD_LLNAME:
            LName();
            break;
        case CMD_SEGDEF:
        case CMD_SEGD32:
            SegDef();
            break;
        case CMD_LEDATA:
        case CMD_LEDA32:
            LEData();
            break;
        case CMD_FIXUPP:
        case CMD_FIXU32:
            FixUpp();
            break;
        case CMD_PUBDEF:
        case CMD_PUBD32:
            PubDef( true );
            break;
        case CMD_STATIC_PUBDEF:
        case CMD_STATIC_PUBD32:
            PubDef( false );
            break;
        case CMD_EXTDEF:
            ExtDef( true );
            break;
        case CMD_STATIC_EXTDEF:
        case CMD_STATIC_EXTD32:
            ExtDef( false );
            break;
        case CMD_CEXTDF:
            CExtDef();
            break;
        case CMD_GRPDEF:
            GrpDef();
            break;
        case CMD_MODEND:
        case CMD_MODE32:
            ModEnd();
            break;
        case CMD_LIDATA:
        case CMD_LIDA32:
            LIData();
            break;
        case CMD_LINNUM:
        case CMD_LINN32:
            LinNum();
            break;
        case CMD_LINSYM:
            LinSym();
            break;
        case CMD_COMDEF:
            ComDef( true );
            break;
        case CMD_STATIC_COMDEF:
            ComDef( false );
            break;
        case CMD_COMDAT:
            ComDat();
            break;
        case CMD_COMENT:
            Coment();
            break;
        case CMD_BAKPAT:
        case CMD_BAKP32:
            BackPatch();
            break;
        case CMD_NBKPAT:
            NBackPatch();
            break;
        case CMD_VERNUM:
        case CMD_VENDEXT:
        case CMD_REGINT:
        case CMD_OVLDEF:
        case CMD_LOCSYM:
        case CMD_DEBSYM:
        case CMD_ENDREC:
        case CMD_BLKDEF:
        case CMD_BLKEND:
        case CMD_TYPDEF:
            SkipPcoRec();
            break;
        case 0:
            Error( ERR_UNEXPECTED_EOF, false );
            break;
        default:
            res = div( RecType, CHAR_BIT );
            i = 1 << res.rem;
            if( !( msgs[ res.quot ] & i ) ) {
                char            tmpstr[MAX_RESOURCE_SIZE];
                uint_16         tmp_options;
                msgs[ res.quot ] |= i;
                MsgGet( ERR_UNKNOWN_REC, tmpstr );
                tmp_options = Options;
                Options = 0;    // force Intel style hex output
                FmtIToHS( tmpstr, RecType, 2 );
                Options = tmp_options;
                DoError( tmpstr, false );
            }
            SkipPcoRec();
        }
    } while( RecType != CMD_MODEND && RecType != CMD_MODE32 && RecType != 0 );
}


static  FmtIToHS( char tmpstr[], rec_type rectp, int len )
/******************************************************/
{
    char        tmprec[10];
//    int               i;
//    int               length;

    DoIToHS( tmprec, rectp, len );
/*    for( i = 0; tmpstr[i] != NULLCHAR; i++ ) {
        if( tmpstr[i] == '%' && tmpstr[i+1] == '2' ) {
            length = strlen( &tmpstr[i] );
            memmove( &tmpstr[i+1], &tmpstr[i], length );
            if( i + length >= MAX_RESOURCE_SIZE - 1 ) {
                tmpstr[MAX_RESOURCE_SIZE - 1] = NULLCHAR;
            } else {
                tmpstr[i+length+1] = NULLCHAR;
            }
            break;
        }
    }
*/    MsgSubStr( tmpstr, tmprec, '1' );
    MsgChgeSpec( tmpstr, '2' );
}
