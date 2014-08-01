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
* Description:  Parse a makefile - targets, dependents, commands.
*
****************************************************************************/


#include <string.h>

#include "make.h"
#include "mstream.h"
#include "mlex.h"
#include "mtarget.h"
#include "macros.h"
#include "mmemory.h"
#include "mpreproc.h"
#include "mrcmsg.h"
#include "msg.h"
#include "msuffix.h"
#include "mupdate.h"
#include "mvecstr.h"
#include "mparse.h"
#include "clibext.h"


UINT16          inlineLevel;
STATIC TLIST    *firstTarget;   /* first set of targets parsed this invocation */


STATIC void ignoring( TOKEN_T t, BOOLEAN freelex )
/*************************************************
 * print message saying ignoring token, and if freelex then LexMaybeFree( t )
 */
{
    enum MsgClass y;

    switch( t ) {
    case TOK_SCOLON:    y = M_SCOLON;       break;
    case TOK_DCOLON:    y = M_DCOLON;       break;
    case TOK_FILENAME:  y = M_FILENAME;     break;
    case TOK_DOTNAME:   y = M_DOTNAME;      break;
    case TOK_CMD:       y = M_CMD;          break;
    case TOK_SUF:       y = M_SUF;          break;
    case TOK_SUFSUF:    y = M_SUFSUF;       break;
    case TOK_PATH:      y = M_PATH;         break;
    default:
#ifdef DEVELOPMENT
        PrtMsg( FTL | LOC | INVALID_TOKEN_IN, t, "ignoring" );
#else
        y = M_UNKNOWN_TOKEN;
#endif
        break;
    }

    PrtMsg( ERR | LOC | IGNORE_OUT_OF_PLACE_M, y );

    if( freelex ) {
        LexMaybeFree( t );
    }
}


STATIC TOKEN_T buildTargs( TLIST **dest, TOKEN_T t )
/***************************************************
 * Parse the input, building a TLIST on dest.  Accept TOK_FILENAME,
 * TOK_SUFSUF and TOK_DOTNAME( if IsDotWithCmds() ).  Terminate on
 * TOK_EOL, TOK_END, TOK_SCOLON, or TOK_DCOLON.  Ignore all other tokens.
 * returns: token it terminates on
 */
{
    char    dotname[MAX_DOT_NAME];

    assert( dest != NULL );

    for( ;; ) {                     /* read till SCOLON or DCOLON */
        switch( t ) {
        case TOK_SCOLON:            /* fall through */
        case TOK_DCOLON:
        case TOK_EOL:
        case TOK_END:
            return( t );
        case TOK_SUFSUF:
            if( !SufBothExist( CurAttr.u.ptr ) ) {
                PrtMsg( ERR | LOC | SUFFIX_DOESNT_EXIST, CurAttr.u.ptr );
            } else {
                char *targname = AddCreator( CurAttr.u.ptr );
                WildTList( dest, targname, TRUE, TRUE);
                FreeSafe( targname );
            }
            FreeSafe( CurAttr.u.ptr );
            break;
        case TOK_DOTNAME:
            if( !IsDotWithCmds( CurAttr.u.dotname ) ) {
                ignoring( TOK_DOTNAME, TRUE );
            } else {
                FmtStr( dotname, ".%s", DotNames[CurAttr.u.dotname] );
                WildTList( dest, dotname, TRUE, TRUE);
            }
            break;
        case TOK_FILENAME:
            WildTList( dest, CurAttr.u.ptr, TRUE, TRUE);
            FreeSafe( CurAttr.u.ptr );
            break;
        default:
            ignoring( t, TRUE );
            break;
        }

        t = LexToken( LEX_PARSER ); /* get next token */
    }
}


STATIC void setSColon( const TLIST *tlist, BOOLEAN scolon )
/**********************************************************
 * Set the targets in tlist to scolon.  Check if any are being
 * coerced to double colon, and print an error message.
 */
{
    /* set up scolon or dcolon */
    for( ; tlist != NULL; tlist = tlist->next ) {
        TARGET * const  curtarg = tlist->target;

        if( !curtarg->special ) {
            if( curtarg->depend == NULL ) {
                /* initial declaration of target */
                curtarg->scolon = scolon;
            } else if( scolon != curtarg->scolon ) {
                PrtMsg( ERR | LOC | TARGET_ALREADY_M, curtarg->node.name,
                    curtarg->scolon ? M_SCOLON : M_DCOLON );
            }
        }
    }
}


STATIC void linkDepend( const TLIST *tlist, DEPEND *dep, TATTR attr )
/********************************************************************
 * Attach a copy of dep to each target in tlist.  Also OR in the attribute
 * flags for each target.  The pointer to dep is given to the first target.
 */
{
    TARGET  *curtarg;       /* current target we're working on          */
    DEPEND  *targdep;       /* last depend for target                   */
    DEPEND  **lastnext;     /* used to track targdep                    */
    TLIST   **endlist;      /* used to find last TLIST off a DEPEND     */
    DEPEND  *tmpdep;

    for( ; tlist != NULL; tlist = tlist->next ) {
        curtarg = tlist->target;            /* alias for tlist->target */

        if( curtarg->special ) {            /* handle special targets */
            if( curtarg->depend == NULL ) {
                // we want to be able to append later to the clist
                // if it is a special target
                curtarg->depend = NewDepend();  /* no tlist on these */
            } else {
                /*
                 * for before and after we need to append command to the end
                 * if there is a clist.  For sufsuf we need to be able to
                 * add suffixes of different paths so we cannot just destroy
                 * and create a new Depend
                 */

                if( !curtarg->before_after && !curtarg->sufsuf ) {
                    FreeDepend( curtarg->depend );
                    curtarg->depend = NewDepend();
                    if( curtarg->dot_default ) {
                        PrtMsg( WRN | LOC | DOT_DEFAULT_REDEFINITION );
                    }
                }
            }
        } else {
            lastnext = &curtarg->depend;
            targdep = *lastnext;
            /*
             * from here on, *lastnext == targdep; hence, we can modify
             * the link to targdep easily.
             */
            if( targdep != NULL ) {
                /* find last depend in list */
                while( targdep->next != NULL ) {
                    lastnext = &targdep->next;
                    targdep = targdep->next;
                }

                if( curtarg->scolon /* && targdep->targs != NULL 14-jul-93 AFS */ ) {
                    /* We have the case of adding more dependent files to
                     * a scolon type target.  If this happens to be an illegal
                     * attempt to attach two cmdlists to targ, then it will be
                     * caught in linkCList().
                     */
                    endlist = &targdep->targs;  /* find last dependent */
                    while( *endlist != NULL ) {
                        endlist = &(*endlist)->next;
                    }
                    tmpdep = DupDepend( dep );  /* make a copy of before we hack it up */
                    *endlist = tmpdep->targs;   /* attach new deps to end */
                    tmpdep->targs = NULL;       /* free useless dep */
                    FreeDepend( tmpdep );
                } else if( targdep->clist != NULL || targdep->targs != NULL ) {
                    /* target isn't a place holder, link new one in */
                    targdep->next = DupDepend( dep );
                } else {
                    /* target is a place holder, free it, and link  */
                    FreeDepend( targdep );
                    *lastnext = DupDepend( dep );
                }
            } else {
                *lastnext = DupDepend( dep );
            }
        }
        TargAttrOrAttr( &curtarg->attr, attr );
    }
    FreeDepend( dep );      /* we'll always make one too many copies */
}


STATIC DEPEND *buildDepend( TATTR *pattr )
/*****************************************
 * Parse the next sequence of TOK_FILENAMEs, and .PREC/.SYMB/...
 * Build a DEPEND structure containing all the filenames.  Set *pattr
 * accordingly.  Always returns non-null DEPEND *.
 */
{
    DEPEND  *dep;
    TLIST   **list;
    TOKEN_T t;

    dep = NewDepend();
    *pattr = FalseAttr;
    list = &dep->targs;

    for( ;; ) {
        t = LexToken( LEX_PARSER );

        if( t == TOK_EOL || t == TOK_END ) {
            break;
        }

        switch( t ) {
        case TOK_DOTNAME:
            switch( CurAttr.u.dotname ) {
            case DOT_ALWAYS:
                pattr->always = TRUE;
                break;
            case DOT_AUTO_DEPEND:
                pattr->auto_dep = TRUE;
                break;
            case DOT_PRECIOUS:
                pattr->precious = TRUE;
                break;
            case DOT_MULTIPLE:
                pattr->multi= TRUE;
                break;
            case DOT_PROCEDURE:
                pattr->multi= TRUE;
                // fall through
            case DOT_SYMBOLIC:
                pattr->symbolic = TRUE;
                break;
            case DOT_EXPLICIT:
                pattr->explicit = TRUE;
                break;
            case DOT_EXISTSONLY:
                pattr->existsonly = TRUE;
                break;
            case DOT_RECHECK:
                pattr->recheck = TRUE;
                break;
            default:
                ignoring( TOK_DOTNAME, TRUE );
                break;
            }
            break;
        case TOK_FILENAME:
            WildTList( list, CurAttr.u.ptr, TRUE, FALSE );
            FreeSafe( CurAttr.u.ptr );        /* not needed any more */
            while( *list != NULL ) {        /* find tail again */
                list = &(*list)->next;
            }
            break;
        default:
            ignoring( t, TRUE );
            break;
        }
    }

    return( dep );
}


STATIC void checkFirstTarget( void )
/**********************************/
{
    TLIST  *head;
    TLIST  *current;
    TLIST  *okayList;    /* List containing the still valid initial targets */
    TLIST  *notOkayList; /* Tlist of targets not valid anymore */
    TLIST  *temp;

    okayList = NULL;
    notOkayList = NULL;

    head = firstTarget;
    if( head != NULL ) {
        /* Go through all the targets to see check if still OK */
        current = head;
        while( current != NULL ) {
            if( !current->target->attr.explicit && !current->target->special ) {
                /* still valid targets */
                temp = okayList;
                okayList = current;
                current = current->next;
                okayList->next = temp;
            } else {
                /* probably a '::' rule that changed to .explicit */
                temp = notOkayList;
                notOkayList = current;
                current = current->next;
                notOkayList->next = temp;
            }
        }
        FreeTList( notOkayList );
        head = NULL;

        /* Reverse the list back */
        while( okayList != NULL ) {
             temp = head;
             head = okayList;
             okayList = okayList->next;
             head->next = temp;
        }
        firstTarget = head;
    }
}

STATIC void setFirstTarget( const TLIST *tlist )
/**********************************************/
{
    /* Check to see if first targets are still valid */
    checkFirstTarget();
    if( firstTarget == NULL ) {
        firstTarget = DupTList( tlist );
        /* Check to see if any of the new targets are valid */
        checkFirstTarget();
    }

}


STATIC void parseTargWarning( const TLIST *walk )
/************************************************
 * we check if any targets are not special, and print a warning if so
 */
{
    for( ; walk != NULL; walk = walk->next ) {
        if( !walk->target->special ) {
            PrtMsg( DBG | INF | LOC | ASSUMING_SYMBOLIC, walk->target->node.name );
            break;
        }
    }
}


STATIC void parseTargDep( TOKEN_T t, TLIST **btlist )
/****************************************************
 * parse a line of form {target}+ (scolon||dcolon) {depend}*
 *                  or  {target}+
 * where:   target ::= TOK_FILENAME || TOK_SUFSUF || TOK_DOTNAME(with cmds)
 *          depend ::= TOK_FILENAME || DOT_PREC/SYMB/...
 *
 * stack targets into btlist
 */
{
    TATTR       attr;       /* hold attributes here */
    DEPEND      *dep;       /* a DEPEND that describes dependents from file */
    BOOLEAN     nodep;      /* true if no dependents from file              */

    /* get the target list, and the last token processed */
    t = buildTargs( btlist, t );
    if( *btlist == NULL ) {         /* only in error conditions */
        return;
    }

    nodep = t == TOK_EOL || t == TOK_END;  /* check if there wasn't a colon */

    /* set the scolon attribute for each of these targets */
    setSColon( *btlist, (int)(t == TOK_SCOLON || nodep) );

    if( !nodep ) {
        dep = buildDepend( &attr );
    } else {
        /* we've reached EOL already, so we'll just assume this */
        if( Glob.debug ) {
            parseTargWarning( *btlist );
        }

        dep = NewDepend();
        TargInitAttr( &attr );
        attr.symbolic = TRUE;
    }

    /* now we attach this depend to each target */
    linkDepend( *btlist, dep, attr );
    setFirstTarget( *btlist );
}


STATIC void parseExtensions( void )
/**********************************
 * parse lines of form .EXTENSIONS:
 *                  or .EXTENSIONS: {ext}+
 */
{
    TOKEN_T     t;
    BOOLEAN     any;

    for( ;; ) {
        t = LexToken( LEX_PARSER );
        if( t == TOK_EOL || t == TOK_END || t == TOK_SCOLON ) {
            break;
        }
        PrtMsg( ERR | LOC | EXPECTING_M, M_SCOLON );
        LexMaybeFree( t );
    }
    if( t == TOK_EOL || t == TOK_END ) {
        ClearSuffixes();
        return;
    }
    any = FALSE;
    for( ;; ) {
        t = LexToken( LEX_PARSER );
        if( t == TOK_EOL || t == TOK_END ) {
            break;
        }
        if( t == TOK_SUF ) {
            if( !SufExists( CurAttr.u.ptr ) ) {
                AddSuffix( CurAttr.u.ptr );   /* we lose CurAttr.u.ptr */
            /*
             * if microsoft option is set we put it in anyway don't
             * care whether or not it exists
             */
            } else if( Glob.compat_nmake || Glob.compat_unix ) {
                FreeSafe( CurAttr.u.ptr );
            } else {
                PrtMsg( ERR | LOC | REDEF_OF_SUFFIX, CurAttr.u.ptr );
                FreeSafe( CurAttr.u.ptr );
            }
            any = TRUE;
        } else {
            ignoring( t, TRUE );
        }
    }
    if( !any ) {
        ClearSuffixes();
    }
}


STATIC void parseDotName( TOKEN_T t, TLIST **btlist )
/****************************************************
 * parse any of the dotnames
 */
{
    if( IsDotWithCmds( CurAttr.u.dotname ) ) {
        parseTargDep( TOK_DOTNAME, btlist );
        return;
    }
    if( CurAttr.u.dotname == DOT_EXTENSIONS || CurAttr.u.dotname == DOT_SUFFIXES ) {
        parseExtensions();
        return;
    }
    for( ;; ) {
        switch( CurAttr.u.dotname ) {
        case DOT_BLOCK:         Glob.block = TRUE;              break;
        case DOT_CONTINUE:      Glob.cont = TRUE;               break;
        case DOT_ERASE:         Glob.erase = TRUE;              break;
        case DOT_FUZZY:         Glob.fuzzy = TRUE;              break;
        case DOT_IGNORE:        Glob.ignore = TRUE;             break;
        case DOT_HOLD:          Glob.hold = TRUE;               break;
        case DOT_NOCHECK:       Glob.nocheck = TRUE;            break;
        case DOT_OPTIMIZE:      Glob.optimize = TRUE;           break;
        case DOT_SILENT:        Glob.silent = TRUE;             break;
        case DOT_RCS_MAKE:      Glob.rcs_make = TRUE;           break;
        default:
            ignoring( TOK_DOTNAME, TRUE );
            break;
        }
        for( ;; ) {
            t = LexToken( LEX_PARSER );
            if( t == TOK_EOL || t == TOK_END ) {
                return;
            }
            if( t != TOK_DOTNAME ) {
                ignoring( t, TRUE );
            }
        }
    }
}


STATIC void linkCList( TLIST *btlist, CLIST *bclist )
/****************************************************
 * attach bclist to each target in btlist
 */
{
    CLIST               *clisthead;
    CLIST               *clistcur;
    TLIST const         *tlist;
    DEPEND * const      *walk;
    CLIST               **walkClist;
    TARGET const        *curtarg;

    assert( btlist != NULL );

    /*
     * Remember that cmds were stacked in reverse order.  Now we simply
     * do a pull, and pop onto another stack to reverse them.
     */
    clisthead = NULL;

    while( bclist != NULL ) {
        clistcur = bclist;
        bclist = bclist->next;

        clistcur->next = clisthead;
        clisthead = clistcur;
    }

    /*
     * Attach cmds to the targets.  Assumes that for each of the targets,
     * the last depend in the list target->depend is the owner of this clist.
     */
    for( tlist = btlist; tlist != NULL; tlist = tlist->next ) {
        curtarg = tlist->target;

        assert( curtarg->depend != NULL );  /* linkDepend() assures this */
        walk = &curtarg->depend;

        if( curtarg->scolon ) {
            /* check if it's an scolon, and attempt more than one clist */
            if( clisthead != NULL && (*walk)->clist != NULL ) {
                if( Glob.compat_nmake || Glob.compat_unix ) {
                    PrtMsg( WRN | LOC | MORE_THAN_ONE_CLIST, curtarg->node.name );
                } else {
                    PrtMsg( ERR | LOC | MORE_THAN_ONE_CLIST, curtarg->node.name );
                }
            } else if( clisthead != NULL ) {
                /* this is the first clist for this scolon */
                (*walk)->clist = DupCList( clisthead );
            }
        } else if( curtarg->sufsuf ) {
            /* special processing is needed for sufsuf */
            curtarg->depend->clist = DupCList( clisthead );
        } else {
            /* we walk the dependents to find the last one */
            while( (*walk)->next != NULL ) {
                walk = &(*walk)->next;
            }

            if( curtarg->before_after ) {
                walkClist = &(*walk)->clist;
                /* if the clist being appended is null then clear the list */
                /* just like .SUFFIXES                                     */
                if( clisthead != NULL ) {
                    // for .AFTER and .BEFORE we need to add to the end of the
                    // clist the new clists
                    while( (*walkClist) != NULL ) {
                        walkClist = &(*walkClist)->next;
                    }
                    *walkClist = DupCList( clisthead );
                } else {
                    FreeCList( *walkClist );
                    *walkClist = NULL;
                }
            } else {
                (*walk)->clist = DupCList( clisthead );
            }
        }
    }

    FreeCList( clisthead ); /* we always make an extra copy */
    FreeTList( btlist );    /* free the tlist we were passed */
}


STATIC void parseSuf( void )
/***************************
 * parse {TOK_SUF}+ : TOK_PATH
 *   or  {TOK_SUF}+ :
 *   or  {TOK_SUF}+
 */
{
    TOKEN_T t;
    char    *path;
    NODE    *head;
    NODE    *cur;

    head = NULL;

    t = TOK_SUF;

    for( ;; ) {
        if( t == TOK_EOL || t == TOK_END || t == TOK_SCOLON ) {
            break;
        }
        if( t == TOK_SUF ) {
            if( !SufExists( CurAttr.u.ptr ) ) {
                PrtMsg( ERR | LOC | SUFFIX_DOESNT_EXIST, CurAttr.u.ptr );
                FreeSafe( CurAttr.u.ptr );
            } else {
                cur = MallocSafe( sizeof( *cur ) );
                cur->name = CurAttr.u.ptr;
                cur->next = head;
                head = cur;
            }
        } else {
            ignoring( t, TRUE );
        }
        t = LexToken( LEX_PARSER );
    }

                /* note that we use mode LEX_PATH here NOT LEX_PARSER! */
    if( t == TOK_END || t == TOK_EOL ) {
        path = NULL;
    } else {
        t = LexToken( LEX_PATH );
        assert( t == TOK_PATH || t == TOK_END || t == TOK_EOL );
        if( t == TOK_END || t == TOK_EOL ) {
            path = NULL;
        } else {
            path = CurAttr.u.ptr;
            t = LexToken( LEX_PATH ); /* prefetch next token */
        }
    }

    for( ;; ) {
        if( t == TOK_END || t == TOK_EOL ) {
            break;
        }
        ignoring( t, TRUE );
        t = LexToken( LEX_PATH );
    }

    while( head != NULL ) {
        cur = head;
        head = head->next;
        SetSufPath( cur->name, path );
        FreeSafe( cur->name );
        FreeSafe( cur );
    }
    if( path != NULL ) {
        FreeSafe( path );               /* from CurAttr.u.ptr */
    }
}


STATIC char *getFileName( const char *intext, size_t *offset )
/*************************************************************
 * get the filename from the given text
 * if there is no file name then get the text body
 * offset - how long after the << is the file name
 * this only gets the explicit fileName
 * for the fileNames that need to be generated
 * we do it later but still we create an FLIST for
 * the temp file anyway
 */
{
    VECSTR      tempStr;
    char        *ret;
    BOOLEAN     doubleQuote;    //are there double quotes

    assert( intext != NULL && offset != NULL );

    *offset     = 0;
    doubleQuote = FALSE;

    if( intext[*offset] == DOUBLEQUOTE ) {
        doubleQuote = TRUE;
        *offset     = 1;
    }
    for( ;; ) {
        if( intext[*offset] == NULLCHAR ) {
            break;
        } else if( (isws( intext[*offset] )        ||
                    intext[*offset]== LESSTHAN     ||
                    intext[*offset]== GREATERTHAN) &&
                    !doubleQuote ) {
            break;
        } else if( doubleQuote && intext[*offset] == BACKSLASH ) {
            if( intext[*offset + 1] == DOUBLEQUOTE ) {
                *offset = *offset + 1;
            }
        } else if( doubleQuote && intext[*offset] == DOUBLEQUOTE ) {
            ++(*offset);
            break;
        }
        ++(*offset);
    }

    if( (intext[(*offset) - 1] != DOUBLEQUOTE && doubleQuote) ||
        (*offset == 1                         && doubleQuote) ) {
        /* error */
        PrtMsg( ERR | LOC | NON_MATCHING_QUOTE );
        ret = NULL;
        return( ret );
    }

    if( ( !doubleQuote && *offset > 0 ) ||
        ( doubleQuote && *offset > 1 ) ) {
        tempStr = StartVec();
        if( doubleQuote ) {
            WriteNVec( tempStr, intext + 1, *offset - 2 );
        } else {
            WriteNVec( tempStr, intext, *offset );
        }
        ret = FinishVec( tempStr );
        return( ret );
    } else {
        // if we need a temporary file then give back << as a filename
        tempStr = StartVec();
        WriteVec( tempStr, INLINE_SYMBOL );
        ret = FinishVec( tempStr );
        return( ret );
    }
}


STATIC void getBody( FLIST *head )
/*********************************
 * get the body of the file from the input stream
 */
{
    FLIST       *current;
    STRM_T      s;
    VECSTR      buf;
    VECSTR      bufTemp;
    char        *temp;
    char const  *currChar;

    current = head;
    if( current == NULL ) {
        return;
    }

    /* Inlinelevel == the number of inline files we need to create */
    while( inlineLevel > 0 && current != NULL ) {
        buf = StartVec();
        WriteVec( buf, "" );
        for( ;; ) {
            s = PreGetCH();
            if( s == STRM_END ) {
                UnGetCH( s );
                PrtMsg( ERR | LOC | UNEXPECTED_EOF );
                return;
            }
            UnGetCH( s );
            temp = ignoreWSDeMacro( TRUE, ForceDeMacro() );
            if( temp[0] == LESSTHAN ) {
                if( temp[1] == LESSTHAN ) {
                    /* terminator of inline file is found when first
                     * two characters are <<
                     */
                     currChar = temp + 2;
                     while( *currChar != NULLCHAR && isws( *currChar ) ) {
                         ++currChar;
                     }
                     if( *currChar == NULLCHAR ) {
                         current->keep = FALSE;
                     } else if( strnicmp( currChar, NOKEEP, 6 ) == 0 ) {
                         current->keep = FALSE;
                     } else if( strnicmp( currChar, KEEP, 4 ) == 0 ) {
                         current->keep = TRUE;
                     } else {
                         /* error only expecting (NO)KEEP */
                         PrtMsg( ERR | LOC | NOKEEP_ONLY );
                     }

                     FreeSafe( temp );
                     break;
                }
            }
            bufTemp = StartVec();
            WriteVec( bufTemp, temp );
            FreeSafe( temp );
            CatVec( buf, bufTemp );
            bufTemp = StartVec();
            WriteVec( bufTemp, "\n" );
            CatVec( buf, bufTemp );
        }
        current->body = FinishVec( buf );
        current       = current->next;
        --inlineLevel;
    }
    if( inlineLevel > 0 ) {
        PrtMsg( ERR | LOC | UNEXPECTED_EOF );
    }
}


STATIC FLIST *GetInlineFile( char **commandIn )
/**********************************************
 * this returns the head of the flist and modifies the command text to
 * change << to the actual file name to be used
 * also returns an offset so that the calling program would know
 * how long the command is in cmdText
 * cmdText is also modified it removes the file name information and
 * substitutes it with the actual file name that is going to be used
 */
{
    size_t  offset;
    size_t  index;
    FLIST   *head;
    FLIST   *current;
    char    *cmdText;
    size_t  start;       // start of cmdText to copy into newCommand
    VECSTR  newCommand;  // stores the new command built by replacing << with
                         // the actual file name

    /* inlinelevel must be initially zero when processing */
    assert( inlineLevel == 0 && commandIn != NULL );

    cmdText = *commandIn;
    head    = NULL;
    current = NULL;

    newCommand = StartVec();

    start = 0;
    /*
     * check for << in the cmdText if found then substitute this
     * with a new name the syntax is <<[filename] it is delimited with
     * a space/tab or <
     * note: the << cannot be part of any macro or else it would not
     * be recognized
     * this part will remove the << from the command text if the inline file
     * is explicitly defined
     */
    for( index = 0; cmdText[index] != NULLCHAR; ++index ) {
        if( cmdText[index] == LESSTHAN ) {
            if( cmdText[index + 1] == LESSTHAN ) {
                // Add the current vector into the new command
                WriteNVec( newCommand, cmdText + start, index - start );

                ++inlineLevel;
                if( head == NULL ) {
                    current = NewFList();
                    head    = current;
                } else {
                    assert( current != NULL );
                    current->next = NewFList();
                    current       = current->next;
                }

                // Add file Name into new command text
                current->fileName = getFileName( cmdText + 2 + index, &offset );

                // Check for long file name
                if( *(cmdText + 2 + index) == DOUBLEQUOTE ) {
                    WriteVec( newCommand, "\"" );
                    WriteVec( newCommand, current->fileName );
                    WriteVec( newCommand, "\"" );
                } else {
                    WriteVec( newCommand, current->fileName );
                }

                current->body = NULL;
                current->next = NULL;
                index = index + offset;
                start = index + 2;
            }
        }
    }
    WriteNVec( newCommand, cmdText + start, index - start );
    FreeSafe( cmdText );
    *commandIn = FinishVec( newCommand );
    getBody( head );
    return( head );
}

TLIST *Parse( void )
/*******************
 * Call LexToken, and dispatch to appropriate routine
 */
{
    TOKEN_T t;
    CLIST   *newclist;
    CLIST   *bclist;
    TLIST   *btlist;
    BOOLEAN clist_warning_given;
    BOOLEAN token_filename;

    firstTarget = NULL;

    bclist = NULL;
    btlist = NULL;

    clist_warning_given = FALSE;
    token_filename      = FALSE;

    for( ;; ) {
        /*
         * If the current target is a sufsuf then
         * we must demacro the text later
         * when the actual filenames are resolved
         */
        if( btlist != NULL ) {
            ImplicitDeMacro = (Glob.compat_nmake || Glob.compat_unix) && btlist->target->sufsuf;
        }
        t = LexToken( LEX_PARSER );
        ImplicitDeMacro = FALSE;

        if( t != TOK_CMD && t != TOK_EOL ) {
            if( btlist != NULL ) {
                /* link the commands to the targets */
                linkCList( btlist, bclist );
                bclist = NULL;
                btlist = NULL;
                if( (Glob.compat_nmake || Glob.compat_unix) && token_filename == TRUE ) {
                    exPop();
                    token_filename = FALSE;
                }
            }
            clist_warning_given = FALSE;
        }

        if( t == TOK_END ) {
            break;
        }

        switch( t ) {
        case TOK_EOL:
            break;
        case TOK_CMD:
            if( *CurAttr.u.ptr == NULLCHAR ) {
                /* discard blank lines */
                FreeSafe( CurAttr.u.ptr );
            } else {
                if( btlist == NULL ) {
                    if( !clist_warning_given ) {
                        PrtMsg( WRN | LOC | CLIST_HAS_NO_OWNER );
                        clist_warning_given = TRUE;
                    }
                    FreeSafe( CurAttr.u.ptr );
                } else {
                    newclist = NewCList();
                    newclist->text       = CurAttr.u.ptr;
                    /* stack in reverse order */
                    newclist->next       = bclist;
                    /* look at the command text to see if we need
                     * to get inline file if we do then create inline
                     * file information in memory
                     * note: that if the clist pertains to a implicit
                     *       rule we must deMacro the text the same
                     *       way as wmake does for microsoft option
                     */
                    ImplicitDeMacro = (Glob.compat_nmake || Glob.compat_unix) && btlist->target->sufsuf;
                    newclist->inlineHead = GetInlineFile( &(newclist->text) );
                    ImplicitDeMacro = FALSE;
                    bclist = newclist;
                }
            }
            break;
        case TOK_SUF:
            parseSuf();
            break;
        case TOK_SUFSUF:
            parseTargDep( t, &btlist );
            break;
        case TOK_FILENAME:
            parseTargDep( t, &btlist );
            if( (Glob.compat_nmake || Glob.compat_unix) && btlist != NULL ) {
                if( btlist->target != NULL && btlist->target->depend != NULL ) {
                    exPush( btlist->target, btlist->target->depend, NULL );
                    token_filename = TRUE;
                }
            }
            break;
        case TOK_DOTNAME:
            parseDotName( t, &btlist );
            break;
        default:
#ifdef DEVELOPMENT
            PrtMsg( FTL | INVALID_TOKEN_IN, "Parse" );
#else
            PrtMsg( WRN | LOC | IGNORE_OUT_OF_PLACE_M, M_UNKNOWN_TOKEN );
#endif
        }
    }

    return( firstTarget );
}


void ParseInit( void )
/********************/
{
    inlineLevel   = 0;
    DoingUpdate = FALSE;
}


void ParseFini( void )
/********************/
{
}
