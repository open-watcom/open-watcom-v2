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


#include <stdio.h>
#include <string.h>
#include "vi.h"
#include "rxsupp.h"
#include "win.h"
#ifdef __WIN__
#include "winvi.h"
#endif

static char     *lastFind=NULL,*sStr=NULL;
#ifdef __WIN__
static bool     lastFindWasRegExp;
static bool     lastFindWasCaseIgnore;
static bool     lastFindWasForward;
static bool     lastFindWasWrap;
#endif

static linenum  lastLine=0,cLineNumber=0;
static int      lastCol=0,cColumn=0;

static int setLineCol( char *, linenum *, int *, int );
static int processFind( range *, char *, int (*)() );

void FindCmdFini( void ){
    MemFree( lastFind );
    MemFree( sStr );
}


/*
 * HilightSearchString - bring a search string into view and hilight it
 */
void HilightSearchString( linenum lineno, int col, int slen )
{
    if( slen > 0 ) {
        GoToColumnOK( col+slen );
    }
    GoToColumnOK( col+1 );
    if( slen > 0 ) {
#ifdef __WIN__
        SetSelRegionCols( lineno, col+1, col + slen );
        DCUpdate();
#else
        DCUpdate();
        HiliteAColumnRange( lineno, col, col+slen-1 );
#endif
    }
    EditFlags.ResetDisplayLine = TRUE;

} /* HilightSearchString */

/*
 * ResetLastFind - set so it is as if no last find was entered
 */
void ResetLastFind( void )
{
    lastLine = 0L;
    lastCol = 0;

} /* ResetLastFind */


/*
 * GetFindForward - get position of forward find string
 */
int GetFindForward( char *st, linenum *ln1, int *col1, int *len1 )
{
    return( GetFind( st, ln1, col1, len1, FINDFL_FORWARD ) );

} /* GetFindForward */

/*
 * GetFindBackwards - get backwards find position
 */
int GetFindBackwards( char *st, linenum *ln1, int *col1, int *len1 )
{
    return( GetFind( st, ln1, col1, len1, FINDFL_BACKWARDS ) );

} /* GetFindBackwards */

/*
 * getFindString - get string and search for it
 */
static int getFindString( range *r, bool is_forward, bool is_fancy, bool search_again )
{
    int         rc;
    char        st[MAX_INPUT_LINE+1];
    char        *res;
    char        *prompt;
    #ifdef __WIN__
        bool            old_ci;
        bool            old_sw;
        bool            old_no;
        fancy_find      ff;
    #endif

    is_fancy = is_fancy;
    search_again = search_again;

    #ifdef __WIN__
        old_ci = EditFlags.CaseIgnore;
        old_sw = EditFlags.SearchWrap;
        old_no = EditFlags.NoReplaceSearchString;
        if( is_fancy ) {
            if( lastFind != NULL ) {
                strcpy( st, lastFind );
                ff.use_regexp = lastFindWasRegExp;
                ff.case_ignore = lastFindWasCaseIgnore;
                ff.search_forward = is_forward;
                ff.search_wrap  = lastFindWasWrap;
            } else {
                st[0] = 0;
            }
            ff.find = st;
            ff.findlen = sizeof( st );
            if( !search_again ) {
                if( !GetFindStringDialog( &ff ) ) {
                    return( ERR_NO_ERR );
                }
            } else {
                EditFlags.NoReplaceSearchString = TRUE;
            }

            is_forward = ff.search_forward;
            EditFlags.CaseIgnore = ff.case_ignore;
            EditFlags.SearchWrap = ff.search_wrap;
            if( !ff.use_regexp ) {
                /* we need to add the string without any changes */
                if( !EditFlags.NoReplaceSearchString ) {
                    AddString2( &lastFind, st );
                    lastFindWasRegExp = FALSE;
                }
                MakeExpressionNonRegular( st );
                EditFlags.NoReplaceSearchString = TRUE;
            }
            res = st;
        } else {
    #endif
            if( is_forward ) {
                prompt = "/";
            } else {
                prompt = "?";
            }
            st[0] = prompt[0];
            rc = PromptForString( prompt, st+1, sizeof( st )-1, &FindHist );
            if( rc != ERR_NO_ERR ) {
                if( rc == NO_VALUE_ENTERED ) {
                    return( ERR_NO_ERR );
                }
                return( rc );
            }
            res = &st[1];       // skip prompt
    #ifdef __WIN__
        }
    #endif

    if( is_forward ) {
        rc = processFind( r, res, GetFindForward );
    } else {
        rc = processFind( r, res, GetFindBackwards );
    }
    #ifdef __WIN__
        EditFlags.NoReplaceSearchString = old_no;
        EditFlags.CaseIgnore = old_ci;
        EditFlags.SearchWrap = old_sw;
        lastFindWasRegExp = ff.use_regexp;
        lastFindWasCaseIgnore = ff.case_ignore;
        lastFindWasForward = ff.search_forward;
        lastFindWasWrap = ff.search_wrap;
    #endif
    EditFlags.LastSearchWasForward = is_forward;
    return( rc );
} /* getFindString */

/*
 * DoFindForward - get string and search for it
 */
int DoFindForward( range *r, long count )
{
    int rc;

    count = count;
    rc = getFindString( r, TRUE, FALSE, FALSE );
    return( rc );

} /* DoFindForward */

/*
 * DoFindBackwards - get string and search for it
 */
int DoFindBackwards( range *r, long count )
{
    int rc;

    count = count;
    rc = getFindString( r, FALSE, FALSE, FALSE );
    return( rc );

} /* DoFindBackwards */

/*
 * FancyDoFindMisc - an EVENT_MISC version of below
 */
int FancyDoFindMisc( void )
{
    range   r;
    int     rc;

    if( CurrentFile == NULL ) {
        // you cant search if theres no file!
        return( ERR_NO_FILE );
    }

    rc = FancyDoFind( &r, 1L );

    if( CurrentLineNumber != r.start.line ) {
        GoToLineNoRelCurs( r.start.line );
    }
    if( CurrentColumn != r.start.column ) {
        GoToColumnOK( r.start.column );
    }
    return( rc );
}

/*
 * FancyDoFind - get string and search for it
 */
int FancyDoFind( range *r, long count )
{
    int rc;

    count = count;
    if( CurrentFile == NULL ) {
        // you cant search if theres no file!
        return( ERR_NO_FILE );
    }
    rc = getFindString( r, TRUE, TRUE, FALSE );
    return( rc );

} /* FancyDoFind */

/*
 * DoNextFindForward - search again, based on last string
 */
int DoNextFindForward( range *r, long count )
{
    char        st=0;

    count = count;
    if( EditFlags.LastSearchWasForward ) {
        return( processFind( r, &st, GetFindForward ) );
    } else {
        return( processFind( r, &st, GetFindBackwards ) );
    }

} /* DoNextFindForward */

/*
 * DoNextFindBackwards - search again, based on last string
 */
int DoNextFindBackwards( range *r, long count )
{
    char        st=0;

    count = count;
    if( !EditFlags.LastSearchWasForward ) {
        return( processFind( r, &st, GetFindForward ) );
    } else {
        return( processFind( r, &st, GetFindBackwards ) );
    }

} /* DoNextFindBackwards */


void jumpTo( range *r )
{
    if( CurrentLineNumber != r->start.line ) {
        GoToLineNoRelCurs( r->start.line );
    }
    if( CurrentColumn != r->start.column ) {
        GoToColumnOK( r->start.column );
    }
}

/*
 * DoNextFindForwardMisc - search again, based on last string (EVENT_MISC)
 */
int DoNextFindForwardMisc( void )
{
    range       r;
    int         rc;

    if( CurrentFile == NULL ) {
        // you cant search if theres no file!
        return( ERR_NO_FILE );
    }
    rc = getFindString( &r, TRUE, TRUE, TRUE );
    jumpTo( &r );

    return( rc );

} /* DoNextFindForwardMisc */

/*
 * DoNextFindBackwardsMisc - search again, based on last string (EVENT_MISC)
 */
int DoNextFindBackwardsMisc( void )
{
    range       r;
    int         rc;

    if( CurrentFile == NULL ) {
        // you cant search if theres no file!
        return( ERR_NO_FILE );
    }
    rc = getFindString( &r, FALSE, TRUE, TRUE );
    jumpTo( &r );

    return( rc );

} /* DoNextFindBackwardsMisc */

/*
 * processFind - set up and do forward find
 */
static int processFind( range *r, char *st, int (*rtn)() )
{
    int         rc,col,len;
    linenum     lineno;

    rc = rtn( st, &lineno, &col, &len );
    if( rc == ERR_NO_ERR ) {
        if( EditFlags.Modeless ) {
            /* select region
            */
            r->line_based = FALSE;
            r->start.line = lineno;
            r->start.column = col;
            r->end.line = lineno;
            r->end.column = col + len - 1;
            SetSelectedRegionFromLine( r, lineno );
        } else {
            r->line_based = FALSE;

            r->start.line = lineno;
            r->start.column = col;

            if( rtn == &GetFindBackwards ) {
                r->end.column-=2;
            } else {
                r->fix_range = TRUE;    /* fix off by 1 error */
            }

            /* highlight region hack
            */
            r->highlight = TRUE;
            r->hi_start.line = lineno;
            r->hi_start.column = col;
            r->hi_end.line = lineno;
            r->hi_end.column = col + len - 1;
        }
        #if 0
            // This does not work if last char is end of line
            #ifdef __WIN__
            if( !EditFlags.Modeless ) {
                HilightSearchString( lineno, col, len );
            }
            #endif
        #endif
    }
    SaveFindRowColumn();

    /* make column 1-based (probably used w/ GoTo in DoMove())
    */
    r->start.column++;
    r->end.column++;
    return( rc );

} /* processFind */

/*
 * GetFind - get a find location
 */
int GetFind( char *st, linenum *ln1, int *col1, int *len1, int flag )
{
    int         rc,col,len;
    linenum     lineno;
    char        *linedata;

    /*
     * do find
     */
    if( CurrentFile == NULL ) {
        return( ERR_NO_FILE );
    }
    rc = setLineCol( st, &lineno, &col, flag );
    if( !rc ) {
        if( flag & FINDFL_FORWARD ) {
            rc = FindRegularExpression( sStr, &lineno, col,
                               &linedata, MAX_LONG, EditFlags.SearchWrap );
        } else {
            rc = FindRegularExpressionBackwards( sStr, &lineno, col,
                                &linedata, -1, EditFlags.SearchWrap );
        }
    }

    /*
     * process results
     */
    if( rc == ERR_NO_ERR ) {

        col = GetCurrRegExpColumn( linedata );
        if( linedata[col] == 0 ) {
            col--;
        }
        len = GetCurrRegExpLength();
        lastLine = lineno;
        lastCol = col;
        *ln1 = lineno;
        *col1 = col;
        *len1 = len;

    } else {

        if( rc == ERR_FIND_NOT_FOUND || rc == ERR_FIND_END_OF_FILE ||
          rc == ERR_FIND_TOP_OF_FILE ) {
            if( !(flag & FINDFL_NOERROR ) ) {
                Error( GetErrorMsg(rc), sStr );
                rc = DO_NOT_CLEAR_MESSAGE_WINDOW;
            }
            if( flag & FINDFL_FORWARD ) {
                lastCol -= 1;
            } else {
                lastCol += 1;
            }
            lastLine = CurrentLineNumber;
        }

    }
    return( rc );

} /* GetFind */


/*
 * setLineCol - set up line and column to start search at
 */
static int setLineCol( char *st, linenum *lineno, int *col, int flag )
{
    fcb         *cfcb;
    line        *cline;
    bool        wrapped;

    /*
     * get next position
     */
    if( st[0] == 0 ) {
        if( lastFind == NULL ) {
            return( ERR_NO_PREVIOUS_SEARCH_STRING );
        }
        if( lastLine != 0 && cColumn == CurrentColumn &&
            cLineNumber == CurrentLineNumber ) {
            *lineno = lastLine;
            if( flag & FINDFL_FORWARD ) {
                *col = lastCol+1;
            } else {
                *col = lastCol - 2;
            }
        } else {
            *lineno = CurrentLineNumber;
            if( flag & FINDFL_FORWARD ) {
                *col = CurrentColumn;
            } else {
                *col = CurrentColumn - 2;
            }
        }
        AddString2( &sStr, lastFind );
    } else {
        if( !EditFlags.NoReplaceSearchString ) {
            AddString2( &lastFind, st );
        }
        AddString2( &sStr, st );
        *lineno = CurrentLineNumber;
        if( flag & FINDFL_FORWARD ) {
            *col = CurrentColumn;
        } else {
            *col = CurrentColumn - 2;
        }
    }

    /*
     * wrap if needed
     */
    if( (flag & FINDFL_NEXTLINE) || (*col < 0) ||
        (CurrentLine->data[ *col ] == 0 ) ) {
        wrapped = FALSE;
        if( flag & FINDFL_FORWARD ) {
            *col = 0;
            (*lineno) += 1;
            if( IsPastLastLine( *lineno ) ) {
                *lineno = 1;
                wrapped = TRUE;
            }
        } else {
            (*lineno) -= 1;
            if( *lineno == 0 ) {
                CFindLastLine( lineno );
                wrapped = TRUE;
            }
            CGimmeLinePtr( *lineno, &cfcb, &cline );
            *col = cline->len - 1;
            if( *col < 0 ) {
                *col = 0;
            }
        }
        if( wrapped && !EditFlags.SearchWrap ) {
            if( flag & FINDFL_FORWARD ) {
                return( ERR_FIND_END_OF_FILE );
            } else {
                return( ERR_FIND_TOP_OF_FILE );
            }
        }
    }

    return( ERR_NO_ERR );

} /* setLineCol */

/*
 * SaveFindRowColumn - save row and column in find cmd
 */
void SaveFindRowColumn( void )
{
    cColumn = CurrentColumn;
    cLineNumber = CurrentLineNumber;

} /* SaveFindRowColumn */

/*
 * ColorFind - find string and color it
 */
int ColorFind( char *data, int findfl )
{
    int         rc=ERR_NO_ERR;
    int         col,len;
    linenum     s;
    char        *buff;

    /*
     * get search string and flags
     */
    buff = StaticAlloc();
    if( (len = NextWordSlash( data, buff ) ) <= 0 ) {
        StaticFree( buff );
        return( ERR_INVALID_FIND_CMD );
    }

    /*
     * go get the match
     */
    EditFlags.LastSearchWasForward = TRUE;
    GoToLineNoRelCurs( 1 );
    rc = GetFind( buff, &s, &col, &len, FINDFL_FORWARD | findfl );
    if( !rc ) {
        GoToLineNoRelCurs( s );
        GoToColumnOK( col+1 );
        DCUpdate();
        #ifndef __WIN__
            // Windows selects instead
            HiliteAColumnRange( s,  col, col+len-1 );
        #endif
        EditFlags.ResetDisplayLine = TRUE;
    }
    StaticFree( buff );
    return( rc );

} /* ColorFind */

/*
 * FancyDoReplace - get strings, search for one, replace with other
 */
int FancyDoReplace( void )
{
#ifdef __WIN__
    static char *lastReplace;
    int         rc;
    char        find[MAX_INPUT_LINE+1], replace[MAX_INPUT_LINE+1];
    fancy_find  ff;
    bool        is_forward = TRUE;
    bool        old_ci;
    bool        old_sw;

    if( CurrentFile == NULL ) {
        // you cant search if theres no file!
        return( ERR_NO_FILE );
    }

    old_ci = EditFlags.CaseIgnore;
    old_sw = EditFlags.SearchWrap;

    if( lastFind != NULL ) {
        strcpy( find, lastFind );
        ff.use_regexp = lastFindWasRegExp;
        ff.case_ignore = lastFindWasCaseIgnore;
        ff.search_forward = is_forward;
        ff.search_wrap  = lastFindWasWrap;
    } else {
        find[0] = 0;
    }
    if( lastReplace != NULL ) {
        strcpy( replace, lastReplace );
    } else {
        replace[0] = 0;
    }
    ff.find = find;
    ff.findlen = sizeof( find );
    ff.replace = replace;
    ff.replacelen = sizeof( replace );

    if( !GetReplaceStringDialog( &ff ) ) {
        return( ERR_NO_ERR );
    }
    EditFlags.CaseIgnore = ff.case_ignore;
    EditFlags.SearchWrap = ff.search_wrap;
    if( !ff.use_regexp ) {
        MakeExpressionNonRegular( find );
        // MakeExpressionNonRegular( replace );
    }
    AddString2( &lastReplace, replace );

    EditFlags.LastSearchWasForward = is_forward;

    /*NOTE: does not use is_forward (how about %s?this/that/?)
            does not use selection_only
    */
    rc = TwoPartSubstitute( find, replace, ff.prompt, ff.search_wrap );

    EditFlags.CaseIgnore = old_ci;
    EditFlags.SearchWrap = old_sw;
    lastFindWasRegExp = ff.use_regexp;
    lastFindWasCaseIgnore = ff.case_ignore;
    lastFindWasForward = ff.search_forward;
    lastFindWasWrap = ff.search_wrap;

    return( rc );
#else
    return( ERR_NO_ERR );
#endif
} /* FancyDoReplace */
