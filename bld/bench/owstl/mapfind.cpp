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
* Description: Benchmark program that tests finds in map
*
****************************************************************************/

#include <iostream>
#include <map>
#include "timer.h"
#include <cstdlib>
#include "testdata.hpp"
#include <functional>

int comparecount = 0;
struct cmp{
    bool operator()( int const x, int const y ) const { comparecount++; return( x < y ); }
};

double doit( TestData const & data, int mapsize, int repetitions )
{
    typedef std::map< int, int, cmp > m_t;
    m_t m;
    int i, j;
    
    for( j = 0; j < mapsize; j++ ){
        m.insert( m_t::value_type(data[j], j) );
    }
    
    comparecount = 0;
    std::cout<<"start\n";
    TimerOn( );
    
    for( i = 0; i < repetitions; i++ ){
        for( j = 0; j < mapsize; j++ ){
            m.find( data[j] );
        }
    }
    
    TimerOff( );
    std::cout<<"stop\n";
    
    return( ( TimerElapsed( )/repetitions ) * 1000 );
}

int main( )
{
    int const repetitions = 1000;
    int const mapsize = 20000;
    
    TestData data(mapsize);
    data.fill_linear();
    
    std::cout << "\nfinding " << mapsize
              << " elements in map, linear order, "
              << repetitions << " times\n";
    std::cout << doit( data, mapsize, repetitions ) << " ms/pass\n";
    std::cout << "compare called " << comparecount/repetitions << " times/pass\n";

    data.fill_rand();

    std::cout << "\nfinding " << mapsize
              << " elements in map, random order, "
              << repetitions << " times\n";
    std::cout << doit( data, mapsize, repetitions ) << " ms/pass\n";
    std::cout << "compare called " << comparecount/repetitions << " times/pass\n";
    
    return( 0 );
}
