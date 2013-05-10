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

#define FreeLabel( a ) MemFree( a )

#ifdef VICOMP
#define MyError(f,...)  printf(f "\n", __VA_ARGS__)
#else
#define MyError(f,...)  Error(f, __VA_ARGS__)
#endif

static  cs_entry      *TOS;

/*
 * oopsBob - fatal block nesting error
 */
#if defined( __WATCOMC__ ) && !defined( __AXP__ )
#pragma aux oopsBob aborts;
#endif
static void oopsBob( char *current, char *start )
{
    MyError( "'%s' has no %s", current, start );
    AbortGen( DO_NOT_CLEAR_MESSAGE_WINDOW );

} /* oopsBob */

/*
 * Push - put a entry on the top of the control stack
 */
static void Push( cstype type )
{
    cs_entry    *new;

#ifndef VICOMP
    if( EditFlags.ScriptIsCompiled ) {
        return;
    }
#endif
    new = MemAlloc( sizeof( cs_entry ) );
    new->next = TOS;
    TOS = new;
    TOS->end = NewLabel();
    TOS->top = NewLabel();
    TOS->type = type;
    TOS->srcline = CurrentSrcLine;

} /* Push */

/*
 * Pop - remove top entry off the control stack
 */
static void Pop( void )
{
    cs_entry    *crap;

#ifndef VICOMP
    if( EditFlags.ScriptIsCompiled ) {
        return;
    }
#endif
    crap = TOS;
    TOS = TOS->next;
    FreeLabel( crap->end );
    FreeLabel( crap->top );
    MemFree( crap );

} /* Pop */

/*
 * CSInit - prepare control stack
 */
void CSInit( void )
{
    TOS = NULL;
    Push( CS_EOS );

} /* CSInit */

/*
 * Purge control stack
 */
vi_rc CSFini( void )
{
    bool        iserr = FALSE;

#ifndef VICOMP
    if( !EditFlags.ScriptIsCompiled ) {
#endif
        while( TOS->type != CS_EOS ) {
            iserr = TRUE;
            MyError( "unfinished c.s. at line %d", TOS->srcline );
            Pop();
        }

        if( iserr ) {
            return( DO_NOT_CLEAR_MESSAGE_WINDOW );
        }
        Pop();
#ifndef VICOMP
    } else {
        if( TOS ){
            // why the heck shouldn't I pop it!
            cs_entry    *crap;

            crap = TOS;
            TOS = TOS->next;
            FreeLabel( crap->end );
            FreeLabel( crap->top );
            MemFree( crap );
        }
    }
#endif
    return( ERR_NO_ERR );

} /* CSFini */

/*
 * CSIf - if {elseif {elseif {...}} {else} endif
 */
void CSIf( void )
{
    Push( CS_IF );
    GenTestCond();
    GenJmpIf( COND_FALSE, TOS->top );

} /* if */

/*
 * CSElseIf - emit elseif labels/branches
 */
void CSElseIf( void )
{
    if( TOS->type != CS_IF ) {
        oopsBob( "elseif", "if" );
    }
    GenJmp( TOS->end );
    GenLabel( TOS->top );
    FreeLabel( TOS->top );
    TOS->top = NewLabel();
    GenTestCond();
    GenJmpIf( COND_FALSE, TOS->top );

} /* CSElseIf */

/*
 * CSElse - else labels/branches
 */
void CSElse( void )
{
    if( TOS->type != CS_IF ) {
        oopsBob( "else", "if" );
    }
    TOS->type = CS_ELSE;
    GenJmp( TOS->end );
    GenLabel( TOS->top );
    TOS->top = NewLabel();

} /* CSElse */

/*
 * CSEndIf - endif labels/branches
 */
void CSEndif( void )
{
    if( TOS->type != CS_IF && TOS->type != CS_ELSE ) {
        oopsBob( "endif", "if" );
    }
    GenLabel( TOS->end );
    GenLabel( TOS->top );
    Pop();

} /*  CSEndIf */

/*
 * CSWhile - loop/endloop - while/endloop - loop/until - while/until
 */
void CSWhile( void )
{
    Push( CS_LOOP );
    GenLabel( TOS->top );
    GenTestCond();
    GenJmpIf( COND_FALSE, TOS->end );

} /* CSWhile */

/*
 * CSLoop - looping labels/branches
 */
void CSLoop( void )
{
    Push( CS_LOOP );
    GenLabel( TOS->top );

} /* CSLoop */

static char _strlw[] = "loop/while";
/*
 * CSEndLoop - endloop labels/branches
 */
void CSEndLoop( void )
{
    if( TOS->type != CS_LOOP ) {
        oopsBob( "endloop", _strlw );
    }
    GenJmp( TOS->top );
    GenLabel( TOS->end );
    Pop();

} /* CSEndLoop */

/*
 * CSUntil - until labels/branches
 */
void CSUntil( void )
{
    if( TOS->type != CS_LOOP ) {
        oopsBob( "until", _strlw );
    }
    GenTestCond();
    GenJmpIf( COND_FALSE, TOS->top );
    GenLabel( TOS->end );
    Pop();

} /* CSUntil */

/*
 * FindLoop - find first loop on control stack
 */
static cs_entry *FindLoop( void )
{
    cs_entry *s;

    s = TOS;
    for( s = TOS; s->type != CS_EOS; s = s->next ) {
        if( s->type == CS_LOOP ) {
            return( s );
        }
    }

    // prevent compiler from giving dead code warning because oopsBob is
    // defined as an aborting function
    if( s->type == CS_EOS ) {
        oopsBob( "break/quif", _strlw );
    }

    return( 0 );

} /* FindLoop */

/*
 * CSContinue - continue branches
 */
void CSContinue( void )
{
    GenJmp( FindLoop()->top );

} /* CSContinue */

/*
 * CSBreak - break labels/branches
 */
void CSBreak( void )
{
    GenJmp( FindLoop()->end );

} /* CSBreak */

/*
 * CSQuif - quit if labels/branches
 */
void CSQuif( void )
{
    GenTestCond();
    GenJmpIf( COND_TRUE, FindLoop()->end );

} /* CSQuif */
