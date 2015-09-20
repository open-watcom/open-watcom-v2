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


#include "vi.h"
#include <setjmp.h>
#include "source.h"
#include "parsecl.h"
#include "ex.h"
#include "specio.h"

static sfile    *tmpTail;
static bool     freeSrcData, hasVar;
static labels   *cLab;
static jmp_buf  genExit;

void AbortGen( vi_rc rc )
{
    longjmp( genExit, (int)rc );
}

/*
 * genItem - generate a src file item
 */
static void genItem( int token, label where )
{
    sfile       *tsf;

    tsf = MemAlloc( sizeof( sfile ) );

    tsf->arg1 = NULL;
    tsf->arg2 = NULL;
    tsf->data = NULL;
    tsf->token = token;
    tsf->line = CurrentSrcLine;
    tsf->hasvar = hasVar;
    tsf->branchcond = COND_FALSE;

    if( where != NULL ) {
        tsf->data = DupString( where );
    }

    InsertLLItemAfter( (ss **)&tmpTail, (ss *)tmpTail, (ss *)tsf );

} /* genItem */

/*
 * GenJmpIf - jump based on last expression result
 */
void GenJmpIf( branch_cond when, label where )
{
#ifndef VICOMP
    if( !EditFlags.ScriptIsCompiled ) {
#endif
        genItem( SRC_T_GOTO, where );
        tmpTail->branchcond = when;
        tmpTail->hasvar = false;
#ifndef VICOMP
    }
#endif

} /* GenJmpIf */

/*
 * GenJmp - stick a jump before current statment
 */
void GenJmp( label where )
{
    genItem( SRC_T_GOTO, where );
    tmpTail->branchcond = COND_JMP;
    tmpTail->hasvar = false;

} /* GenJmp */

/*
 * GenLabel - stick a label before current statment
 */
void GenLabel( label where )
{
    vi_rc   rc;

    genItem( SRC_T_LABEL, where );
    if( (rc = AddLabel( tmpTail, cLab, where )) != ERR_NO_ERR ) {
        AbortGen( rc );
    }
    tmpTail->hasvar = false;
    strcpy( where, cLab->name[cLab->cnt - 1] );

} /* GenLabel */

/*
 * GenTestCond - generate test condition for current statement
 */
void GenTestCond( void )
{
    char        v1[MAX_SRC_LINE];

    /*
     * process syntax of test condition
     * IF expr
     */
    strcpy( v1, CurrentSrcData );
    RemoveLeadingSpaces( v1 );
    if( v1[0] == 0 ) {
        AbortGen( ERR_SRC_INVALID_IF );
    }

    /*
     * build the if data structure
     */
#ifndef VICOMP
    if( EditFlags.CompileScript ) {
#endif
        genItem( SRC_T_IF, v1 );
#ifndef VICOMP
    } else {
        genItem( SRC_T_IF, NULL );
        tmpTail->arg1 = DupString( v1 );
    }
#endif

} /* GenTestCond */

/*
 * genExpr - gen an expression assignment
 */
static void genExpr( void )
{
    char        v1[MAX_SRC_LINE], v2[MAX_SRC_LINE], tmp[MAX_SRC_LINE];
#ifndef VICOMP
    expr_oper   oper = EXPR_EQ;
#endif

    /*
     * get expression syntax :
     * EXPR %v = v1
     */
    if( NextWord1( CurrentSrcData, v1 ) <= 0 ) {
        AbortGen( ERR_SRC_INVALID_EXPR );
    }
    if( NextWord1( CurrentSrcData, tmp ) <= 0 ) {
        AbortGen( ERR_SRC_INVALID_EXPR );
    }
    if( tmp[1] == '=' && tmp[2] == 0 ) {
        switch( tmp[0] ) {
#ifndef VICOMP
        case '+': oper = EXPR_PLUSEQ; break;
        case '-': oper = EXPR_MINUSEQ; break;
        case '*': oper = EXPR_TIMESEQ; break;
        case '/': oper = EXPR_DIVIDEEQ; break;
#else
        case '+': break;
        case '-': break;
        case '*': break;
        case '/': break;
#endif
        default:
            AbortGen( ERR_SRC_INVALID_EXPR );
            break;
        }
    } else {
        if( tmp[0] != '=' || tmp[1] != 0 ) {
            AbortGen( ERR_SRC_INVALID_EXPR );
        }
    }
    strcpy( v2, CurrentSrcData );
    RemoveLeadingSpaces( v2 );
    if( v2[0] == 0 ) {
        AbortGen( ERR_SRC_INVALID_EXPR );
    }
#ifndef VICOMP
    if( EditFlags.CompileScript ) {
#endif
        genItem( SRC_T_EXPR, StrMerge( 4, v1, SingleBlank, tmp, SingleBlank, v2 ) );
#ifndef VICOMP
    } else {
        /*
         * build the expr data structure
         */
        genItem( SRC_T_EXPR, NULL );
        tmpTail->u.oper = oper;
        tmpTail->arg1 = DupString( v1 );
        tmpTail->arg2 = DupString( v2 );
    }
#endif

} /* genExpr */

/*
 * NewLabel - generate a new unique label
 */
label NewLabel( void )
{
    char        buff[MAX_NUM_STR];
    label       tmp;

    MySprintf( buff, "_l_%l", CurrentSrcLabel++ );
    tmp = DupString( buff );
    return( tmp );

} /* NewLabel */

/*
 * PreProcess - pre-process source file
 */
vi_rc PreProcess( const char *fn, sfile **sf, labels *lab )
{
    GENERIC_FILE        gf;
    int                 i, token, k, len;
    sfile               *tsf;
    char                tmp[MAX_SRC_LINE], tmp2[MAX_SRC_LINE];
    char                tmp3[MAX_SRC_LINE];
    bool                ret;
#ifdef VICOMP
    bool                AppendingFlag = false;
#else
    #define             AppendingFlag   EditFlags.Appending
#endif

    /*
     * get source file
     */
#ifdef VICOMP
    ret = SpecialOpen( fn, &gf );
#else
    if( EditFlags.CompileScript ) {
        EditFlags.OpeningFileToCompile = true;
        ret = SpecialOpen( fn, &gf, false );
        EditFlags.OpeningFileToCompile = false;
    } else {
        ret = SpecialOpen( fn, &gf, EditFlags.BoundData );
    }
#endif
    if( !ret ) {
        return( ERR_FILE_NOT_FOUND );
    }

    /*
     * init control
     */
    CSInit();
    CurrentSrcLine = 0L;

    tsf = MemAlloc( sizeof( sfile ) );
    tsf->next = NULL;
    tsf->prev = NULL;
    tsf->arg1 = NULL;
    tsf->arg2 = NULL;
    tsf->data = NULL;
    tsf->token = SRC_T_NULL;
    *sf = tmpTail = tsf;
    cLab = lab;

    /*
     * set up error handler
     */
    i = setjmp( genExit );
    if( i != 0 ) {
        SpecialFclose( &gf );
        return( (vi_rc)i );
    }

    /*
     * process each line
     */
    while( SpecialFgets( tmp, MAX_SRC_LINE - 1, &gf ) >= 0 ) {

        /*
         * prepare this line
         */
        CurrentSrcLine++;
#ifndef VICOMP
        if( !EditFlags.ScriptIsCompiled ) {
#endif
            RemoveLeadingSpaces( tmp );
            k = strlen( tmp );
            memcpy( tmp3, tmp, k + 1 );
            if( (len = NextWord1( tmp, tmp2 )) <= 0 ) {
                continue;
            }
            if( tmp2[0] == '#' ) {
                continue;
            }
            hasVar = false;
            for( i = 0; i < k; i++ ){
                if( tmp3[i] == '%' ) {
                    hasVar = true;
                    break;
                }
            }

            /*
             * if we are appending (ie, an append token was encounterd
             * before, stop tokenizing
             */
            if( !AppendingFlag ) {
                token = Tokenize( SourceTokens, tmp2, true );
#ifndef VICOMP
                if( token == SRC_T_VBJ__ ) {
                    EditFlags.ScriptIsCompiled = true;
                    continue;
                }
#endif
            } else {
                token = TOK_INVALID;
            }
#ifndef VICOMP
        } else {
            len = NextWord1( tmp, tmp2 );
            hasVar = ( tmp2[0] != '0' );
            token = atoi( &tmp2[1] );
        }
#endif
        /*
         * process recognized tokens
         */
        if( token != TOK_INVALID ) {

            RemoveLeadingSpaces( tmp );
            if( token > SRC_T_NULL ) {
                genItem( token, tmp );
                continue;
            }

            /*
             * get parm
             */
            CurrentSrcData = DupString( tmp );
            freeSrcData = true;

            /*
             * process token
             */
            switch( token ) {
            case SRC_T_EXPR:
                genExpr();
                break;
            case SRC_T_LABEL:
                GenLabel( tmp );
                break;
            case SRC_T_IF:
                CSIf();
                break;
            case SRC_T_QUIF:
                CSQuif();
                break;
            case SRC_T_ELSEIF:
                CSElseIf();
                break;
            case SRC_T_ELSE:
                CSElse();
                break;
            case SRC_T_ENDIF:
                CSEndif();
                break;
            case SRC_T_LOOP:
                CSLoop();
                break;
            case SRC_T_ENDLOOP:
            case SRC_T_ENDWHILE:
                CSEndLoop();
                break;
            case SRC_T_WHILE:
                CSWhile();
                break;
            case SRC_T_UNTIL:
                CSUntil();
                break;
            case SRC_T_BREAK:
                CSBreak();
                break;
            case SRC_T_CONTINUE:
                CSContinue();
                break;
            default:
                genItem( token, NULL );
                if( token == SRC_T_GOTO ) {
#ifndef VICOMP
                    if( EditFlags.ScriptIsCompiled ) {
                        NextWord1( CurrentSrcData, tmp );
                        tmpTail->branchcond = atoi( CurrentSrcData );
                        strcpy( CurrentSrcData, tmp );
                    } else {
#endif
                        tmpTail->branchcond = COND_JMP;
#ifndef VICOMP
                    }
#endif
                }
                tmpTail->data = CurrentSrcData;
                freeSrcData = false;
                break;
            }
            if( freeSrcData ) {
                MemFree( CurrentSrcData );
            }
        /*
         * set all other tokens to be processed at run time
         */
        } else {
#ifndef VICOMP
            if( EditFlags.ScriptIsCompiled ) {
                RemoveLeadingSpaces( tmp );
                genItem( token, tmp );
                continue;
            }
#endif
            if( !AppendingFlag ) {
                token = Tokenize( TokensCmdLine, tmp2, true );
            } else {
                token = TOK_INVALID;
            }
            switch( token ) {
            case PCL_T_COMMANDWINDOW:
            case PCL_T_STATUSWINDOW:
            case PCL_T_COUNTWINDOW:
            case PCL_T_EDITWINDOW:
            case PCL_T_EXTRAINFOWINDOW:
            case PCL_T_FILECWINDOW:
            case PCL_T_LINENUMBERWINDOW:
            case PCL_T_DIRWINDOW:
            case PCL_T_FILEWINDOW:
            case PCL_T_SETWINDOW:
            case PCL_T_SETVALWINDOW:
            case PCL_T_MESSAGEWINDOW:
            case PCL_T_MENUWINDOW:
            case PCL_T_MENUBARWINDOW:
            case PCL_T_ENDWINDOW:
            case PCL_T_SETCOLOR:
            case PCL_T_MATCH:
            case PCL_T_DIMENSION:
            case PCL_T_BORDER:
            case PCL_T_HILIGHT:
            case PCL_T_TEXT:
            case PCL_T_ALIAS:
            case PCL_T_ABBREV:
            case PCL_T_MENU:
            case PCL_T_MENUITEM:
            case PCL_T_ENDMENU:
            case PCL_T_WHITESPACE:
            case PCL_T_SELECTION:
            case PCL_T_EOFTEXT:
            case PCL_T_KEYWORD:
            case PCL_T_OCTAL:
            case PCL_T_HEX:
            case PCL_T_INTEGER:
            case PCL_T_CHAR:
            case PCL_T_PREPROCESSOR:
            case PCL_T_SYMBOL:
            case PCL_T_INVALIDTEXT:
            case PCL_T_IDENTIFIER:
            case PCL_T_JUMPLABEL:
            case PCL_T_COMMENT:
            case PCL_T_FLOAT:
            case PCL_T_STRING:
            case PCL_T_VARIABLE:
            case PCL_T_FILETYPESOURCE:
            case PCL_T_ENDFILETYPESOURCE:
            case PCL_T_LOCATE:
            case PCL_T_MAP:
            case PCL_T_MAP_DMT:

            case PCL_T_MENUFILELIST:
            case PCL_T_MENULASTFILES:

            case PCL_T_DEFAULTWINDOW:
            case PCL_T_ACTIVEMENUWINDOW:
            case PCL_T_GREYEDMENUWINDOW:
            case PCL_T_ACTIVEGREYEDMENUWINDOW:
                RemoveLeadingSpaces( tmp );
                token += SRC_T_NULL + 1;
                genItem( token, tmp );
                break;

            case PCL_T_SET:
                token += SRC_T_NULL + 1;
#ifdef VICOMP
                WorkLine->data[0] = 0;
                Set( tmp );
                genItem( token, WorkLine->data );
#else
                if( EditFlags.CompileScript ) {
                    vi_rc   rc;

                    WorkLine->data[0] = 0;
                    rc = Set( tmp );
                    if( rc != ERR_NO_ERR ) {
                        Error( GetErrorMsg( rc ) );
                    }
                    genItem( token, WorkLine->data );
                } else {
                    genItem( token, tmp );
                }
#endif
                break;

            default:
                if( AppendingFlag ) {
                    if( tmp3[0] == '.' && tmp3[1] == 0 ) {
                        AppendingFlag = false;
                    }
                } else if( token == TOK_INVALID ) {
                    /*
                     * see if the current token is a Ex token.  If
                     * it isn't, then see if the next one is
                     * (i.e., look for <n> append)
                     */
                    token = Tokenize( TokensEx, tmp2, false );
                    if( token == TOK_INVALID ) {
                        if( NextWord1( tmp, tmp2 ) >= 0 ) {
                            token = Tokenize( TokensEx, tmp2, false );
                            if( token == EX_T_APPEND ) {
                                AppendingFlag = true;
                            }
                        }
                    }
                }
                if( tmp3[0] == '>' ) {
                    tmp3[0] = ' ';
                }
                genItem( TOK_INVALID, tmp3 );
                break;
            }
        }

    }

    SpecialFclose( &gf );
    AppendingFlag = false;
    return( CSFini() );

} /* PreProcess */
