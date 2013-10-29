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


#include "cgstd.h"
#include "coderep.h"
#include "pattern.h"
#include "dumpio.h"

static char *opRegTab[] = {
    #define RG( a,b,c,d,e,f ) #f
    #include "rg.h"
    #undef RG
};

static char * VerTab[] = {
    #define _V_( x ) #x
    #include "v.h"
    #undef _V_
    ""
};

static char * GenTab[] = {
    #define _G_( x ) #x
    #include "g.h"
    #undef _G_
    "G_UNKNOWN",
    #define _R_( x, f ) #x
    #include "r.h"
    #undef _R_
    ""
};


static  void    PrtOps( operand_types ops ) {
/*******************************************/

    DumpChar( ',' );
    if( ops & R_1 ) {
        DumpChar( 'R' );
    }
    if( ops & M_1 ) {
        DumpChar( 'M' );
    }
    if( ops & U_1 ) {
        DumpChar( 'U' );
    }
    if( ops & C_1 ) {
        DumpChar( 'C' );
    }
    DumpChar( ',' );
    if( ops & R_2 ) {
        DumpChar( 'R' );
    }
    if( ops & M_2 ) {
        DumpChar( 'M' );
    }
    if( ops & U_2 ) {
        DumpChar( 'U' );
    }
    if( ops & C_2 ) {
        DumpChar( 'C' );
    }
    DumpChar( ',' );
    if( ops & R_R ) {
        DumpChar( 'R' );
    }
    if( ops & M_R ) {
        DumpChar( 'M' );
    }
    if( ops & U_R ) {
        DumpChar( 'U' );
    }
    if( ops & C_R ) {
        DumpChar( 'C' );
    }
    DumpChar( ',' );
    if( ( ops & NONE ) == NONE ) {
        DumpLiteral( "NONE" );
    } else if( ( ops & NONE ) == 0 ) {
        DumpLiteral( "BOTH_EQ" );
    } else if( ops & EQ_R2 ) {
        DumpLiteral( "EQ_R2" );
    } else {
        DumpLiteral( "EQ_R1" );
    }
    DumpChar( ')' );
}

extern  void    DumpGen( opcode_entry *try ) {
/********************************************/

    DumpPadString( GenTab[try->generate], 15 );
}

extern  void    DumpTab( opcode_entry *try ) {
/********************************************/
    vertype     verify;

    PrtOps( try->op_type );
    DumpChar( ',' );
    verify = try->verify;
    if( verify & NOT_VOLATILE ) {
        DumpLiteral( "NVI+" );
        verify &= ~NOT_VOLATILE;
    }
    DumpPadString( VerTab[verify], 10 );
    DumpChar( ',' );
    DumpGen( try );
    DumpChar( ',' );
    DumpPadString( opRegTab[try->reg_set], 10 );
    DumpNL();
}
