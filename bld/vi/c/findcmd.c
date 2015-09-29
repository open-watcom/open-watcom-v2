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
#include "rxsupp.h"
#include "win.h"

static char     *lastFind = NULL;
static char     *sStr = NULL;
#ifdef __WIN__
static char     *lastFindStr = NULL;
static bool     lastFindWasRegExp;
static bool     lastFindWasCaseIgnore;
static bool     lastFindWasForward;
static bool     lastFindWasWrap;
#endif

static i_mark   lastPos = { 0, 0 };
static i_mark   currPos = { 0, 0 };
static info     *lastPosInfo = NULL;

static vi_rc    setLineCol( char *, i_mark *, find_type );
static vi_rc    processFind( range *, char *, vi_rc (*)( char *, i_mark *, int * ) );

void FindCmdFini( void )
{
    MemFree( lastFind );
    MemFree( sStr );
#ifdef __WIN__
    MemFree( lastFindStr );
#endif
}


/*
 * HilightSearchString - bring a search string into view and hilight it
 */
void HilightSearchString( i_mark *pos, int slen )
{
    if( slen > 0 ) {
        GoToColumnOK( pos->column + slen );
    }
    GoToColumnOK( pos->column + 1 );
    if( slen > 0 ) {
#ifdef __WIN__
        SetSelRegionCols( pos->line, pos->column + 1, pos->column + slen );
        DCUpdate();
#else
        DCUpdate();
        HiliteAColumnRange( pos->line, pos->column, pos->column + slen - 1 );
#endif
    }
    EditFlags.ResetDisplayLine = true;

} /* HilightSearchString */

/*
 * ResetLastFind - set so it is as if no last find was entered
 */
void ResetLastFind( info *inf )
{
    if( lastPosInfo != inf ) {
        lastPos.line = 0;
        lastPos.column = 0;
        lastPosInfo = NULL;
    }

} /* ResetLastFind */


/*
 * GetFindForward - get position of forward find string
 */
static vi_rc GetFindForward( char *st, i_mark *pos1, int *len1 )
{
    find_type   flags;

    flags = FINDFL_FORWARD;
    if( EditFlags.SearchWrap )
        flags |= FINDFL_WRAP;
    if( EditFlags.NoReplaceSearchString )
        flags |= FINDFL_NOCHANGE;
    return( GetFind( st, pos1, len1, flags ) );

} /* GetFindForward */

/*
 * GetFindBackwards - get backwards find position
 */
static vi_rc GetFindBackwards( char *st, i_mark *pos1, int *len1 )
{
    find_type   flags;

    flags = FINDFL_BACKWARDS;
    if( EditFlags.SearchWrap )
        flags |= FINDFL_WRAP;
    if( EditFlags.NoReplaceSearchString )
        flags |= FINDFL_NOCHANGE;
    return( GetFind( st, pos1, len1, flags ) );

} /* GetFindBackwards */

/*
 * getFindString - get string and search for it
 */
static vi_rc getFindString( range *r, bool is_forward, bool is_fancy, bool search_again )
{
    vi_rc       rc;
    char        st[MAX_INPUT_LINE + 1];
    char        *res;
    char        *prompt;
#ifdef __WIN__
    bool        old_ci;
    bool        old_sw;
    bool        old_no;
    fancy_find  ff;
#endif

    is_fancy = is_fancy;
    search_again = search_again;

#ifdef __WIN__
    old_ci = EditFlags.CaseIgnore;
    old_sw = EditFlags.SearchWrap;
    old_no = EditFlags.NoReplaceSearchString;
    if( is_fancy ) {
        if( lastFindStr != NULL ) {
            strcpy( st, lastFindStr );
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
                return( RANGE_REQUEST_CANCELLED );
            }
        } else {
            st[0] = 0;
            EditFlags.NoReplaceSearchString = true;
        }
        is_forward = ff.search_forward;
        EditFlags.CaseIgnore = ff.case_ignore;
        EditFlags.SearchWrap = ff.search_wrap;
        if( !ff.use_regexp ) {
            /* we need to add the string without any changes */
            if( !EditFlags.NoReplaceSearchString ) {
                ReplaceString( &lastFindStr, st );
                lastFindWasRegExp = false;
            }
            MakeExpressionNonRegular( st );
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
        rc = PromptForString( prompt, st + 1, sizeof( st ) - 1, &EditVars.FindHist );
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
vi_rc DoFindForward( range *r, long count )
{
    vi_rc   rc;

    count = count;
    rc = getFindString( r, true, false, false );
    return( rc );

} /* DoFindForward */

/*
 * DoFindBackwards - get string and search for it
 */
vi_rc DoFindBackwards( range *r, long count )
{
    vi_rc   rc;

    count = count;
    rc = getFindString( r, false, false, false );
    return( rc );

} /* DoFindBackwards */

static void defaultRange( range *r )
{
    r->start = CurrentPos;
    r->end = CurrentPos;
    r->line_based = false;
    r->highlight = false;
    r->fix_range = false;
}

void JumpTo( i_mark *pos )
{
    if( CurrentPos.line != pos->line ) {
        GoToLineNoRelCurs( pos->line );
    }
    if( CurrentPos.column != pos->column ) {
        GoToColumnOK( pos->column );
    }
}

/*
 * FancyDoFindMisc - an EVENT_MISC version of below
 */
vi_rc FancyDoFindMisc( void )
{
    range   r;
    vi_rc   rc;

    if( CurrentFile == NULL ) {
        // you cant search if theres no file!
        return( ERR_NO_FILE );
    }
    defaultRange( &r );
    rc = FancyDoFind( &r, 1L );
    if( rc != RANGE_REQUEST_CANCELLED ) {
        JumpTo( &r.start );
    }

    return( rc );

} /* FancyDoFindMisc */

/*
 * FancyDoFind - get string and search for it
 */
vi_rc FancyDoFind( range *r, long count )
{
    vi_rc   rc;

    count = count;
    if( CurrentFile == NULL ) {
        // you cant search if theres no file!
        return( ERR_NO_FILE );
    }
    rc = getFindString( r, true, true, false );
    return( rc );

} /* FancyDoFind */

/*
 * DoNextFindForward - search again, based on last string
 */
vi_rc DoNextFindForward( range *r, long count )
{
    char        st = 0;

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
vi_rc DoNextFindBackwards( range *r, long count )
{
    char        st = 0;

    count = count;
    if( !EditFlags.LastSearchWasForward ) {
        return( processFind( r, &st, GetFindForward ) );
    } else {
        return( processFind( r, &st, GetFindBackwards ) );
    }

} /* DoNextFindBackwards */


/*
 * DoNextFindForwardMisc - search again, based on last string (EVENT_MISC)
 */
vi_rc DoNextFindForwardMisc( void )
{
    range       r;
    vi_rc       rc;

    if( CurrentFile == NULL ) {
        // you cant search if theres no file!
        return( ERR_NO_FILE );
    }
    defaultRange( &r );
    rc = getFindString( &r, true, true, true );
    JumpTo( &r.start );

    return( rc );

} /* DoNextFindForwardMisc */

/*
 * DoNextFindBackwardsMisc - search again, based on last string (EVENT_MISC)
 */
vi_rc DoNextFindBackwardsMisc( void )
{
    range       r;
    vi_rc       rc;

    if( CurrentFile == NULL ) {
        // you cant search if theres no file!
        return( ERR_NO_FILE );
    }
    defaultRange( &r );
    rc = getFindString( &r, false, true, true );
    JumpTo( &r.start );

    return( rc );

} /* DoNextFindBackwardsMisc */

/*
 * processFind - set up and do forward find
 */
static vi_rc processFind( range *r, char *st, vi_rc (*rtn)( char *, i_mark *, int * ) )
{
    int         len;
    i_mark      pos;
    vi_rc       rc;

    rc = rtn( st, &pos, &len );
    if( rc == ERR_NO_ERR ) {
        if( EditFlags.Modeless ) {
            /* select region
            */
            r->line_based = false;
            r->start = pos;
            r->end = pos;
            r->end.column += len - 1;
            SetSelectedRegionFromLine( r, pos.line );
        } else {
            r->line_based = false;

            r->start = pos;

            if( rtn == &GetFindBackwards ) {
                r->end.column -= 2;
            } else {
                r->fix_range = true;    /* fix off by 1 error */
            }

            /* highlight region hack
            */
            r->highlight = true;
            r->hi_start = pos;
            r->hi_end = pos;
            r->hi_end.column += len - 1;
#if 0
// This does not work if last char is end of line
#ifdef __WIN__
            HilightSearchString( &pos, len );
#endif
#endif
        }
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
vi_rc GetFind( char *st, i_mark *pos1, int *len1, find_type flags )
{
    int         len;
    char        *linedata = NULL;
    i_mark      pos2;
    vi_rc       rc;

    /*
     * do find
     */
    if( CurrentFile == NULL ) {
        return( ERR_NO_FILE );
    }
    rc = setLineCol( st, &pos2, flags );
    if( rc == ERR_NO_ERR ) {
        if( flags & FINDFL_FORWARD ) {
            rc = FindRegularExpression( sStr, &pos2, &linedata, MAX_LONG, flags );
        } else {
            rc = FindRegularExpressionBackwards( sStr, &pos2, &linedata, -1, flags );
        }
    }
    lastPosInfo = CurrentInfo;
    
    /*
     * process results
     */
    if( rc == ERR_NO_ERR ) {
        if( linedata[pos2.column] == 0 ) {
            pos2.column--;
        }
        len = GetCurrRegExpLength();
        lastPos = pos2;
        *pos1 = pos2;
        *len1 = len;

    } else {

        if( rc == ERR_FIND_NOT_FOUND || rc == ERR_FIND_END_OF_FILE ||
            rc == ERR_FIND_TOP_OF_FILE ) {
            if( !(flags & FINDFL_NOERROR) ) {
                Error( GetErrorMsg( rc ), sStr );
                rc = DO_NOT_CLEAR_MESSAGE_WINDOW;
            }
            if( flags & FINDFL_FORWARD ) {
                lastPos.column -= 1;
            } else {
                lastPos.column += 1;
            }
            lastPos.line = CurrentPos.line;
        }

    }
    return( rc );

} /* GetFind */


/*
 * setLineCol - set up line and column to start search at
 */
static vi_rc setLineCol( char *st, i_mark *pos, find_type flags )
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
        if( lastPos.line != 0 && currPos.column == CurrentPos.column &&
            currPos.line == CurrentPos.line ) {
            *pos = lastPos;
            if( flags & FINDFL_FORWARD ) {
                pos->column += 1;
            } else {
                pos->column -= 2;
            }
        } else {
            *pos = CurrentPos;
            if( flags & FINDFL_FORWARD ) {
                pos->column += 0;
            } else {
                pos->column -= 2;
            }
        }
        ReplaceString( &sStr, lastFind );
    } else {
        if( !(flags & FINDFL_NOCHANGE) ) {
            ReplaceString( &lastFind, st );
        }
        ReplaceString( &sStr, st );
        *pos = CurrentPos;
        if( flags & FINDFL_FORWARD ) {
            pos->column += 0;
        } else {
            pos->column -= 2;
        }
    }

    /*
     * wrap if needed
     */
    if( flags & FINDFL_NEXTLINE ) {
        wrapped = false;
        if( flags & FINDFL_FORWARD ) {
            pos->column = 0;
            pos->line += 1;
            if( IsPastLastLine( pos->line ) ) {
                pos->line = 1;
                wrapped = true;
            }
        } else {
            pos->line -= 1;
            if( pos->line == 0 ) {
                CFindLastLine( &pos->line );
                wrapped = true;
            }
            CGimmeLinePtr( pos->line, &cfcb, &cline );
            pos->column = cline->len - 1;
            if( pos->column < 0 ) {
                pos->column = 0;
            }
        }
        if( wrapped && !(flags & FINDFL_WRAP) ) {
            if( flags & FINDFL_FORWARD ) {
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
    currPos = CurrentPos;
}

/*
 * ColorFind - find string and color it
 */
vi_rc ColorFind( const char *data, find_type findfl )
{
    vi_rc       rc;
    int         len;
    char        *buff;
    i_mark      pos;

    /*
     * get search string and flags
     */
    buff = StaticAlloc();
    GetNextWord1( data, buff );
    if( *buff == '\0' ) {
        StaticFree( buff );
        return( ERR_INVALID_FIND_CMD );
    }

    /*
     * go get the match
     */
    GoToLineNoRelCurs( 1 );
    rc = GetFind( buff, &pos, &len, FINDFL_FORWARD | findfl );
    if( rc == ERR_NO_ERR ) {
        pos.column += 1;
        JumpTo( &pos );
        DCUpdate();
#ifndef __WIN__
        // Windows selects instead
        HiliteAColumnRange( pos.line, pos.column, pos.column + len - 1 );
#endif
        EditFlags.ResetDisplayLine = true;
    }
    StaticFree( buff );
    return( rc );

} /* ColorFind */

/*
 * SetLastFind - set the last find string
 */
void SetLastFind( const char *newLastFind )
{
    ReplaceString( &lastFind, newLastFind );

} /* SetLastFind */

/*
 * FancyDoReplace - get strings, search for one, replace with other
 */
vi_rc FancyDoReplace( void )
{
#ifdef __WIN__
    static char *lastReplace;
    vi_rc       rc;
    char        find[MAX_INPUT_LINE + 1], replace[MAX_INPUT_LINE + 1];
    fancy_find  ff;
    bool        is_forward = true;
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
        ff.search_wrap = lastFindWasWrap;
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
    ReplaceString( &lastReplace, replace );

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
