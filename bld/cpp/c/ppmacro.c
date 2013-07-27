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
* Description:  Macro processing for the preprocessor.
*
****************************************************************************/


#include "preproc.h"

#define PPT_UNEXPANDABLE_ID     PPT_LAST_TOKEN

extern  char    PP__DATE__[14];
extern  char    PP__TIME__[11];


typedef struct macro_arg {
        char            *name;
        MACRO_TOKEN     *arg;
} MACRO_ARG;

typedef struct nested_macros {
        struct nested_macros *next;
        MACRO_ENTRY *fmentry;
        MACRO_ARG *macro_parms;
        bool    rescanning;
        bool    substituting_parms;
} NESTED_MACRO;

NESTED_MACRO *NestedMacros;
int           MacroDepth;
MACRO_TOKEN  *MacroExpansion( MACRO_ENTRY *, bool );
MACRO_TOKEN  *NestedMacroExpansion( MACRO_ENTRY * );

void FreeTokenList( MACRO_TOKEN *head )
{
    MACRO_TOKEN *mtok;

    for( ;; ) {
        if( head == NULL ) break;
        mtok = head;
        head = head->next;
        PP_Free( mtok );
    }
}

void DeleteNestedMacro( void )
{
    MACRO_ENTRY  *fmentry;
    NESTED_MACRO *nested;
    MACRO_ARG    *macro_parms;
    MACRO_TOKEN  *mtok;
    int          i;

    nested = NestedMacros;
    if( nested != NULL ) {
        NestedMacros = nested->next;
        macro_parms =  nested->macro_parms;
        fmentry = nested->fmentry;
        PP_Free( nested );
        i = fmentry->parmcount - 1;
        if( i > 0  &&  macro_parms != NULL ) {
            do {
                --i;
                for(; (mtok=macro_parms[i].arg) != NULL; ) {
                    macro_parms[i].arg = mtok->next;
                    PP_Free( mtok );
                }
            } while( i != 0 );
            PP_Free( macro_parms );
        }
    }
}

MACRO_TOKEN *PPNextToken( void )
{
    MACRO_TOKEN *mtok;

    for( ;; ) {
        mtok = PPTokenList;
        if( mtok == NULL )  break;
        PPTokenList = mtok->next;
        mtok->next = NULL;
        if( mtok->token == PPT_NULL  &&  mtok->data[0] == 'Z' ) {
            DeleteNestedMacro();
            PP_Free( mtok );
            continue;
        }
        PPTokenPtr = mtok->data;
        break;
    }
    return( mtok );
}

MACRO_TOKEN *NextMToken( void )
{
    MACRO_TOKEN *mtok;
    size_t      len;
    char        token;

    mtok = PPNextToken();
    if( mtok == NULL ) {
        if( PP_ScanNextToken( &token ) != 0 ) {         /* 03-jan-95 */
            token = PPT_EOF;
            len = 1;
            PPTokenPtr = " ";
        } else {
            len = PPCharPtr - PPTokenPtr;
            if( token == PPT_WHITE_SPACE  ||  token == PPT_COMMENT ) {
                len = 1;
                PPTokenPtr = " ";
            }
        }
        mtok = (MACRO_TOKEN *)PP_Malloc( sizeof(MACRO_TOKEN) + len );
        mtok->next = NULL;
        mtok->token = token;
        strcpy( mtok->data, PPTokenPtr );
        PPTokenPtr = PPCharPtr;
    }
    return( mtok );
}

MACRO_TOKEN *PPTrimWhiteSpace( MACRO_TOKEN *head )
{
    MACRO_TOKEN *mtok;
    MACRO_TOKEN *prev;

    mtok = head;
    prev = NULL;
    for( ;; ) {
        if( mtok == NULL ) break;
        if( mtok->token != PPT_WHITE_SPACE )  prev = mtok;
        mtok = mtok->next;
    }
    if( prev != NULL ) {
        if( prev->next != NULL ) {
            FreeTokenList( prev->next );
            prev->next = NULL;
        }
    }
    return( head );
}

MACRO_ARG *PPCollectParms( MACRO_ENTRY *fmentry )
{
    int         bracket;
    MACRO_TOKEN *mtok;
    MACRO_TOKEN *head;
    MACRO_TOKEN *tail;
    int         parm_cnt;
    MACRO_ARG   *macro_parms;

    macro_parms = NULL;
    if( fmentry->parmcount != 0 ) { /* if () expected */
        if( fmentry->parmcount > 1 ) {
            macro_parms = (MACRO_ARG *)PP_Malloc( (fmentry->parmcount - 1) *
                                    sizeof( MACRO_ARG ) );
        }
        parm_cnt = 0;
        mtok = PPNextToken();
        if( mtok == NULL  &&  PPTokenPtr[0] == '(' ) {
            // already positioned at the '('
        } else {
            for( ;; ) {
                if( mtok == NULL )  mtok = NextMToken();
                if( mtok->token != PPT_WHITE_SPACE ) break;
                PP_Free( mtok );
                mtok = NULL;
            }
            PP_Free( mtok );    // this should be '(', throw it away
        }
        bracket = 0;
        head = NULL;
        tail = NULL;
        for( ;; ) {
            for( ;; ) {
                mtok = NextMToken();
                if( mtok->token != PPT_WHITE_SPACE ) break;
                if( head != NULL ) break;
                PP_Free( mtok );
            }
            if( mtok->token == PPT_EOF ) break;         /* 03-jan-95 */
            if( mtok->token == PPT_LEFT_PAREN ) {
                ++bracket;
            } else if( mtok->token == PPT_RIGHT_PAREN ) {
                if( bracket == 0 ) break;
                --bracket;
            } else if( mtok->token == PPT_COMMA  &&  bracket == 0 ) {
                if( parm_cnt < fmentry->parmcount - 1 ) {
                    macro_parms[ parm_cnt ].arg = PPTrimWhiteSpace( head );
                }
                PP_Free( mtok );
                ++parm_cnt;
                head = NULL;
                tail = NULL;
                continue;
            }
            if( head == NULL )  head = mtok;
            if( tail != NULL )  tail->next = mtok;
            tail = mtok;
        }
        PP_Free( mtok );
        if( parm_cnt < fmentry->parmcount - 1 ) {
            macro_parms[ parm_cnt ].arg = PPTrimWhiteSpace( head );
            ++parm_cnt;
        } else {
            mtok = head;
            while( head != NULL ) {
                mtok = head->next;
                PP_Free( head );
                head = mtok;
            }
        }
#if 0
        if( parm_cnt < fmentry->parmcount - 1 ) {
            CErr( ERR_TOO_FEW_MACRO_PARMS, fmentry->name );
        } else if( parm_cnt > fmentry->parmcount - 1 ) {
            CErr( ERR_TOO_MANY_MACRO_PARMS, fmentry->name );
        }
#endif
        for( ;; ) {
            if( parm_cnt >= fmentry->parmcount - 1 ) break;
            macro_parms[ parm_cnt ].arg = NULL;
            ++parm_cnt;
        }
    }
    return( macro_parms );
}

void DumpMTokens( MACRO_TOKEN *mtok )
{
    while( mtok ) {
        printf( "%s\n", mtok->data );
        mtok = mtok->next;
    }
    fflush( stdout );
}
void DumpNestedMacros( void )
{
    NESTED_MACRO *nested;

    nested = NestedMacros;
    while( nested ) {
        printf( "%s\n", nested->fmentry->name );
        nested = nested->next;
    }
    fflush( stdout );
}


int TokLength( char *p )
{
    int len;

    len = 0;
    while( *p++ ) ++len;
    return( len );
}

MACRO_TOKEN *BuildAToken( char *p )
{
    int         len;
    MACRO_TOKEN *mtok;

    len = TokLength( p );
    mtok = (MACRO_TOKEN *)PP_Malloc( sizeof(MACRO_TOKEN) + len );
    mtok->next = NULL;
    len = 0;
    for( ;; ) {
        mtok->data[len] = *p;
        if( *p == '\0' ) break;
        ++p;
        ++len;
    }
    return( mtok );
}

MACRO_TOKEN *AppendToken( MACRO_TOKEN *head, char token, char *data )
{
    MACRO_TOKEN *tail;
    MACRO_TOKEN *new;

    new = BuildAToken( data );
    new->token = token;
    if( head == NULL ) {
        head = new;
    } else {
        tail = head;
        while( tail->next != NULL ) tail = tail->next;
        tail->next = new;
    }
    return( head );
}

bool MacroBeingExpanded( MACRO_ENTRY *fmentry )
{
    NESTED_MACRO *nested;

    nested = NestedMacros;
    while( nested ) {
        if( nested->fmentry == fmentry )  return( TRUE );
        if( !nested->rescanning )  break;
        nested = nested->next;
    }
    return( FALSE );
}

int Expandable( MACRO_ENTRY *me, MACRO_TOKEN *mtok, bool macro_parm )
{
    int         lparen;
    char        token;

    if( me->parmcount == PP_SPECIAL_MACRO ) {
        return( 1 );
    }
    if( me->parmcount == 0 ) { /* if () not expected */
        if( macro_parm ) {                              /* 20-feb-93 */
            if( MacroBeingExpanded( me ) )  return( 0 );
        }
        return( 1 );
    }
    for( ;; ) {
        if( mtok == NULL ) break;
        if( mtok->token != PPT_WHITE_SPACE  &&  mtok->token != PPT_NULL ) break;
        mtok = mtok->next;
    }
    if( mtok != NULL ) {
        if( mtok->token == PPT_LEFT_PAREN ) {
            if( MacroDepth == 1  &&  !macro_parm )  return( 1 );
            lparen = 0;
            for( ;; ) {
                mtok = mtok->next;
                if( mtok == NULL ) break;
                if( mtok->token == PPT_LEFT_PAREN ) {
                    ++lparen;
                } else if( mtok->token == PPT_RIGHT_PAREN ) {
                    if( lparen == 0 )  return( 1 );
                    --lparen;
                }
            }
        }
    } else if( !macro_parm ) {
        for( ;; ) {
            if( PP_ScanNextToken( &token ) != 0 )  return( 0 );
            if( token != PPT_WHITE_SPACE  &&  token != PPT_COMMENT )  break;
        }
        if( token == PPT_LEFT_PAREN ) {
            return( 1 );
        } else if( PPFlags & PPFLAG_PREPROCESSING ) {
            return( 2 );
        }
    }
    return( 0 );
}

MACRO_TOKEN *ExpandNestedMacros( MACRO_TOKEN *head, bool rescanning )
{
    MACRO_TOKEN *mtok;
    MACRO_TOKEN *toklist;
    MACRO_TOKEN *prev_tok;
    MACRO_TOKEN *old_tokenlist;
    MACRO_ENTRY *me;
    int         i;

    mtok = head;
    ++MacroDepth;
    prev_tok = NULL;
    for( ;; ) {
        if( mtok == NULL ) break;
        toklist = NULL;
        if( mtok->token == PPT_ID ) {
            // if macro and not being expanded, then expand it
            // only tokens available for expansion are those in mtok list
            me = PP_MacroLookup( mtok->data );
            if( me != NULL ) {
                if( rescanning ) {
                    if( MacroBeingExpanded( me ) ) {
                        mtok->token = PPT_UNEXPANDABLE_ID;
                    } else {
                        toklist = mtok;
                        while( toklist->next != NULL ) {
                            toklist = toklist->next;
                        }
                        toklist->next = PPTokenList;
                        i = Expandable( me, mtok->next, FALSE );
                        switch( i ) {
                        case 0:         // macro is currently not expandable
                            mtok->token = PPT_TEMP_ID;
                            toklist->next = NULL;
                            toklist = NULL;
                            break;
                        case 1:         // macro is expandable
                            PPTokenList = mtok->next;
                            if( head == mtok ) {
                                head = NULL;
                                prev_tok = NULL;
                            }
                            PP_Free( mtok );
                            toklist = MacroExpansion( me, rescanning );
                            mtok = PPTokenList;
                            PPTokenList = NULL;
                            break;
                        case 2:         // we skipped over some white space
                            mtok->token = PPT_UNEXPANDABLE_ID;
                            toklist->next = NULL;
                            toklist = BuildAToken( " " );
                            toklist->token = PPT_WHITE_SPACE;
                            toklist->next = mtok->next;
                            mtok->next = toklist;
                            toklist = NULL;
                            break;
                        }
                    }
                } else {        // expanding a macro parm
                    if( Expandable( me, mtok->next, TRUE ) ) {
                        old_tokenlist = PPTokenList;
                        PPTokenList = mtok->next;
                        if( head == mtok ) {
                            head = NULL;
                            prev_tok = NULL;
                        }
                        PP_Free( mtok );
                        toklist = NestedMacroExpansion( me );
                        mtok = PPTokenList;
                        PPTokenList = old_tokenlist;
                    } else {
                        prev_tok = mtok;
                        mtok = mtok->next;      // advance onto next token
                    }
                }
            } else {
                mtok->token = PPT_SAVED_ID;     // avoid rechecking this ID
                prev_tok = mtok;
                mtok = mtok->next;      // advance onto next token
            }
        } else if( mtok->token == PPT_NULL ) {
            toklist = mtok->next;
            rescanning = NestedMacros->rescanning;
            DeleteNestedMacro();
            PP_Free( mtok );
            mtok = toklist;
            toklist = NULL;
        } else {                        // advance onto next token
            prev_tok = mtok;
            mtok = mtok->next;
        }
        if( toklist != NULL ) {         // new tokens to insert
            if( prev_tok == NULL ) {
                head = toklist;
            } else {
                prev_tok->next = toklist;
            }
            if( mtok != NULL ) {
                while( toklist->next != NULL )  toklist = toklist->next;
                toklist->next = mtok;
            }
            if( prev_tok == NULL ) {
                mtok = head;
            } else {
                mtok = prev_tok->next;
            }
        } else {
            // either no change, or tokens were deleted
            if( prev_tok == NULL ) {
                head = mtok;
            } else {
                prev_tok->next = mtok;
            }
        }
    }
    for( mtok = head; mtok; mtok = mtok->next ) {       /* 02-nov-93 */
        // change a temporarily unexpandable ID into an ID because it
        // could become expandable in a later rescanning phase
        if( mtok->token == PPT_TEMP_ID )  mtok->token = PPT_ID;
    }
    --MacroDepth;
    return( head );
}

MACRO_TOKEN *Glue2Tokens( MACRO_TOKEN *first, MACRO_TOKEN *second )
{
    MACRO_TOKEN *mtok;
    size_t      len;

    len = 0;
    if( first != NULL )  len += strlen( first->data );  /* 21-apr-93 */
    if( second != NULL ) len += strlen( second->data );
    mtok = (MACRO_TOKEN *)PP_Malloc( sizeof(MACRO_TOKEN) + len );
    mtok->next = NULL;
    mtok->token = first->token;
    if( mtok->token == PPT_SAVED_ID || mtok->token == PPT_UNEXPANDABLE_ID ) {
        mtok->token = PPT_ID;
    }
    mtok->data[0] = '\0';
    if( first != NULL )  strcpy( mtok->data, first->data );
    if( second != NULL ) strcat( mtok->data, second->data );
    return( mtok );
}

MACRO_TOKEN *GlueTokens( MACRO_TOKEN *head )
{
    MACRO_TOKEN *mtok;
    MACRO_TOKEN *prev;
    MACRO_TOKEN *next;
    MACRO_TOKEN *new;

    mtok = head;
    prev = NULL;
    for( ;; ) {
        if( mtok == NULL ) break;
        if( mtok->token != PPT_WHITE_SPACE ) {
            next = mtok->next;
            if( next == NULL ) break;
            if( next->token == PPT_WHITE_SPACE )  next = next->next;
            if( next == NULL ) break;
            if( next->token == PPT_SHARP_SHARP ) {
                next = next->next;
                // glue mtok->token with next->token to make one token
                // create new token
                new = Glue2Tokens( mtok, next );
                if( prev == NULL ) {
                    head = new;
                } else {
                    prev->next = new;
                }
                while( new->next != NULL ) {
                    prev = new;
                    new = new->next;
                }
                if( next != NULL ) {                    /* 21-apr-93 */
                    new->next = next->next;
                }
                do {
                    next = mtok->next;
                    PP_Free( mtok );
                    mtok = next;
                } while( mtok != new->next );
                mtok = new;
                continue;       /* to catch consecutive ##'s */
            }
        }
        prev = mtok;
        mtok = mtok->next;
    }
    return( head );
}


MACRO_TOKEN *BuildMTokenList( MACRO_ENTRY *me, MACRO_ARG *macro_parms )
{
    MACRO_TOKEN *mtok;
    MACRO_TOKEN *head;
    MACRO_TOKEN *tail;
    char        *p;
    char        *p2;
    unsigned    len;
    int         i;
    char        token;
    char        prev_token;
    char        c;

    p = me->replacement_list;
    for( i = 1; i < me->parmcount; i++ ) {
        macro_parms[i-1].name = p;
        while( *p != '\0' ) p++;
        p++;
    }
    head = NULL;
    tail = NULL;
    if( p == NULL )  return( NULL );
    prev_token = PPT_NULL;
    for( ;; ) {
        if( *p == 0 ) break;
        p2 = PP_ScanToken( p, &token );
        if( token == PPT_WHITE_SPACE  &&  prev_token == PPT_SHARP_SHARP ) {
            p = p2;
            continue;
        }
        len = p2 - p;
        if( token == PPT_ID ) {
            c = *p2;
            *p2 = '\0';
            for( i = 1; i < me->parmcount; i++ ) {
                if( strcmp( p, macro_parms[i-1].name ) == 0 ) {
                    token = PPT_MACRO_PARM;
                    i--;
                    len = 0;
                    break;
                }
            }
            *p2 = c;
        }
        mtok = (MACRO_TOKEN *)PP_Malloc( sizeof(MACRO_TOKEN) + len );
        mtok->next = NULL;
        mtok->token = token;
        prev_token = token;
        if( token == PPT_MACRO_PARM ) {
            mtok->data[0] = i;
        } else {
            memcpy( mtok->data, p, len );
            mtok->data[len] = '\0';
        }
        p = p2;
        if( head == NULL )  head = mtok;
        if( tail != NULL )  tail->next = mtok;
        tail = mtok;
    }
    return( head );
}

MACRO_TOKEN *DuplicateList( MACRO_TOKEN *list )
{
    MACRO_TOKEN *mtok;
    MACRO_TOKEN *head;
    MACRO_TOKEN *tail;

    head = NULL;
    tail = NULL;
    for( ;; ) {
        if( list == NULL ) break;
        mtok = BuildAToken( list->data );
        mtok->token = list->token;
        if( head == NULL )  head = mtok;
        if( tail != NULL )  tail->next = mtok;
        tail = mtok;
        list = list->next;
    }
    return( head );
}

unsigned MakeString( MACRO_TOKEN *list, char *p )
{
    unsigned    len;
    char        *p2;
    bool        output;

    len = 0;
    output = ( p != NULL );
    if( output )
        *p++ = '\"';
    ++len;
    for( ;; ) {
        if( list == NULL ) break;
        p2 = list->data;
        while( *p2 != '\0' ) {
            if( *p2 == '\"'  ||  *p2 == '\\' ) {
                if( list->token == PPT_LITERAL ) {
                    if( output )
                        *p++ = '\\';
                    ++len;
                }
            }
            if( output )
                *p++ = *p2;
            p2++;
            ++len;
        }
        list = list->next;
    }
    if( output )
        *p++ = '\"';
    ++len;
    if( output )
        *p = '\0';
    return( len );
}

MACRO_TOKEN *BuildString( MACRO_TOKEN *list )
{
    MACRO_TOKEN *mtok;
    unsigned    len;

    len = MakeString( list, NULL );
    mtok = (MACRO_TOKEN *)PP_Malloc( sizeof(MACRO_TOKEN) + len );
    mtok->next = NULL;
    mtok->token = PPT_LITERAL;
    MakeString( list, mtok->data );
    return( mtok );
}

static bool SharpSharp( MACRO_TOKEN *mtok )
{
    for( ;; ) {
        if( mtok == NULL ) break;
        if( mtok->token == PPT_SHARP_SHARP )  return( TRUE );
        if( mtok->token != PPT_WHITE_SPACE )  break;
        mtok = mtok->next;
    }
    return( FALSE );
}

MACRO_TOKEN *SubstituteParms( MACRO_TOKEN *head, MACRO_ARG *macro_parms )
{
    MACRO_TOKEN *mtok;
    MACRO_TOKEN *mtok2;
    MACRO_TOKEN *list;
    MACRO_TOKEN *toklist;
    MACRO_TOKEN *prev_tok;
    char        prev_token;

    mtok = head;
    prev_tok = NULL;
    prev_token = PPT_NULL;
    for( ;; ) {
        if( mtok == NULL ) break;
        list = NULL;
        if( mtok->token == PPT_SHARP ) {
            // replace this and next token (macro parm) with a string
            mtok2 = mtok;
            for( ;; ) {
                mtok2 = mtok2->next;
                if( mtok2 == NULL ) break;
                if( mtok2->token != PPT_WHITE_SPACE ) break;
            }
            if( mtok2 != NULL ) {
                if( mtok2->token == PPT_MACRO_PARM ) {
                    list = BuildString(
                            macro_parms[(unsigned char)mtok2->data[0]].arg );
                    toklist = mtok->next;
                    mtok->next = mtok2->next;
                    mtok2->next = NULL;
                    FreeTokenList( toklist );
                }
            }
        } else if( mtok->token == PPT_MACRO_PARM ) {
            // replace this ID with a copy of the tokens from
            // macro_parms[mtok->data[0]].arg
            list = DuplicateList(
                    macro_parms[(unsigned char)mtok->data[0]].arg );
            if( prev_token != PPT_SHARP_SHARP && !SharpSharp( mtok->next ) ) {
                list = ExpandNestedMacros( list, FALSE );
            }
            if( list == NULL ) {                        /* 21-apr-93 */
                list = mtok;
                mtok = mtok->next;
                PP_Free( list );
                list = NULL;
                if( prev_tok != NULL ) {
                    prev_tok->next = mtok;
                }
            }
        }
        if( list != NULL ) {
            if( prev_tok == NULL ) {
                head = list;
            } else {
                prev_tok->next = list;
            }
            while( list->next != NULL )  list = list->next;
            list->next = mtok->next;
            PP_Free( mtok );
            mtok = list;
        }
        if( mtok == NULL ) break;                       /* 21-apr-93 */
        prev_tok = mtok;
        if( mtok->token != PPT_WHITE_SPACE )  prev_token = mtok->token;
        mtok = mtok->next;
    }
    return( head );
}

MACRO_TOKEN *BuildSpecialToken( MACRO_ENTRY *me )
{
    MACRO_TOKEN *head;
    char        *p;
    char        *filename;
    char        token;
    char        buffer[200];

    p = NULL;
    token = 0;

    switch( me->name[2] ) {
    case 'L':                           /* __LINE__ */
        sprintf( buffer, "%d", PPLineNumber );
        p = buffer;
        token = PPT_NUMBER;
        break;
    case 'F':                           /* __FILE__ */
        p = buffer;
        *p++ = '\"';
        filename = PP_File->filename;
        for( ;; ) {
            if( *filename == '\0' ) break;
//          24-may-94  if( *filename == '\\' )  *p++ = '\\';
            if( *filename == '\\' )  *p++ = '\\';       // 14-sep-94
            *p++ = *filename;
            ++filename;
        }
        *p++ = '\"';
        *p = '\0';
        p = buffer;
        token = PPT_LITERAL;
        break;
    case 'D':                           /* __DATE__ */
        p = PP__DATE__;
        token = PPT_LITERAL;
        break;
    case 'T':                           /* __TIME__ */
        p = PP__TIME__;
        token = PPT_LITERAL;
        break;
    case 'S':                           /* __STDC__ */
        p = "1";
        token = PPT_NUMBER;
        break;
    }
    head = BuildAToken( p );
    head->token = token;
    return( head );
}

MACRO_TOKEN *MacroExpansion( MACRO_ENTRY *me, bool rescanning )
{
    MACRO_ARG   *macro_parms;
    MACRO_TOKEN *head;
    MACRO_TOKEN *mtok;
    NESTED_MACRO *nested;

    nested = (NESTED_MACRO *)PP_Malloc( sizeof( NESTED_MACRO ) );
    nested->fmentry = me;
    nested->rescanning = rescanning;
    nested->substituting_parms = FALSE;
    nested->macro_parms = NULL;
    if( me->parmcount == PP_SPECIAL_MACRO ) {
        head = BuildSpecialToken( me );
        nested->next = NestedMacros;
        NestedMacros = nested;
    } else {
        macro_parms = PPCollectParms( me );
        nested->next = NestedMacros;
        NestedMacros = nested;
        nested->macro_parms = macro_parms;
        head = BuildMTokenList( me, macro_parms );
        if( macro_parms != NULL ) {
            nested->substituting_parms = TRUE;
            head = SubstituteParms( head, macro_parms );
            nested->substituting_parms = FALSE;
        }
        head = GlueTokens( head );
        for( mtok = head; mtok; mtok = mtok->next ) {   /* 26-oct-93 */
            if( mtok->token == PPT_ID ) {
                for( nested = NestedMacros; nested; nested = nested->next ) {
                    if( strcmp( nested->fmentry->name, mtok->data ) == 0 ) {
                        if( !nested->substituting_parms ) {
                            // change token so it won't be considered a
                            // candidate as a macro
                            mtok->token = PPT_SAVED_ID;
                            break;
                        }
                    }
                }
            }
        }
    }
    head = AppendToken( head, PPT_NULL, "Z-<end of macro>" );
    return( head );
}

MACRO_TOKEN *NestedMacroExpansion( MACRO_ENTRY *me )
{
    return( ExpandNestedMacros( MacroExpansion( me, FALSE ), TRUE ) );
}

void DoMacroExpansion( MACRO_ENTRY *me )
{
    MacroDepth = 0;
    PPTokenList = NestedMacroExpansion( me );
    // GetMacroToken will feed back tokens from the TokenList
    // when the PPTokenList is exhausted, then revert back to normal scanning
}

void PPMacroVarInit( void )
/*************************/
{
    NestedMacros = NULL;
    MacroDepth = 0;
}

