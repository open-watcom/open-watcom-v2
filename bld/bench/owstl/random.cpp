/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2004-2013 The Open Watcom Contributors. All Rights Reserved.
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
* Description: Benchmark program that tests random number generators
*
****************************************************************************/

#include <iostream>
#include "timer.h"
#include <cstdlib>
#include <random>

#include "mt19937ar.c"

static int ans_tr1;
static int ans_orig;

template< typename gen >
double do_tr1( gen &r, long repetitions )
{
    long i;
    
    std::cout<<"start\n";
    TimerOn( );
    
    for( i = 0; i < repetitions; i++ ){
        ans_tr1 = r();
    }
    
    TimerOff( );
    std::cout<<"stop\n";
    
    return( ( TimerElapsed( )/repetitions ) * 1e9 );
}

double do_orig_mt( long repetitions )
{
    init_genrand(5489);
    long i;
    
    std::cout<<"start\n";
    TimerOn( );
    
    for( i = 0; i < repetitions; i++ ){
        ans_orig = genrand_int32();
    }
    
    TimerOff( );
    std::cout<<"stop\n";
    
    return( ( TimerElapsed( )/repetitions ) * 1e9 );
}


int main( )
{
    long const repetitions = 100000000;
    using namespace std;
    
    std::tr1::mt19937 r0;
    cout << "\ncalling tr1::mt " << repetitions << " times\n";
    cout << do_tr1( r0, repetitions ) << " ns/pass\n";

    cout << "\ncalling original mt " << repetitions << " times\n";
    cout << do_orig_mt( repetitions ) << " ns/pass\n";
    
    if( ans_tr1 == ans_orig ) std::cout<<"\nresults agree\n";
    else cout<<"\nlibrary error - results disagree!\n";
    
    tr1::minstd_rand r1;
    cout << "\ncalling tr1::minstd " << repetitions << " times\n";
    cout << do_tr1( r1, repetitions ) << " ns/pass\n";
    cout<< "ans = "<<ans_tr1<<"\n";
    
    _watcom::lcg32 r2;
    cout << "\ncalling _watcom::lcg32 " << repetitions << " times\n";
    cout << do_tr1( r2, repetitions ) << " ns/pass\n";
    cout<< "ans = "<<ans_tr1<<"\n";
    
    return( 0 );
}
