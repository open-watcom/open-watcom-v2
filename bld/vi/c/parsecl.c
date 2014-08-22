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
#include "walloca.h"
#include "parsecl.h"
#include "ex.h"

static char pDelims[] = " /!";
static char pkwDelims[] = " /";

/*
 * ParseCommandLine - parse a command line
 */
vi_rc ParseCommandLine( char *buff, linenum *n1, bool *n1flag, linenum *n2, bool *n2flag,
                      int *token, char *data )
{
    char        *tres, *tmp;
    int         i, j, k;
    linenum     l;
    vi_rc       rc;

    /*
     * set up for parse
     */
    tres = alloca( MAX_INPUT_LINE );
    tmp = alloca( MAX_INPUT_LINE );
    if( tmp == NULL || tres == NULL ) {
        return( ERR_NO_STACK );
    }
    *n1flag = false;
    *n2flag = false;
    data[0] = 0;

    /*
     * change null command to '.'
     */
    RemoveLeadingSpaces( buff );
    if( buff[0] == 0 ) {
        buff[0] = '.';
        buff[1] = 0;
    }

    /*
     * check for magic '%' - all lines
     */
    if( buff[0] == '%' ) {
        *n1flag = true;
        *n2flag = true;
        *n1 = 1;
        rc = CFindLastLine( n2 );
        if( rc != ERR_NO_ERR ) {
            return( rc );
        }
        EliminateFirstN( buff, 1 );
        RemoveLeadingSpaces( buff );
    /*
     * check for magic '#' - selected region
     */
    } else if( buff[0] == '#' || buff[0] == '@' ) {
        if( !SelRgn.selected ) {
            if( buff[0] == '#' ) {
                return( ERR_NO_SELECTION );
            } else {
                // use @ in scripts (eg mcsel.vi) when
                // we KNOW something was just selected
                SelRgn.selected = true;
            }
        }
        *n1flag = true;
        *n2flag = true;
        if( SelRgn.start.line > SelRgn.end.line ) {
            *n1 = SelRgn.end.line;
            *n2 = SelRgn.start.line;
        } else {
            *n1 = SelRgn.start.line;
            *n2 = SelRgn.end.line;
        }
        EliminateFirstN( buff, 1 );
        RemoveLeadingSpaces( buff );
    /*
     * try to get line range
     */
    } else {
        rc = GetAddress( buff, &l );
        if( rc > ERR_NO_ERR || rc == DO_NOT_CLEAR_MESSAGE_WINDOW ) {
            return( rc );
        }
        if( rc == ERR_NO_ERR ) {
            *n1flag = true;
            *n1 = l;
            RemoveLeadingSpaces( buff );
            if( buff[0] == ',' ) {
                EliminateFirstN( buff, 1 );
                RemoveLeadingSpaces( buff );
                rc = GetAddress( buff, &l );
                if( rc > ERR_NO_ERR ) {
                    return( rc );
                }
                if( rc != ERR_NO_ERR ) {
                    return( ERR_INVALID_COMMAND );
                }
                *n2flag = true;
                /*
                 * swap order (if start > end)
                 */
                if( *n1 > l ) {
                    *n2 = *n1;
                    *n1 = l;
                } else {
                    *n2 = l;
                }
            }
        }
    }

    /*
     * check for system token
     */
    if( buff[0] == '!' ) {
        strcpy( data, buff + 1 );
        *token = PCL_T_SYSTEM;
        return( ERR_NO_ERR );
    }

    /*
     * get token and data
     */
    if( NextWord( buff, tres, pkwDelims ) < 0 ) {
        return( ERR_NO_ERR );
    }
    if( !CheckAlias( tres, tmp ) ) {
        j = strlen( tmp );
        k = strlen( buff );
        for( i = k; i >= 0; i-- ) {
            buff[i + j] = buff[i];
        }
        for( i = 0; i < j; i++ ) {
            buff[i] = tmp[i];
        }
        if( NextWord( buff, tres, pDelims ) < 0 ) {
            return( ERR_NO_ERR );
        }
    }

    j = Tokenize( TokensCmdLine, tres, false );
    if( j == TOK_INVALID ) {
        j = Tokenize( TokensEx, tres, false );
        if( j != TOK_INVALID ) {
            j += 1000;
        }
    }
    *token = j;
    strcpy( data, buff );
    return( ERR_NO_ERR );

} /* ParseCommandLine */

#define NUM_STACK_SIZE  30
/*
 * GetAddress - parse to obtain line number
 */
vi_rc GetAddress( char *buff, linenum *num  )
{
    linenum     numstack[NUM_STACK_SIZE];
    char        currnum[NUM_STACK_SIZE];
    linenum     sum;
    int         numptr;
    int         k, nument, csign, numsign;
    int         i, j;
    bool        numinprog, stopnum, endparse;
    char        c;
    char        *tmp, st[2];
    int         len;
    find_type   fl;
    i_mark      pos;
    vi_rc       rc;

    /*
     * check if we have a numeric type thingy here
     */
    c = buff[0];
    if( !(c == '/' || c == '?' || c == '+' || c == '-' || c == '\'' ||
        c == '.' || c == '$' || (c >= '0' && c <= '9')) ) {
        return( NO_NUMBER );
    }
    if( c == '+' || c == '-' ) {
        sum = CurrentPos.line;
    } else {
        sum = 0;
    }
    numptr = k = nument = 0;
    csign = numsign = 1;
    numinprog = stopnum = endparse = false;

    while( !endparse ) {
        c = buff[k];
        if( c >= '0' && c <= '9' ) {
            currnum[numptr++] = c;
            numinprog = true;
        } else {
            switch( c ) {
            case '/':
            case '?':
                {
                    if( numinprog ) {
                        return( NO_NUMBER );
                    }
                    tmp = StaticAlloc();
                    st[0] = c;
                    st[1] = 0;
                    NextWord( &buff[k], tmp, st );
                    if( c == '?' ) {
                        fl = FINDFL_BACKWARDS | FINDFL_NEXTLINE;
                    } else {
                        fl = FINDFL_FORWARD | FINDFL_NEXTLINE;
                    }
                    rc = GetFind( tmp, &pos, &len, fl );
                    numstack[nument] = pos.line;
                    stopnum = true;
                    StaticFree( tmp );
                    if( rc != ERR_NO_ERR ) {
                        return( rc );
                    }
                    if( buff[k] == 0 ) {
                        k--;
                    }
                    break;
                }

            case '\'':
                if( numinprog ) {
                    return( NO_NUMBER );
                }
                j = buff[k + 1] - 'a';
                rc = VerifyMark( j + 1, true );
                if( rc != ERR_NO_ERR ) {
                    return( rc );
                }
                numstack[nument] = MarkList[j].p.line;
                stopnum = true;
                k++;
                break;
            case '+':
                csign = 1;
                if( numinprog ) {
                    stopnum = true;
                } else {
                    numsign = 1;
                }
                break;
            case '-':
                if( numinprog ) {
                    stopnum = true;
                    csign = -1;
                } else {
                    numsign = -1;
                    csign = 1;
                }
                break;
            case '.':
                if( numinprog ) {
                    return( NO_NUMBER );
                }
                numstack[nument] = CurrentPos.line;
                stopnum = true;
                break;
            case '$':
                if( numinprog ) {
                    return( NO_NUMBER );
                }
                rc = CFindLastLine( &numstack[nument] );
                if( rc != ERR_NO_ERR ) {
                    return( rc );
                }
                stopnum = true;
                break;
            default:
                endparse = true;
                if( numinprog ) {
                    stopnum = true;
                }
                break;
            }
        }
        if( !endparse ) {
            k++;
        }

        /*
         * check if a number was being scanned
         */
        if( stopnum && numinprog ) {
            currnum[numptr] = 0;
            numptr = 0;
            numstack[nument] = atol( currnum );
            numinprog = false;
        }
        if( stopnum ) {
            numstack[nument] *= numsign;
            nument++;
            numsign = csign;
            csign = 1;
            stopnum = false;
        }
    }
    EliminateFirstN( buff, k );

    for( i = 0; i < nument; i++ ) {
        sum += numstack[i];
    }

    *num = sum;

    return( ERR_NO_ERR );

} /* GetAddress */
