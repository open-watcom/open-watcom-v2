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
* Description:  Scanner
*
****************************************************************************/


#include "ftnstd.h"
#include "errcod.h"
#include "extnsw.h"
#include "stmtsw.h"
#include "global.h"
#include "csetinfo.h"
#include "cpopt.h"
#include "ferror.h"
#include "comio.h"

#include <string.h>
#include <stdlib.h>

extern  character_set   CharSetInfo;

#define COLUMNS 17

static  const token_state __FAR StateTable[][COLUMNS] = {

// AL  EX  SG  DP  DI  HL  AP  OP  SP  TC  BC  EL  CM  OL  HX  CS  DB
  SAN,SAN,SSG,SLL,SNM,SAN,SFQ,SOP,SSP,STC,SBC,SNR,SCM,SAN,SAN,SAN,SDB, // NS
  SSO,SEX,SSO,SML,SNM,SHL,SSO,SSO,SSP,STC,SBC,SNR,SCM,SSO,SSO,SSO,SSO, // NM
  SAN,SAN,SSO,SSO,SAN,SAN,SSO,SSO,SSP,STC,SBC,SNR,SCM,SAN,SAN,SAN,SDB, // AN
  SSO,SSO,SSO,SML,SNM,SSO,SSO,SSO,SSP,STC,SBC,SNR,SCM,SSO,SSO,SSO,SSO, // SG
  SSO,SEX,SSO,SSO,SFT,SSO,SSO,SSO,SSP,STC,SBC,SNR,SCM,SSO,SSO,SSO,SSO, // FT
  SLG,SLG,SEN,SSO,SEN,SLG,SSO,SSO,SSP,STC,SBC,SNR,SCM,SLG,SLG,SLG,SDB, // LX
  SSO,SSO,SEN,SSO,SEN,SSO,SSO,SSO,SSP,STC,SBC,SNR,SCM,SSO,SSO,SSO,SSO, // EX
  SSO,SSO,SSO,SSO,SEN,SSO,SSO,SSO,SSP,STC,SBC,SNR,SCM,SSO,SSO,SSO,SSO, // EN
  SLG,SLG,SSO,SFL,SSO,SLG,SSO,SSO,SSP,STC,SBC,SNR,SCM,SLG,SLG,SLG,SDB, // LG
  SLG,SLX,SSO,SSO,SFT,SLG,SSO,SSO,SSP,STC,SBC,SNR,SCM,SLG,SLG,SLG,SDB, // ML
  SIQ,SIQ,SIQ,SIQ,SIQ,SIQ,SAP,SIQ,SIQ,SIQ,SIQ,SNR,SIQ,SIQ,SIQ,SIQ,SIQ, // IQ
  SSO,SSO,SSO,SSO,SSO,SSO,SIQ,SSO,SSO,STC,SBC,SNR,SCM,SOL,SHX,SCS,SSO, // AP
  SIH,SIH,SIH,SIH,SIH,SIH,SIH,SIH,SIH,SIH,SIH,SNR,SIH,SIH,SIH,SIH,SIH, // IH
  SFM,SFM,SFM,SFM,SFM,SFM,SFM,SFM,SFM,SFM,SFM,SNR,SFM,SFM,SFM,SFM,SFM, // FM
  SSO,SSO,SSO,SSO,SSO,SSO,SSO,SSO,SSO,SSO,SBC,SNR,SCM,SSO,SSO,SSO,SSO, // OL
  SSO,SSO,SSO,SSO,SSO,SSO,SSO,SSO,SSO,SSO,SBC,SNR,SCM,SSO,SSO,SSO,SSO, // HX
  SSO,SSO,SSO,SSO,SSO,SSO,SSO,SSO,SSO,SSO,SBC,SNR,SCM,SSO,SSO,SSO,SSO, // CS
  SLG,SLG,SSO,SSO,SFT,SLG,SSO,SSO,SSP,STC,SBC,SNR,SCM,SLG,SLG,SLG,SDB, // LL
};

#define BAD_LOG         14

char    *LogTab[] = {
        "EQ",   "NE",    "LT",    "GT",    "LE",     "GE",
        "OR",   "AND",   "NOT",   "EQV",   "NEQV",   "XOR",
        "TRUE", "FALSE", NULL
};

static  char            ExpREA;
static  char            ExpDBL;
static  char            ExpEXT;
static  TOKCLASS        TokenREA;
static  TOKCLASS        TokenDBL;
static  TOKCLASS        TokenEXT;

/* Forward declarations */
static  void    ScanNum( void );
static  void    LkUpLog( void );

void    InitScan( void ) {
//==================

// Initialize the scanner.

    if( Options & OPT_EXTEND_REAL ) {
        TokenREA = TO_DBL;
        ExpREA = 'D';
        TokenDBL = TO_EXT;
        ExpDBL = 'Q';
    } else {
        TokenREA = TO_REA;
        ExpREA = 'E';
        TokenDBL = TO_DBL;
        ExpDBL = 'D';
    }
    TokenEXT = TO_EXT;
    ExpEXT = 'Q';
    Line = 0;
    State = SNS;
    TkCrsr = &TokenBuff[ 0 ];
    LexToken.stop = TkCrsr;
    LexToken.col = Column;
    LexToken.flags = 0;
    LexToken.line = 0;
    SrcRecNum = CurrFile->rec;
    // at this point, the line has already been read
    // so just print it with an ISN in front
    LinePrint();
    if( StmtType == STMT_CONT ) {
        Error( CC_NOT_INITIAL );
    }
    // examine the statement number if there was one
    ScanNum();
    ExtnSw &= ~XS_CONT_20;
}


void    Scan( void ) {
//==============

// Collect a token.

    token_state state2;
    byte        ch;
    token_state old_state;
    char        *dpt = NULL;
    byte        tab;
    int         len;
    int         hlen;
    TOKCLASS    class;
    char_class  ch_class;
    char_class  wasextch;

    if( !(LexToken.flags & TK_LAST) ) {
        wasextch = 0;
        old_state = SNS;
        LexToken.start = LexToken.stop;
        LexToken.col = Column;
        LexToken.line = Line;
        for(;;) {
            ch = *Cursor;
            ch_class = CharSetInfo.character_set[ ch ];
            wasextch |= ch_class;
            state2 = StateTable[ State ][ ch_class & C_MASK ];
            switch( state2 ) {
            case SAN :
            case SLG :
                if( ch_class & C_LOW ) { // lower case character
                    ch += 'A' - 'a';
                }
                *TkCrsr = ch;
                TkCrsr++;
                State = state2;
                break;
            case SDB :
                if( CharSetInfo.character_width( Cursor ) != 2 ) {
                    Error( CC_BAD_CHAR );
                } else if( CharSetInfo.is_double_byte_blank( Cursor ) ) {
                    *Cursor = ' ';
                    *( Cursor + 1 ) = ' ';
                    Column--; // compensate for Column++ and Cursor++
                    Cursor--; // after select
                } else {
                    State = StateTable[ State ][ C_AL ];
                    *TkCrsr = ch;
                    TkCrsr++;
                    Cursor++;
                    Column++;
                    *TkCrsr = *Cursor;
                    TkCrsr++;
                }
                break;
            case SNM :
            case SFT :
            case SEN :
                *TkCrsr = ch;
                TkCrsr++;
                State = state2;
                break;
            case SSG :
                *TkCrsr = ch;
                TkCrsr++;
                // 0..71 is the statement area
                // 72..79 is the sequence area
                ++Cursor;
                if( ++Column >= LastColumn ) { // we've just processed column 72
                    *Cursor = NULLCHAR;
                }
                State = SOP;
                class = TO_OPR;
                goto token;
            case SEX :
            case SLX :
                if( LexToken.flags & TK_LENSPEC ) {
                    LexToken.flags &= ~TK_LENSPEC;
                    goto token;
                }
                if( ch_class & C_LOW ) { // lower case character
                    ch += 'A' - 'a';
                }
                State = state2;
                switch( ch ) {
                case( 'Q' ):
                    class = TokenEXT;
                    break;
                case( 'D' ):
                    class = TokenDBL;
                    break;
                case( 'E' ):
                    class = TokenREA;
                    break;
                }
                *TkCrsr = ch;
                TkCrsr++;
                break;
            case SML :
            case SLL :
                dpt = TkCrsr;
                old_state = State;
                *TkCrsr = ch;
                TkCrsr++;
                State = state2;
                break;
            case SFQ :
                State = SIQ;
                break;
            case SIQ :
                state2 = SIQ;
            case SFM :
                if( ch_class == C_TC ) {
                    tab = 8 - Column % 8;
                    // Column gets incremented normally at bottom of loop
                    Column += tab - 1;
                    memset( TkCrsr, ' ', tab );
                    TkCrsr += tab;
                } else {
                    *TkCrsr = ch;
                    TkCrsr++;
                }
                State = state2;
                break;
            case SOL :
            case SHX :
            case SCS :
                *TkCrsr = NULLCHAR; // for conversion routines
                TkCrsr++;
            case SAP :
                State = state2;
                break;
            case SSO :
                goto token;
            case SOP :
                *TkCrsr = ch;
                TkCrsr++;
                // 0..71 is the statement area
                // 72..79 is the sequence area
                ++Cursor;
                if( ++Column >= LastColumn ) { // we've just processed column 72
                    *Cursor = NULLCHAR;
                }
                State = SOP;
                class = TO_OPR;
                goto token;
            case SFL :
                if( old_state == SNS ) {
                    // Consider: i .eq. j
                    LkUpLog();
                    if( LexToken.log != BAD_LOG ) {
                        // if it's a valid logical operator,
                        // set final state to SFL
                        State = state2;
                    } else if( dpt == NULL ) {
                        dpt = LexToken.start;
                    }
                } else {
                    // Consider: 1 .eq. 2
                    State = state2;
                }
                goto token;
            case SHL :
                if( LexToken.flags & TK_LENSPEC ) {
                    LexToken.flags &= ~TK_LENSPEC;
                    goto token;
                }
                *TkCrsr = NULLCHAR;
                len = atoi( LexToken.start );
                LexToken.start = TkCrsr;
                State = SIH;
                class = TO_LIT;
                StmtSw |= SS_HOLLERITH;
                break;
            case SIH :
                if( TkCrsr - LexToken.start >= len ) {
                    TkCrsr = LexToken.start + len; // in case of TABCHAR
                    State = SSO;
                    goto token;
                }
                if( ch_class == C_TC ) {
                    tab = 8 - Column % 8;
                    // Column gets incremented normally at bottom of loop
                    Column += tab - 1;
                    memset( TkCrsr, ' ', tab );
                    TkCrsr += tab;
                } else {
                    *TkCrsr = ch;
                    TkCrsr++;
                }
                break;
            case SSP :
                if( State == SNS ) {
                    LexToken.col = Column;
                }
                break;
            case STC :
                // Column gets incremented normally at bottom of loop
                Column += 7 - Column % 8;
                break;
            case SBC :
                Error( CC_BAD_CHAR );
                break;
            case SCM :
                if( !(ExtnSw & XS_EOL_COMMENT) ) {
                    Extension( CC_EOL_COMMENT );
                    ExtnSw |= XS_EOL_COMMENT;
                }
            case SNR :
                if( LexToken.flags & TK_INCLUDE ) {
                    LexToken.flags |= TK_LAST;
                    goto token;
                }
                // 0..71 is the statement area
                // 72..79 is the sequence area
                // calculate the number of spaces
                // that we may require for filling
                tab = LastColumn - Column;
                ComRead();
                if( StmtType != STMT_CONT ) {
                    LexToken.flags |= TK_LAST;
                    goto token;
                }
                if( ( State == SIQ ) || ( State == SIH ) || ( State == SFM ) ) {
                    memset( TkCrsr, ' ', tab );
                    TkCrsr += tab;
                }
                ++Line;
                ComPrint();
                ScanNum();
                if( Line >= 20 ) {
                    if( !(ExtnSw & XS_CONT_20) ) {
                        Extension( CC_TOO_MANY_CONT );
                        ExtnSw |= XS_CONT_20;
                    }
                    if( (TkCrsr-TokenBuff) + (LastColumn-CONT_COL) > TOKLEN ) {
                        TkCrsr = TokenBuff; // so we don't overflow TokenBuff
                        if( !(StmtSw & SS_CONT_ERROR_ISSUED) ) {
                            Error( CC_CONT_OVERFLOW );
                            StmtSw |= SS_CONT_ERROR_ISSUED;
                        }
                    }
                }
                if( State == SNS ) {
                    LexToken.col = Column;
                }
                Column--; // to offset ++Cursor and ++Column in code
                Cursor--; // after } (must be after ScanNum
                break;    // and Error since Error uses Column)
            }
            // 0..71 is the statement area
            // 72..79 is the sequence area
            ++Cursor;
            if( ++Column >= LastColumn ) { // we've just processed column 72
                *Cursor = NULLCHAR;
            }
        }
token:  LexToken.stop  = TkCrsr;
        state2 = State;                // final state for Scan
        State = SNS;                   // set to no state for next time
        switch( state2 ) {
        case SAN :
            class = TO_NAM;
            break;
        case SNM :
            class = TO_INT;
            break;
        case SFT :
            class = TokenREA;
            break;
        case SOL :
            class = TO_OCT;
            break;
        case SHX :
            class = TO_HEX;
            break;
        case SCS :
        case SAP :
            class = TO_LIT;
            break;
        case SFL :
            if( old_state == SNS ) {
                LexToken.start++;              // skip over starting dot
                // 0..71 is the statement area
                // 72..79 is the sequence area
                ++Cursor;
                if( ++Column >= LastColumn ) { // just processed column 72
                    *Cursor = NULLCHAR;
                }
                class = TO_LGL;
            } else {
                State = SLG;               // remember logical collected
                LexToken.stop = dpt;          // decimal not part of token
                class = TO_INT;
            }
            break;
        case SML :
            if( ( old_state == SNS ) || ( old_state == SSG ) ) {
                class = TO_OPR;
            } else {
                class = TokenREA;
            }
            break;
        case SLX :
        case SLG :
            LexToken.stop = dpt + sizeof( char ); // decimal is part of token
            State = SAN;                       // remember alphanumeric started
            if( old_state == SNS ) {
                class = TO_OPR;
            } else {
                class = TokenREA;
            }
            break;
        case SNS :
            LexToken.flags |= TK_EOL;
        case SLL :
            class = TO_OPR;
            break;
        case SIH :
            // We get here if and only if we are in a hollerith constant
            // and we've reached the end of the line (next line is not a
            // continuation line) and the length of the hollerith constant
            // has not been met.
            // "tab" is how may characters left until column 72
            hlen = TkCrsr - LexToken.start;        // get how many chars so far
            if( tab + hlen < len ) {            // if not enough characters
                Warning( HO_CUT_OFF );          // - say the hollerith cut off
                len = tab + hlen;               // - set length up to col 72
            }                                   // }
            memset( TkCrsr, ' ', len - hlen );  // fill rest of hollerith
            TkCrsr = LexToken.start + len;         // set new collection location
            break;
        case SFM :
            class = TO_FMT;
            break;
        case SIQ :
            Error( SX_QUOTE );
            class = TO_LIT;
            break;
        }
        LexToken.class = class;
        if( !(ExtnSw & XS_CHAR_EXTN) && ( class != TO_LIT ) &&
            ( class != TO_FMT ) && ( wasextch & C_EXT ) ) {
            Extension( CC_SET_EXTEND );
            ExtnSw |= XS_CHAR_EXTN;
        }
    } else {
        if( State == SAN ) {
            // Consider:   ... SYM.FIELD
            // ".FIELD" would have been scanned in anticipation of a logical
            // constant or operator but we reached the end of the statement
            // so that last token is "FIELD".
            LexToken.start = LexToken.stop;
            LexToken.stop = TkCrsr;
            LexToken.class = TO_NAM;
        } else {
            LexToken.class = TO_OPR;
        }
        LexToken.flags |= TK_EOL;
    }
}


static  void    ScanNum( void ) {
//=========================

    if( StmtType != STMT_CONT ) {
        StmtNo = NextStmtNo;
        if( StmtNo == 0 ) {
            if( StmtNoFound ) {
                Error( ST_NUM_ZERO );
            }
        }
    } else {
        if( StmtNoFound ) {
            Error( CC_STMTNO_ON_CONT );
        }
        if( ( ContType == C_BC ) || ( ContType & C_EXT ) ) {
            // non-FORTRAN char or WATFOR-77 extension
            // but issue message only once
            if( !(ExtnSw & XS_CHAR_EXTN) ) {
                Extension( CC_SET_EXTEND );
                ExtnSw |= XS_CHAR_EXTN;
            }
        }
    }
}


static  void    LkUpLog( void ) {
//=========================

    int         index;
    char        *ptr;

    index = 0;
    ptr = LexToken.start + sizeof( char ); // skip over "."
    *TkCrsr = NULLCHAR;
    for(;;) {
        if( strcmp( LogTab[ index ], ptr ) == 0 ) break;
        if( LogTab[ ++index ] == NULL ) break;
    }
    LexToken.log = index;
}
