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


#ifndef __ASSURE_H__
#define __ASSURE_H__

#if defined( DEBUG_ASSERT ) && !defined( NDEBUG )

    void Assure( int condition, char * msg, int fatal = -1 );
    void NoDefault( char * file, int line );
    void Assertion( int condition, char * cond, char * file, int line );

    #define REQUIRE( x, m )          Assure( (int)(x), (char *) (m) )
    #define ASSERT( x, m, f )        Assure( (int)(x), (char *) (m), (int)(f) )
    #define NODEFAULT                NoDefault( __FILE__, __LINE__ )
    #define ASSERTION( x )           Assertion( (int)(x), #x, __FILE__, __LINE__ )

#else

    #define REQUIRE( x, m )
    #define ENSURE( x, m )
    #define ASSERT( x, m, a )
    #define NODEFAULT
    #define ASSERTION( x )

#endif


#endif // __ASSURE_H__
