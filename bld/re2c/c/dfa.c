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


#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "globals.h"
#include "substr.h"
#include "dfa.h"

typedef struct GoTo {
    Char        ch;
    void        *to;
} GoTo;

State *State_new(void)
{
    State   *s = malloc( sizeof( State ) );

    s->label = 0;
    s->rule = NULL;
    s->next = NULL;
    s->link = NULL;
    s->depth = 0;
    s->kCount = 0;
    s->kernel = NULL;
    s->isBase = 0;
    s->action = NULL;
    s->go.nSpans = 0;
    s->go.span = NULL;
    return( s );
}

static void State_delete(State *s)
{
    if( s->kernel )
        free( s->kernel );
    if( s->go.span )
        free( s->go.span );
    free( s );
}

static Ins **closure( Ins **cP, Ins *i )
{
    while( !i->i.marked ) {
        i->i.marked = TRUE;
        *(cP++) = i;
        if(i->i.tag == FORK){
            cP = closure(cP, i + 1);
            i = (Ins*) i->i.link;
        } else if(i->i.tag == GOTO){
            i = (Ins*) i->i.link;
        } else
            break;
    }
    return cP;
}

static State *DFA_findState( DFA *d, Ins **kernel, uint kCount )
{
    Ins     **cP, **iP, *i;
    State   *s;
    
    kernel[kCount] = NULL;
    cP = kernel;
    for( iP = kernel; (i = *iP) != NULL; ++iP ) {
        if( i->i.tag == CHAR || i->i.tag == TERM ) {
            *cP++ = i;
        } else {
            i->i.marked = FALSE;
        }
    }
    kCount = cP - kernel;
    kernel[kCount] = NULL;
    for( s = d->head; s != NULL; s = s->next ) {
        if( s->kCount == kCount ) {
            for( iP = s->kernel; (i = *iP) != NULL; ++iP ) {
                if( !i->i.marked ) {
                    break;
                }
            }
            if( i == NULL ) {
                break;
            }
        }
    }
    if( s == NULL ) {
        s = State_new();
        DFA_addState( d, d->tail, s );
        s->kCount = kCount;
        s->kernel = malloc( ( kCount + 1 ) * sizeof( Ins * ) );
        memcpy( s->kernel, kernel, ( kCount + 1 ) * sizeof( Ins * ) );
        s->link = d->toDo;
        d->toDo = s;
    }
    for( iP = kernel; (i = *iP) != NULL; ++iP ) {
        i->i.marked = FALSE;
    }
    return s;
}

DFA *DFA_new( Ins *ins, uint ni, uint lb, uint ub, Char *rep )
{
    DFA     *d = malloc( sizeof( DFA ) );
    Ins     **work = malloc( ( ni + 1 ) * sizeof( Ins * ) );
    uint    nc = ub - lb;
    GoTo    *goTo = malloc( nc * sizeof( GoTo ) );
    Span    *span = malloc( nc * sizeof( Span ) );

    d->lbChar = lb;
    d->ubChar = ub;
    memset( goTo, 0, nc * sizeof( GoTo ) );
    d->tail = &d->head;
    d->head = NULL;
    d->nStates = 0;
    d->toDo = NULL;
    DFA_findState( d, work, closure( work, &ins[0] ) - work );
    while(d->toDo){
        State   *s = d->toDo;
        Ins     **cP, **iP, *i;
        uint    nGoTos = 0;
        uint    j;

        d->toDo = s->link;
        s->rule = NULL;
        for( iP = s->kernel; (i = *iP) != NULL; ++iP ) {
            if( i->i.tag == CHAR ) {
                Ins *j2;
                for( j2 = i + 1; j2 < (Ins *)i->i.link; ++j2 ) {
                    if( !(j2->c.link = goTo[j2->c.value - lb].to) )
                        goTo[nGoTos++].ch = j2->c.value;
                    goTo[j2->c.value - lb].to = j2;
                }
            } else if( i->i.tag == TERM ) {
                if( !s->rule || ((RegExp *)i->i.link)->u.RuleOp.accept < s->rule->u.RuleOp.accept ) {
                    s->rule = (RegExp *)i->i.link;
                }
            }
        }

        for( j = 0; j < nGoTos; ++j ) {
            GoTo *go = &goTo[goTo[j].ch - lb];
            i = (Ins*)go->to;
            for( cP = work; i != NULL; i = (Ins *)i->c.link ) {
                cP = closure(cP, i + i->c.bump);
            }
            go->to = DFA_findState( d, work, cP - work );
        }

        s->go.nSpans = 0;
        for( j = 0; j < nc; ) {
            State *to = (State *)goTo[rep[j]].to;
            while( ++j < nc && goTo[rep[j]].to == to ) ;
            span[s->go.nSpans].ub = lb + j;
            span[s->go.nSpans].to = to;
            s->go.nSpans++;
        }

        for( j = nGoTos; j-- > 0; ) {
            goTo[goTo[j].ch - lb].to = NULL;
        }

        s->go.span = malloc( s->go.nSpans * sizeof( Span ) );
        memcpy( s->go.span, span, s->go.nSpans * sizeof( Span ) );

        Action_new_Match( s );

    }
    free( work );
    free( goTo );
    free( span );

    return( d );
}

void DFA_delete( DFA *d )
{
    State   *s;

    while( (s = d->head) != NULL ){
        d->head = s->next;
        State_delete( s );
    }
}

void DFA_addState( DFA *d, State **a, State *s )
{
    s->label = d->nStates++;
    s->next = *a;
    *a = s;
    if( a == d->tail ) {
        d->tail = &s->next;
    }
}
