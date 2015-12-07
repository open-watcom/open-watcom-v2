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


//
// LEX       : lexical analyser
//

#include <string.h>
#include "ftnstd.h"
#include "opr.h"
#include "opn.h"
#include "errcod.h"
#include "stmtsw.h"
#include "progsw.h"
#include "global.h"
#include "ferror.h"
#include "frl.h"
#include "comio.h"
#include "scan.h"

extern  char            *StmtKeywords[];
extern  char            *LogTab[];

#define LOG_OPS         11
#define XLOG_OPS        11

static  const OPR __FAR        LogOpr[] = { // must correspond to table in SCAN
        OPR_EQ,  OPR_NE,  OPR_LT,  OPR_GT,  OPR_LE,   OPR_GE,
        OPR_OR,  OPR_AND, OPR_NOT, OPR_EQV, OPR_NEQV, OPR_NEQV,
        OPR_PHI, OPR_PHI, OPR_PHI
};

/* Forward declarations */
static  void    GetOpnd( void );
static  void    FlushStatement( void );
static  void    GetOpr( void );
static  void    SetSwitch( void );



static  itnode  *NewITNode( void ) {
//==================================

// Create a new itnode.

    itnode      *new;

    new = FrlAlloc( &ITPool, sizeof( itnode ) );
    if( new != NULL ) {
        new->opnd = Lex.ptr;
        new->opnd_size = Lex.len;
        new->opr = Lex.opr;
        new->oprpos = Lex.oprpos;
        new->opn.ds = Lex.opn.ds;
        new->opnpos = Lex.opnpos;
        new->link = NULL;
        new->list = NULL;
        new->flags = 0;
        new->typ = FT_NO_TYPE;
        new->chsize = 0;
        new->is_unsigned = 0;
        new->is_catparen = 0;
    }
    return( new );
}


void    MakeITList( void ) {
//====================

// Make up the internal text list.

    itnode      *new_it;

    InitScan();
    Lex.opr = OPR_TRM;
    Lex.oprpos = ( LexToken.line << 8 ) + LexToken.col + 1;
    Scan();
    if( !(LexToken.flags & TK_EOL) ) {
        if( LexToken.col < CONT_COL - 1 ) {
            Error( CC_NOT_DIGITS );
        }
    }
    BrCnt = 0;
    SPtr1 = NULL;
    SPtr2 = NULL;
    GetOpnd();
    ITHead = NewITNode();
    if( ITHead == NULL ) {
        FlushStatement();
        return;
    }
    CITNode = ITHead;
    for(;;) {
        if( LexToken.flags & TK_EOL ) break;
        GetOpr();
        GetOpnd();
        // Consider what happens when NewITNode() calls FrlAlloc() who
        // in turns calls FMemAlloc() and there is no memory left.
        // FMemAlloc() frees the I.T. list starting at ITHead so when
        // we return CITNode has been freed.
        // So don't code ---->    CITNode->link = NewITNode();
        new_it = NewITNode();
        if( new_it == NULL ) {
            FlushStatement();
            return;
        }
        CITNode->link = new_it;
        CITNode = CITNode->link;
        SetSwitch();
    }
    // If the last token in the statement is a NULL operand, then
    // make the caret point 1 past the operator. Otherwise
    // it will point at the end of the line and for fixed-length
    // files which contain trailing blanks this will look funny.
    if( CITNode->opn.ds == DSOPN_PHI ) {
        CITNode->opnpos = CITNode->oprpos + 1;
    }
    if( BrCnt != 0 ) {
        Error( PC_NO_CLOSEPAREN );
    }
    Lex.opr = OPR_TRM;
    Lex.opn.ds = DSOPN_PHI;
    Lex.oprpos = 9999;
    Lex.opnpos = 9999;
    Lex.len = 0;
    new_it = NewITNode();
    if( new_it == NULL ) {
        FlushStatement();
        return;
    }
    CITNode->link = new_it;
    CITNode->link->link = NULL;
    CITNode = ITHead;
}


static  void FlushStatement( void ) {
//========================================

    for(;;) {
        ComRead();
        if( ( ProgSw & PS_SOURCE_EOF ) != 0 ) break;
        if( StmtType != STMT_CONT ) break;
        ComPrint();
    }
}


static  void    SetSwitch( void ) {
//===========================

// Set statement switches according to operators.

    if( Lex.opr == OPR_LBR ) {
        ++BrCnt;
    } else if( Lex.opr == OPR_RBR ) {
        --BrCnt;
        if( BrCnt < 0 ) {
            Error( PC_UNMATCH_PAREN );
            BrCnt = 0;
        } else if( BrCnt == 0 ) {
            if( SPtr1 == NULL ) {
                SPtr1 = CITNode;
            } else if( SPtr2 == NULL ) {
                SPtr2 = CITNode;
            }
        }
    } else if( BrCnt == 0 ) {
        if( Lex.opr == OPR_COM ) {
            if( ( StmtSw & SS_EQUALS_FOUND ) != 0 ) {
                StmtSw |= SS_EQ_THEN_COMMA;
            } else {
                StmtSw |= SS_COMMA_FOUND;
            }
        } else if( Lex.opr == OPR_EQU ) {
            if( ( StmtSw & SS_COMMA_FOUND ) != 0 ) {
                StmtSw |= SS_COMMA_THEN_EQ;
            } else {
                StmtSw |= SS_EQUALS_FOUND;
            }
        }
    }
}


static  OPR    LkUpOpr( void ) {
//=========================

    switch( *LexToken.start ) {
    case ',':   return( OPR_COM );
    case '(':   return( OPR_LBR );
    case ')':   return( OPR_RBR );
    case '=':   return( OPR_EQU );
    case '/':   return( OPR_DIV );
    case '*':   return( OPR_MUL );
    case '-':   return( OPR_MIN );
    case '+':   return( OPR_PLS );
    case ':':   return( OPR_COL );
    case '%':   return( OPR_FLD );
    case '.':   return( OPR_DPT );
    }
    Error( SX_INV_OPR );
    return( OPR_PHI );
}


static  void    GetOpr( void ) {
//========================

// Collect an operator.

    Lex.oprpos = ( LexToken.line << 8 ) + LexToken.col + 1;
    if( LexToken.class == TO_OPR ) {
        Lex.opr = LkUpOpr();
        // this is to tell scan not to collect
        //     4d4      in    INTEGER*4  d4,a,b . . .
        //     4h,a,b   in    INTEGER*4  h,a,b . . .
        LexToken.flags &= ~TK_LENSPEC;
        if( ( Lex.opr == OPR_MUL ) && ( CITNode == ITHead ) ) {
            LexToken.flags |= TK_LENSPEC;
        }
        Scan();
        if( ( LexToken.class == TO_OPR ) && !(LexToken.flags & TK_EOL) &&
            ( ( Lex.opr == OPR_MUL ) || ( Lex.opr == OPR_DIV ) ) &&
            ( Lex.opr == LkUpOpr() ) ) {
            if( Lex.opr == OPR_MUL ) {
                Lex.opr = OPR_EXP;
            } else {
                Lex.opr = OPR_CAT;
            }
            Scan();
        }
    } else if( LexToken.class == TO_LGL ) {
        Lex.opr = LogOpr[ LexToken.log ];
        if( LexToken.log >= XLOG_OPS ) {
            Extension( MD_LOGOPR_EXTN, LogTab[ LexToken.log ] );
        }
        Scan();
    } else {
        Lex.opr = OPR_PHI;
    }
}


static  void    GetOpnd( void ) {
//=========================

// Scan off an operand.

    char        *kw;

    Lex.ptr = LexToken.start;
    Lex.opnpos = ( LexToken.line << 8 ) + LexToken.col + 1;
    if( LexToken.class == TO_OPR ) {
        Lex.opn.ds = DSOPN_PHI;
        Lex.len = 0;
    } else if( LexToken.class == TO_LGL ) {
        if( LexToken.log > LOG_OPS ) {
            Lex.len = LexToken.stop - LexToken.start;
            Lex.opn.ds = DSOPN_LGL;
            Scan();
        } else {
            Lex.len = 0;
            Lex.opn.ds = DSOPN_PHI;
        }
    } else {
        Lex.len = LexToken.stop - LexToken.start;
        Lex.opn.ds = LexToken.class;
        // this is a kludge to collect FORMAT/INCLUDE statements
        // we don't want INCLUDE statements to span lines
        if( (ITHead == NULL) && (Lex.opr == OPR_TRM) && (Lex.opn.ds == DSOPN_NAM) ) {
            if( Lex.len == 6 ) {
                if( ( Cursor == NULL ) || ( *Cursor == '(' ) ) {
                    kw = StmtKeywords[ PR_FMT ];
                    if( memcmp( LexToken.start, kw, 6 ) == 0 ) {
                        State = SFM;
                    }
                }
            } else if( Lex.len == 7 ) {
                if( ( Cursor != NULL ) && ( *Cursor == '\'' ) ) {
                    kw = StmtKeywords[ PR_INCLUDE ];
                    if( memcmp( LexToken.start, kw, 7 ) == 0 ) {
                        LexToken.flags |= TK_INCLUDE;
                    }
                }
            }
        }
        // end of kludge
        Scan();
    }
}
