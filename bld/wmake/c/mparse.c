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


#include <string.h>

#include "macros.h"
#include "make.h"
#include "massert.h"
#include "memory.h"
#include "misc.h"
#include "mlex.h"
#include "mparse.h"
#include "mpreproc.h"
#include "mrcmsg.h"
#include "msg.h"
#include "mstream.h"
#include "msuffix.h"
#include "mtarget.h"
#include "mtypes.h"
#include "mupdate.h"
#include "mvecstr.h"


UINT16          inlineLevel;
STATIC TLIST   *firstTarget;       /* first set of targets parsed
                                           this invocation */



STATIC void ignoring( TOKEN_T t, BOOLEAN free )
/**********************************************
 * print message saying ignoring token, and if free then LexMaybeFree( t )
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
        PrtMsg( FTL|LOC| INVALID_TOKEN_IN, t, "ignoring" );
#else
        y = M_UNKNOWN_TOKEN;
#endif
        break;
    }

    PrtMsg( ERR|LOC| IGNORE_OUT_OF_PLACE_M, y );

    if( free ) {
        LexMaybeFree( y );
    }
}


STATIC TOKEN_T buildTargs( TLIST **dest, TOKEN_T t )
/***************************************************
 * Parse the input, building a TLIST on dest.  Accept TOK_FILENAME,
 * TOK_SUFSUF and TOK_DOTNAME( if IsDotWithCmds() ).  Terminate on
 * EOL, STRM_END, TOK_SCOLON, or TOK_DCOLON.  Ignore all other tokens.
 * returns: token it terminates on
 */
{
    char    dotname[ MAX_DOT_NAME ];

    assert( dest != NULL );

    for(;;) {                       /* read till SCOLON or DCOLON */
        switch( t ) {
        case TOK_SCOLON:            /* fall through */
        case TOK_DCOLON:
        case EOL:
        case STRM_END:
            return( t );
        case TOK_SUFSUF:
            if( !SufBothExist( CurAttr.ptr ) ) {
                PrtMsg( ERR|LOC| SUFFIX_DOESNT_EXIST, CurAttr.ptr );
            } else {
                AddCreator( CurAttr.ptr );
                WildTList( dest, CurAttr.ptr, TRUE , TRUE);
            }
            FreeSafe( CurAttr.ptr );
            break;
        case TOK_DOTNAME:
            if( !IsDotWithCmds( CurAttr.num ) ) {
                ignoring( TOK_DOTNAME, TRUE );
            } else {
                FmtStr( dotname, ".%s", DotNames[ CurAttr.num ] );
                WildTList( dest, dotname, TRUE , TRUE);
            }
            break;
        case TOK_FILENAME:
            WildTList( dest, CurAttr.ptr, TRUE , TRUE);
            FreeSafe( CurAttr.ptr );
            break;
        default:
            ignoring( t, TRUE );
            break;
        }

        t = LexToken( LEX_PARSER ); /* get next token */
    }
}


STATIC void setSColon( TLIST *tlist, BOOLEAN scolon )
/****************************************************
 * Set the targets in tlist to scolon.  Check if any are being
 * coerced to double colon, and print an error message.
 */
{
    TARGET  *curtarg;
    DEPEND  *curdep;
                                /* set up scolon or dcolon */
    for( ; tlist != NULL; tlist = tlist->next ) {
        curtarg = tlist->target;
        if( !curtarg->special ) {
            curdep = curtarg->depend;
            if( curtarg->depend == NULL ) {
                    /* initial declaration of target */
                curtarg->scolon = scolon;
            } else if( scolon != curtarg->scolon ) {
                PrtMsg( ERR|LOC| TARGET_ALREADY_M, curtarg->node.name,
                    curtarg->scolon ? M_SCOLON : M_DCOLON );
            }
        }
    }
}


STATIC void linkDepend( TLIST *tlist, DEPEND *dep, TATTR attr )
/**************************************************************
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
            if( !(curtarg->depend) ) {
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

                if (!curtarg->before_after && !curtarg->sufsuf) {
                    FreeDepend(curtarg->depend);
                    curtarg->depend = NewDepend();
                    if (curtarg->dot_default) {
                        PrtMsg( WRN|LOC| DOT_DEFAULT_REDEFINITION);
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
                    tmpdep = dep;   /* make a copy of before we hack it up */
                    dep = DupDepend( tmpdep );
                    *endlist = tmpdep->targs;   /* attach new deps to end */
                    tmpdep->targs = NULL;       /* free useless dep */
                    FreeDepend( tmpdep );
                } else if( targdep->clist != NULL || targdep->targs != NULL ) {
                        /* target isn't a place holder, link new one in */
                    targdep->next = dep;
                    dep = DupDepend( dep );
                } else {
                        /* target is a place holder, free it, and link  */
                    FreeDepend( targdep );
                    *lastnext = dep;
                    dep = DupDepend( dep );
                }
            } else {
                *lastnext = dep;
                dep = DupDepend( dep );
            }
        }
        TargOrAttr( curtarg, attr );
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
    BOOLEAN InitialEntry;  // This is for the MS Option where an empty
                           // dependents list corresponds to .symbolic

    dep = NewDepend();
    *pattr = FalseAttr;
    list = &dep->targs;

    InitialEntry = TRUE;

    for(;;) {
        t = LexToken( LEX_PARSER );

        if( t == EOL || t == STRM_END ) {
            break;
        }

        InitialEntry = FALSE;

        switch( t ) {
        case TOK_DOTNAME:
            switch( CurAttr.num ) {
            case DOT_ALWAYS:
                pattr->always = TRUE;
                break;
            case DOT_AUTO_DEPEND:
                pattr->auto_dep = TRUE;
                break;
            case DOT_PRECIOUS:
                pattr->prec = TRUE;
                break;
            case DOT_MULTIPLE:
                pattr->multi= TRUE;
                break;
            case DOT_PROCEDURE:
                pattr->multi= TRUE;
                // fall through
            case DOT_SYMBOLIC:
                pattr->symb = TRUE;
                break;
            case DOT_EXPLICIT:
                pattr->explicit = TRUE;
                break;
            default:
                ignoring( TOK_DOTNAME, TRUE );
                break;
            }
            break;
        case TOK_FILENAME:
            WildTList( list, CurAttr.ptr, TRUE, FALSE );
            FreeSafe( CurAttr.ptr );        /* not needed any more */
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


STATIC void checkFirstTarget ( void )
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
        /* Go through all the targets to see check if all targets are
            still okay */
        current = head;
        while (current != NULL) {
            if( !current->target->attr.explicit &&
                !current->target->special) {
                /* still valid targets */
                temp = okayList;
                okayList = current;
                current  = current->next;
                okayList->next = temp;
            } else {
                /* probably a '::' rule that changed to .explicit */
                temp = notOkayList;
                notOkayList = current;
                current  = current->next;
                notOkayList->next = temp;
            }
        }
        FreeTList(notOkayList);
        head = NULL;

        /* Reverse the list back */
        while (okayList != NULL) {
             temp = head;
             head = okayList;
             okayList = okayList->next;
             head->next = temp;
        }
        firstTarget = head;

    }
}

STATIC void setFirstTarget( TLIST *tlist )
/****************************************/
{
    /* Check to see if first targets are still valid */
    checkFirstTarget();
    if (firstTarget == NULL) {
        firstTarget = DupTList( tlist );
        /* Check to see if any of the new targets are valid */
        checkFirstTarget();
    }

}


STATIC void parseTargWarning( TLIST *walk )
/******************************************
 * we check if any targets are not special, and print a warning if so
 */
{
    while( walk != NULL ) {
        if( !walk->target->special ) {
            PrtMsg( DBG|WRN|LOC| ASSUMING_SYMBOLIC,
                DotNames[ DOT_SYMBOLIC ] );
            break;
        }
        walk = walk->next;
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

    nodep = t == EOL || t == STRM_END;  /* check if there wasn't a colon */

        /* set the scolon attribute for each of these targets */
    setSColon( *btlist, t == TOK_SCOLON || nodep );

    if( !nodep ) {
        dep = buildDepend( &attr );
    } else {
            /* we've reached EOL already, so we'll just assume this */
        if( Glob.debug ) {
            parseTargWarning( *btlist );
        }

        dep = NewDepend();
        TargInitAttr( &attr );
        attr.symb = TRUE;
    }

        /* now we attach this depend into each target */
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

    for(;;) {
        t = LexToken( LEX_PARSER );
        if( t == EOL || t == STRM_END || t == TOK_SCOLON ) break;
        PrtMsg( ERR|LOC| EXPECTING_M, M_SCOLON );
        LexMaybeFree( t );
    }
    if( t == EOL || t == STRM_END ) {
        ClearSuffixes();
        return;
    }
    any = FALSE;
    for(;;) {
        t = LexToken( LEX_PARSER );
        if( t == EOL || t == STRM_END ) break;
        if( t == TOK_SUF ) {
            if( !SufExists( CurAttr.ptr ) ) {
                AddSuffix( CurAttr.ptr );   /* we lose CurAttr.ptr */
            /*
             * if microsoft option is set we put it in anyway don't
             * care whether or not it exists
             */
            } else if (Glob.microsoft) {
                FreeSafe( CurAttr.ptr );
            } else {
                PrtMsg( ERR|LOC| REDEF_OF_SUFFIX, CurAttr.ptr );
                FreeSafe( CurAttr.ptr );
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
    if( IsDotWithCmds( CurAttr.num ) ) {
            parseTargDep( TOK_DOTNAME, btlist );
            return;
    }
    if( CurAttr.num == DOT_EXTENSIONS || CurAttr.num == DOT_SUFFIXES ) {
        parseExtensions();
        return;
    }
    for(;;) {
        switch( CurAttr.num ) {
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
        case DOT_KEEP_SPACES:   Glob.keep_spaces = TRUE;        break;
        default:
            ignoring( TOK_DOTNAME, TRUE );
            break;
        }
        for(;;) {
            t = LexToken( LEX_PARSER );
            if( t == EOL || t == STRM_END ) return;
            if( t != TOK_DOTNAME ) {
                ignoring( t, TRUE );
            }
        }
    }
}


/* links the clist to the sufsuf target */
STATIC void linkClistSufsuf (TARGET *curtarg,
                             CLIST  *clist,
                             char   *cur_target_path,
                             char   *cur_depend_path) {

    DEPEND *walk;
    SLIST  *slist;

    assert(curtarg != NULL && curtarg->depend != NULL);

    walk = curtarg->depend;
    while (walk->next != NULL) {
        walk = walk->next;
    }


    /*
     * if both the dep_path and the target path is null
     * then we are doing the trivial case of just replacing the
     * current clist if not we have to create an SLIST that contains
     * both the new targ_path and  new dep_path
     */
    if (cur_depend_path == NULL && cur_target_path == NULL ) {
        if (walk->clist != NULL ){
            FreeSafe ( walk->clist );
        }

        walk->clist = DupCList( clist );

    } else {

        if (walk->slist == NULL ) {
            walk->slist = NewSList ();
            walk->slist->targ_path = StrDupSafe( cur_target_path );
            walk->slist->dep_path  = StrDupSafe( cur_depend_path );
            walk->slist->clist     = DupCList ( clist );
            walk->slist->next      = NULL;
        } else {
            slist = NewSList ();
            slist->targ_path = StrDupSafe( cur_target_path );
            slist->dep_path  = StrDupSafe( cur_depend_path );
            slist->clist     = DupCList ( clist );
            slist->next      = walk->slist;
            walk->slist      = slist;
        }

    }

}

STATIC void linkCList( TLIST *btlist, CLIST *bclist ,
                       char  *cur_target_path, char *cur_depend_path)
/****************************************************
 * attach bclist to each target in btlist
 */
{
    CLIST   *clisthead;
    CLIST   *clistcur;
    TLIST   *tlist;
    DEPEND  **walk;
    CLIST   **walkClist;
    TARGET  *curtarg;

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
    tlist = btlist;
    while( tlist != NULL ) {
        curtarg = tlist->target;
        tlist = tlist->next;            /* advance to next in tlist */

        assert( curtarg->depend != NULL );  /* linkDepend() assures this */
        walk = &curtarg->depend;

        if( curtarg->scolon ) {
            /* check if it's an scolon, and attempt more than one clist */
            if( clisthead != NULL && (*walk)->clist != NULL ) {
                if (Glob.microsoft) {
                    PrtMsg( WRN|LOC| MORE_THAN_ONE_CLIST, curtarg->node.name );
                } else {
                    PrtMsg( ERR|LOC| MORE_THAN_ONE_CLIST, curtarg->node.name );
                }
            } else if( clisthead != NULL ) {
                /* this is the first clist for this scolon */
                (*walk)->clist = clisthead;
                clisthead = DupCList( clisthead );
            }
        } else if ( curtarg->sufsuf ) {
            /* special processing is needed for sufsuf */
            linkClistSufsuf(curtarg,
                            clisthead,
                            cur_target_path,
                            cur_depend_path);

        } else {
            /* we walk the dependents to find the last one */
            while( (*walk)->next != NULL ) {
                walk = &(*walk)->next;
            }

            if (curtarg->before_after) {
                walkClist = &(*walk)->clist;
                /* if the clist being appended is null then clear the list */
                /* just like .SUFFIXES                                     */
                if (clisthead != NULL) {
                    // for .AFTER and .BEFORE we need to add to the end of the
                    // clist the new clists
                    while ( (*walkClist) != NULL) {
                        walkClist = &(*walkClist)->next;
                    }
                    *walkClist = clisthead;
                } else {
                    FreeCList(*walkClist);
                    *walkClist = NULL;
                }
            } else {
                (*walk)->clist = clisthead;
            }
            clisthead = DupCList( clisthead );
        }
    }

    FreeCList( clisthead ); /* we always make an extra copy */
    FreeTList( btlist );    /* free the tlist we were passed */
}


STATIC void parseSuf( void )
/****************************
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

    for(;;) {
        if( t == EOL || t == STRM_END || t == TOK_SCOLON ) break;
        if( t == TOK_SUF ) {
            if( !SufExists( CurAttr.ptr ) ) {
                PrtMsg( ERR|LOC| SUFFIX_DOESNT_EXIST, CurAttr.ptr );
                FreeSafe( CurAttr.ptr );
            } else {
                cur = MallocSafe( sizeof( *cur ) );
                cur->name = CurAttr.ptr;
                cur->next = head;
                head = cur;
            }
        } else {
            ignoring( t, TRUE );
        }
        t = LexToken( LEX_PARSER );
    }

                /* note that we use mode LEX_PATH here NOT LEX_PARSER! */
    if( t == STRM_END || t == EOL ) {
        path = NULL;
    } else {
        t = LexToken( LEX_PATH );
        assert( t == TOK_PATH || t == STRM_END || t == EOL );
        if( t == STRM_END || t == EOL ) {
            path = NULL;
        } else {
            path = CurAttr.ptr;
            t = LexToken( LEX_PATH ); /* prefetch next token */
        }
    }

    for(;;) {
        if( t == STRM_END || t == EOL ) break;
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
        FreeSafe( path );               /* from CurAttr.ptr */
    }
}



STATIC char *getFileName (char* intext, int *offset){
/************************************************
 * get the filename from the given text
 * if there is no file name then get the text body
 * offset - how long after the << is the file name
 * this only gets the explicit fileName
 * for the fileNames that need to be generated
 * we do it later but still we create an FLIST for
 * the temp file anyway
 */

    VECSTR  tempStr;
    char*   ret;
    BOOLEAN doubleQuote;    //are there double quotes

    assert (intext != NULL && offset != NULL);

    *offset     = 0;
    doubleQuote = FALSE;

    if (intext[*offset] == DOUBLEQUOTE) {
        doubleQuote = TRUE;
        *offset     = 1;
    }
    while (1) {
        if (intext[*offset] == NULLCHAR) {
            break;
        } else if ((isws(intext[*offset])          ||
                    intext[*offset]== LESSTHAN     ||
                    intext[*offset]== GREATERTHAN) &&
                    !doubleQuote){
            break;
        } else if (doubleQuote && intext[*offset] == BACKSLASH) {
            if (intext[*offset + 1] == DOUBLEQUOTE ) {
                *offset = *offset + 1;
            }
        } else if (doubleQuote && intext[*offset] == DOUBLEQUOTE) {
            ++(*offset);
            break;
        }
        ++(*offset);
    }

    if (intext[(*offset)-1] != DOUBLEQUOTE && doubleQuote ||
        *offset == 1                       && doubleQuote) {
        /* error */
        PrtMsg(ERR|LOC|NON_MATCHING_QUOTE);
        ret = NULL;
        return (ret);
    }


    if ( !doubleQuote && *offset > 0 ||
         doubleQuote  && *offset > 1) {
        tempStr = StartVec();
        if (doubleQuote) {
            WriteNVec(tempStr,intext+1,*offset-2);
        } else {
            WriteNVec(tempStr,intext,*offset);
        }
        ret = FinishVec(tempStr);
        return (ret);

    } else {
        // if we need a temporary file then give back << as a filename
        tempStr = StartVec();
        WriteVec(tempStr,INLINE_SYMBOL);
        ret = FinishVec(tempStr);
        return (ret);
    }
}



STATIC void getBody (FLIST* head) {
/************************************
 * get the body of the file from the input stream
 */

    FLIST* current;

    TOKEN_T t;
    VECSTR  buf;
    VECSTR  bufTemp;
    char*   temp;
    char*   currChar;

    current = head;
    if (current == NULL) {
        return;
    }

    /* Inlinelevel == the number of inline files we need to create */
    while (inlineLevel >  0    &&
           current     != NULL) {
        buf = StartVec();
        WriteVec(buf,"");
        while (1) {
            t = PreGetCH();
            if (t == STRM_END) {
                UnGetCH(t);
                PrtMsg(ERR|LOC|UNEXPECTED_EOF);
                return;
            }
            UnGetCH(t);
            temp = ignoreWSDeMacro(TRUE,ForceDeMacro());
            if (temp[0] == LESSTHAN) {
                if (temp[1] == LESSTHAN) {
                    /* terminator of inline file is found when first
                     * two characters are <<
                     */
                     currChar = temp+2;
                     while (*currChar != NULLCHAR &&
                            isws(*currChar)) {
                         ++currChar;
                     }
                     if (*currChar == NULLCHAR) {
                         current->keep = FALSE;
                     } else if (strnicmp(currChar,NOKEEP,6)== 0) {
                         current->keep = FALSE;
                     } else if (strnicmp(currChar,KEEP,4) == 0) {
                         current->keep = TRUE;
                     } else {
                         /* error only expecting (NO)KEEP */
                         PrtMsg(ERR|LOC|NOKEEP_ONLY);
                     }

                     FreeSafe(temp);
                     break;
                }
            }
            bufTemp = StartVec();
            WriteVec(bufTemp,temp);
            FreeSafe(temp);
            CatVec(buf,bufTemp);
            bufTemp = StartVec();
            WriteVec(bufTemp,"\n");
            CatVec(buf,bufTemp);
        }
        current->body = FinishVec(buf);
        current       = current->next;
        --inlineLevel;
    }
    if (inlineLevel > 0) {
        PrtMsg(ERR|LOC|UNEXPECTED_EOF);
    }
}

/*
 * this returns the head of the flist and modifies the command text to
 * change << to the actual file name to be used
 * also returns an offset so that the calling program would know
 * how long the command is in cmdText
 * cmdText is also modified it removes the file name information and
 * substitutes it with the actual file name that is going to be used
 */
STATIC FLIST* GetInlineFile (char** commandIn) {

    int     offset;
    int     index;
    FLIST*  head;
    FLIST*  current;
    char*   cmdText;
    int     start;       // start of cmdText to copy into newCommand
    VECSTR  newCommand;  // stores the new command built by replacing << with
                         // the actual file name

    /* inlinelevel must be initially zero when processing */
    assert(inlineLevel == 0 && commandIn != NULL);

    cmdText = *commandIn;
    head    = NULL;
    current = NULL;

    newCommand = StartVec();

    index = 0;
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
    while (cmdText[index] != NULLCHAR) {
        if (cmdText[index] == LESSTHAN) {
            if (cmdText[index+1] == LESSTHAN) {

                // Add the current vector into the new command
                WriteNVec(newCommand,cmdText+start,index-start);

                ++inlineLevel;
                if (head == NULL) {
                    current = NewFList();
                    head    = current;
                } else {
                    current->next = NewFList();
                    current       = current->next;
                }

                // Add file Name into new command text
                current->fileName = getFileName (cmdText+2+index,&offset);

                // Check for long file name
                if (*(cmdText+2+index) == DOUBLEQUOTE) {
                    WriteVec( newCommand, "\"");
                    WriteVec( newCommand, current->fileName);
                    WriteVec( newCommand, "\"");
                } else {
                    WriteVec( newCommand, current->fileName);
                }

                current->body     = NULL;
                current->next     = NULL;
                index = index + offset;
                start = index + 2;

            }
            ++index;
        }
        ++index;
    }
    WriteNVec( newCommand, cmdText+start,index-start);
    FreeSafe (cmdText);
    *commandIn = FinishVec(newCommand);
    getBody(head);
    return(head);

}

STATIC char* formatPathName(const char * inPath) {

    char   buf [_MAX_PATH];

    if (inPath != NULL) {
        _makepath ( buf , NULL, inPath, NULL, NULL );
        return (StrDupSafe(buf));
    } else {
        return (NULL);
    }

}




STATIC void getCurTargDepPath (char ** cur_targ_path,
                               char ** cur_dep_path) {
/*
 * this is to get the current target path and dependent path
 * when sufsuf is created
 */
    if (*cur_targ_path != NULL) {
        FreeSafe (*cur_targ_path);
    }
    if (*cur_dep_path != NULL) {
        FreeSafe (*cur_dep_path);
    }
    if (targ_path != NULL ) {
        *cur_targ_path = formatPathName ( targ_path );
    } else {
        *cur_targ_path = StrDupSafe("");
    }
    if (dep_path != NULL ) {
        *cur_dep_path  = formatPathName ( dep_path );
    } else {
        *cur_dep_path = StrDupSafe("");
    }

    FreeSafe ( targ_path );
    FreeSafe ( dep_path );
    targ_path = NULL;
    dep_path  = NULL;
}


extern TLIST *Parse( void )
/***************************
 * Call LexToken, and dispatch to appropriate routine
 */
{
    TOKEN_T t;
    CLIST   *newclist;
    CLIST   *bclist;
    TLIST   *btlist;
    BOOLEAN clist_warning_given;
    BOOLEAN token_filename;
    char* cur_depend_path;
    char* cur_target_path;

    firstTarget = NULL;

    bclist = NULL;
    btlist = NULL;

    cur_depend_path = NULL;
    cur_target_path = NULL;

    clist_warning_given = FALSE;
    token_filename      = FALSE;


    for(;;) {

        /*
         * If the current target is a sufsuf then
         * we must demacro the text later
         * when the actual filenames are resolved
         */
        if (btlist != NULL) {
            ImplicitDeMacro = Glob.microsoft &&
                              btlist->target->sufsuf;
        }
        t = LexToken( LEX_PARSER );
        ImplicitDeMacro = FALSE;

        if( t != TOK_CMD && t != EOL ) {
            if( btlist != NULL ) {
                    /* link the commands to the targets */
                linkCList( btlist,
                           bclist,
                           cur_target_path,
                           cur_depend_path );
                bclist = NULL;
                btlist = NULL;
                if(cur_depend_path != NULL) {
                    FreeSafe(cur_depend_path);
                }
                if(cur_target_path != NULL) {
                    FreeSafe(cur_target_path);
                }
                cur_target_path = NULL;
                cur_depend_path = NULL;
                if (Glob.microsoft && token_filename == TRUE)  {
                    exPop();
                    token_filename = FALSE;
                }
            }
            clist_warning_given = FALSE;
        }

        if( t == STRM_END ) break;

        switch( t ) {
        case EOL:
            break;
        case TOK_CMD:
            if( *CurAttr.ptr == NULLCHAR ) {
                    /* discard blank lines */
                FreeSafe( CurAttr.ptr );
            } else {
                if( btlist == NULL ) {
                    if( !clist_warning_given ) {
                        PrtMsg( WRN|LOC| CLIST_HAS_NO_OWNER );
                        clist_warning_given = TRUE;
                    }
                    FreeSafe( CurAttr.ptr );
                } else {
                    newclist = NewCList();
                    newclist->text       = CurAttr.ptr;
                    /* stack in reverse order */
                    newclist->next       = bclist;
                    /* look at the command text to see if we need
                     * to get inline file if we do then create inline
                     * file information in memory
                     * note: that if the clist pertains to a implicit
                     *       rule we must deMacro the text the same
                     *       way as wmake does for microsoft option
                     */
                    if (btlist != NULL) {
                        ImplicitDeMacro = Glob.microsoft &&
                              btlist->target->sufsuf;
                    }
                    newclist->inlineHead = GetInlineFile(&(newclist->text));
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
            getCurTargDepPath (&cur_target_path,
                               &cur_depend_path);
            if (btlist != NULL) {
                btlist->target->sufsuf = TRUE;
            }
            break;
        case TOK_FILENAME:
            parseTargDep( t, &btlist );
            if (Glob.microsoft && btlist != NULL)  {
                if (btlist->target != NULL && btlist->target->depend != NULL) {
                    exPush(btlist->target,btlist->target->depend,NULL);
                    token_filename = TRUE;
                }
            }
            break;
        case TOK_DOTNAME:
            parseDotName( t, &btlist );
            break;
        default:
#ifdef DEVELOPMENT
            PrtMsg( FTL| INVALID_TOKEN_IN, "Parse" );
#else
            PrtMsg( WRN| LOC| IGNORE_OUT_OF_PLACE_M, M_UNKNOWN_TOKEN );
#endif
        }
    }

    return( firstTarget );
}


extern void ParseInit( void )
/***************************/
{
    inlineLevel   = 0;
    DoingUpdate = FALSE;
}


extern void ParseFini( void )
/***************************/
{
}

