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


// Note: some other comilers do not handle "continue" statements correctly.
//       Some other compilers exhibit non-standard behaviour with "break"
//       also.
//
// The ARM (section 6.6.2) and the ANSI draft (section 6.6.2) both specify
// that "continue" behaves equivalently to a "goto" to a generated label
// just before the the end of the current loop.
//
// Thus, functions c0,c1,c2 are equivalent to functions f0,f1,f2.

// #pragma on (print_ptree)
// #pragma on (dump_ptree)
// #pragma on (dump_emit_ic)
// #pragma on (dump_lines)
// #pragma on (dump_tokens)

struct S
{
    S();
};


void goop( int );
void poop( const S& );

void f0( int i )
{
    for( ; i>0; --i ) {
        if( i == 3 ) goto lab;
        const int b = 6;
        goop( b );
lab:    continue;
    }
}

void f1( int i )
{
    for( ; i>0; --i ) {
        if( i == 3 ) goto lab;
        int b = 6;
        goop( b );
lab:    continue;
    }
}

void f2( int i )
{
    for( ; i>0; --i ) {
        if( i == 3 ) goto lab;
        S s;
        poop( s );
lab:    continue;
    }
}

void c0( int i )
{
    for( ; i>0; --i ) {
        if( i == 3 ) continue;
        const int b = 6;
        goop( b );
    }
}

void c1( int i )
{
    for( ; i>0; --i ) {
        if( i == 3 ) continue;
        int b = 6;
        goop( b );
    }
}

void c2( int i )
{
    for( ; i>0; --i ) {
        if( i == 3 ) continue;
        S s;
        poop( s );
lab:    continue;
    }
}

// #pragma on (dump_exec_ic)
// #pragma on ( dump_scopes )
