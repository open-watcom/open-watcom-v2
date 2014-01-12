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


#include "plusplus.h"
#include "initdefs.h"
#include "stats.h"
#include "preproc.h"
#include "scan.h"
#include "name.h"
#include "weights.gh"
#include "kwhash.h"

#define BITSIZE   (CHAR_BIT * sizeof( kwflags ))

typedef unsigned char  kwflags;

ExtraRptCtr( ctr_calls );
ExtraRptCtr( ctr_ids );
ExtraRptCtr( ctr_len_min );
ExtraRptCtr( ctr_len_max );
ExtraRptCtr( ctr_len_mask );

static kwflags kw_flags[(T_LAST_TOKEN + BITSIZE) / BITSIZE] = {0};

static const kwflags _bits[BITSIZE] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };

TOKEN KwLookup(         // TRANSFORM TO T_ID OR KEYWORD TOKEN
    unsigned len )      // - length of id
{
    TOKEN    token;     // - token value for KW

#ifdef XTRA_RPT
    ExtraRptIncrementCtr( ctr_calls );
    if( len < LEN_MIN )
        ExtraRptIncrementCtr( ctr_len_min );
    if( len > LEN_MAX )
        ExtraRptIncrementCtr( ctr_len_max );
    if( (( 1 << len ) & LEN_MASK) == 0 )
        ExtraRptIncrementCtr( ctr_len_mask );
#endif
    token = keyword_hash( Buffer, TokValue, len ) + FIRST_KEYWORD;
    /* hash is now a token number */
    ++len;
    if( NameMemCmp( Tokens[token], Buffer, len ) != 0 || (kw_flags[token / BITSIZE] & _bits[token % BITSIZE]) ) {
        ExtraRptIncrementCtr( ctr_ids );
        token = T_ID;
    }
    return( token );
}

void KwDisable(                 // DISABLE A KEYWORD TOKEN TO T_ID
    TOKEN token )               // - token id
{
    kw_flags[token / BITSIZE] |= _bits[token % BITSIZE];
}

void KwEnable(                  // ENABLE A KEYWORD TOKEN FROM T_ID
    TOKEN token )               // - token id
{
    kw_flags[token / BITSIZE] &= ~_bits[token % BITSIZE];
}

#ifdef XTRA_RPT
static void init(               // INITIALIZATION
    INITFINI* defn )            // - definition
{
    defn = defn;
    ExtraRptRegisterCtr( &ctr_calls, "# keyword hashs" );
    ExtraRptRegisterCtr( &ctr_ids, "# keywords that are ids" );
    ExtraRptRegisterCtr( &ctr_len_mask, "# keywords that fail len mask test" );
    ExtraRptRegisterCtr( &ctr_len_min, "# keywords that fail min len test" );
    ExtraRptRegisterCtr( &ctr_len_max, "# keywords that fail max len test" );
}


INITDEFN( keyword, init, InitFiniStub );
#endif
