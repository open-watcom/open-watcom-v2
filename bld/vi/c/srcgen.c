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

    tsf->token = token;
    tsf->line = CurrentSrcLine;
    tsf->hasvar = hasVar;

    if( where != NULL ) {
        AddString( &(tsf->data), where );
    }

    InsertLLItemAfter( (ss **)&tmpTail, (ss *)tmpTail, (ss *)tsf );

} /* genItem */

/*
 * GenJmpIf - jump based on last expression result
 */
void GenJmpIf( int when, label where )
{
    if( !EditFlags.ScriptIsCompiled ) {
        genItem( SRC_T_GOTO, where );
        tmpTail->branchcond = when;
        tmpTail->hasvar = FALSE;
    }

} /* GenJmpIf */

/*
 * GenJmp - stick a jump before current statment
 */
void GenJmp( label where )
{
    genItem( SRC_T_GOTO, where );
    tmpTail->branchcond = 2;
    tmpTail->hasvar = FALSE;

} /* GenJmp */

/*
 * GenLabel - stick a label before current statment
 */
void GenLabel( label where )
{
    vi_rc   rc;

    genItem( SRC_T_LABEL, where );
    if( rc = AddLabel( tmpTail, cLab, where ) ) {
        AbortGen( rc );
    }
    tmpTail->hasvar = FALSE;
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
    if( EditFlags.CompileScript ) {
        genItem( SRC_T_IF, v1 );
    } else {
        genItem( SRC_T_IF, NULL );
        AddString( &tmpTail->arg1, v1 );
    }

} /* GenTestCond */

/*
 * genExpr - gen an expression assignment
 */
static void genExpr( void )
{
    char        v1[MAX_SRC_LINE], v2[MAX_SRC_LINE], tmp[MAX_SRC_LINE];
    expr_oper   oper;

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
    oper = EXPR_EQ;
    if( tmp[1] == '=' && tmp[2] == 0 ) {
        if( tmp[0] == '+' ) {
            oper = EXPR_PLUSEQ;
        } else if( tmp[0] == '-' ) {
            oper = EXPR_MINUSEQ;
        } else if( tmp[0] == '*' ) {
            oper = EXPR_TIMESEQ;
        } else if( tmp[0] == '/' ) {
            oper = EXPR_DIVIDEEQ;
        } else {
            AbortGen( ERR_SRC_INVALID_EXPR );
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
    if( EditFlags.CompileScript ) {

        genItem( SRC_T_EXPR, StrMerge( 4, v1, SingleBlank, tmp, SingleBlank, v2 ) );

    } else {
        /*
         * build the expr data structure
         */
        genItem( SRC_T_EXPR, NULL );
        tmpTail->u.oper = oper;
        AddString( &tmpTail->arg1, v1 );
        AddString( &tmpTail->arg2, v2 );
    }

} /* genExpr */

/*
 * NewLabel - generate a new unique label
 */
label NewLabel( void )
{
    char        buff[MAX_NUM_STR];
    label       tmp;

    MySprintf( buff, "_l_%l", CurrentSrcLabel++ );
    AddString( &tmp, buff );
    return( tmp );

} /* NewLabel */

/*
 * PreProcess - pre-process source file
 */
vi_rc PreProcess( char *fn, sfile **sf, labels *lab )
{
    GENERIC_FILE        gf;
    int                 i, token, k, len, dammit, rec;
    sfile               *tsf;
    char                tmp[MAX_SRC_LINE], tmp2[MAX_SRC_LINE];
    char                tmp3[MAX_SRC_LINE];
    vi_rc               rc;
    bool                ret;

    /*
     * get source file
     */
    if( EditFlags.CompileScript ) {
        EditFlags.OpeningFileToCompile = TRUE;
    }
    ret = SpecialOpen( fn, &gf );
    EditFlags.OpeningFileToCompile = FALSE;
    if( !ret ) {
        return( ERR_FILE_NOT_FOUND );
    }

    /*
     * init control
     */
    CSInit();
    CurrentSrcLine = 0L;

    tsf = MemAlloc( sizeof( sfile ) );
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
        if( !EditFlags.ScriptIsCompiled ) {
            RemoveLeadingSpaces( tmp );
            k = strlen( tmp );
            memcpy( tmp3, tmp, k + 1 );
            if( (len = NextWord1( tmp, tmp2 )) <= 0 ) {
                continue;
            }
            if( tmp2[0] == '#' ) {
                continue;
            }
            hasVar = FALSE;
            for( i = 0; i < k; i++ ){
                if( tmp3[i] == '%' ) {
                    hasVar = TRUE;
                    break;
                }
            }

            /*
             * if we are appending (ie, an append token was encounterd
             * before, stop tokenizing
             */
            if( !EditFlags.Appending ) {
                token = Tokenize( SourceTokens, tmp2, TRUE );
                if( token == SRC_T_VBJ__ ) {
                    EditFlags.ScriptIsCompiled = TRUE;
                    continue;
                }
            } else {
                token = -1;
            }
        } else {
            len = NextWord1( tmp, tmp2 );
            hasVar = (bool) tmp2[0] - '0';
            token = atoi( &tmp2[1] );
        }

        /*
         * process recognized tokens
         */
        if( token >= 0 ) {

            RemoveLeadingSpaces( tmp );
            if( token > SRC_T_NULL ) {
                genItem( token, tmp );
                continue;
            }

            /*
             * get parm
             */
            AddString( &CurrentSrcData, tmp );
            freeSrcData = TRUE;

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
                    if( EditFlags.ScriptIsCompiled ) {
                        NextWord1( CurrentSrcData, tmp );
                        tmpTail->branchcond = atoi( CurrentSrcData );
                        strcpy( CurrentSrcData, tmp );
                    } else {
                        tmpTail->branchcond = 2;
                    }
                }
                tmpTail->data = CurrentSrcData;
                freeSrcData = FALSE;
                break;
            }
            if( freeSrcData ) {
                MemFree( CurrentSrcData );
            }
        /*
         * set all other tokens to be processed at run time
         */
        } else {
            if( EditFlags.ScriptIsCompiled ) {
                RemoveLeadingSpaces( tmp );
                genItem( token, tmp );
                continue;
            }
            if( tmp2[len - 1] == '!' ) {
                dammit = TRUE;
                tmp2[len - 1] = 0;
            } else {
                dammit = FALSE;
            }

            if( !EditFlags.Appending ) {
                rec = token = Tokenize( ParseClTokens, tmp2, TRUE );
                if( dammit && token != PCL_T_MAP ) {
                    tmp2[len - 1] = '!';
                    token = -1;
                }
            } else {
                token = -1;
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
                RemoveLeadingSpaces( tmp );
                token += SRC_T_NULL + 1;
                genItem( token, tmp );
                break;

            case PCL_T_MAP:
                token += SRC_T_NULL + 1;
                if( !dammit ) {
                    genItem( token, tmp );
                } else {
                    strcpy( tmp2, "! " );
                    strcat( tmp2, tmp );
                    genItem( token, tmp2 );
                }
                break;

            case PCL_T_SET:
                token += SRC_T_NULL + 1;
                if( EditFlags.CompileScript ) {
                    WorkLine->data[0] = 0;
                    rc = Set( tmp );
                    if( rc != ERR_NO_ERR ) {
                        Error( GetErrorMsg( rc ) );
                    }
                    genItem( token, WorkLine->data );
                } else {
                    genItem( token, tmp );
                }
                break;

            default:
                if( EditFlags.Appending ) {
                    if( tmp3[0] == '.' && tmp3[1] == 0 ) {
                        EditFlags.Appending = FALSE;
                    }
                } else if( rec < 0 ) {
                    /*
                     * see if the current token is a Ex token.  If
                     * it isn't, then see if the next one is
                     * (i.e., look for <n> append)
                     */
                    token = Tokenize( ExTokens, tmp2, FALSE );
                    if( token < 0 ) {
                        if( NextWord1( tmp, tmp2 ) >= 0 ) {
                            token = Tokenize( ExTokens, tmp2, FALSE );
                            if( token == EX_T_APPEND ) {
                                EditFlags.Appending = TRUE;
                            }
                        }
                    }
                }
                if( tmp3[0] == '>' ) {
                    tmp3[0] = ' ';
                }
                genItem( -1, tmp3 );
                break;
            }
        }

    }

    SpecialFclose( &gf );
    EditFlags.Appending = FALSE;
    return( CSFini() );

} /* PreProcess */
