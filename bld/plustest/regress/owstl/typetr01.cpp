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
* Description:  Use this file as a template for creating new tests.
*
****************************************************************************/

#include <iostream>
#include <type_traits>
#include "sanity.cpp"

using namespace std::tr1;

/* ------------------------------------------------------------------
 * this function doesn't really do anything 
 * it is just one big compile time test 
 */
bool the_test( )
{
    // test is_*
    static_assert( is_arithmetic<void>::value == false, "fail");
    static_assert( is_arithmetic<int>::value == true, "fail");
    static_assert( is_arithmetic<float>::value == true, "fail");

    static_assert( is_const<long double const>::value == true, "fail" );
    static_assert( is_const<double>::value == false, "fail" );
    static_assert( is_const<int const *>::value == false, "fail" );
    static_assert( is_const<int * const>::value == true, "fail" );
    static_assert( is_const<float volatile const>::value == true, "fail" );
    
    static_assert( is_floating_point<double>::value == true, "fail" );
    static_assert( is_floating_point<float>::value == true, "fail" );
    static_assert( is_floating_point<long double const>::value == true, "fail" );
    static_assert( is_floating_point<float *>::value == false, "fail" );
    
    static_assert( is_fundamental<int>::value == true, "fail" );
    static_assert( is_fundamental<bool>::value == true, "fail" );
    static_assert( is_fundamental<double volatile>::value == true, "fail" );
    static_assert( is_fundamental<char const>::value == true, "fail" );
    static_assert( is_fundamental<void *>::value == false, "fail" );
    static_assert( is_fundamental<wchar_t>::value == true, "fail" ); 
    
    static_assert( is_integral<void>::value == false, "fail" );
    static_assert( is_integral<void const>::value == false, "fail" );
    static_assert( is_integral<int>::value == true, "fail" );
    static_assert( is_integral<short>::value == true, "fail" );
    static_assert( is_integral<const int>::value == true, "fail" );
    static_assert( is_integral<unsigned int>::value == true, "fail" );
    static_assert( is_integral<signed int>::value == true,  "fail" );
    static_assert( is_integral<int volatile>::value == true, "fail" );

    static_assert( is_pointer<void>::value == false, "fail" );
    static_assert( is_pointer<void*>::value == true, "fail" );
    
    static_assert( is_reference<char &>::value == true, "fail" );
    static_assert( is_reference<void>::value == false, "fail" );
    static_assert( is_reference<char & const>::value == true, "fail" );
    
    static_assert( is_void<void>::value == true, "fail" );
    static_assert( is_void<int>::value == false, "fail" );
    
    static_assert( is_volatile<long volatile>::value == true, "fail" );
    static_assert( is_volatile<double>::value == false, "fail" );
    static_assert( is_volatile<int volatile *>::value == false, "fail" );
    static_assert( is_volatile<int * volatile>::value == true, "fail" );
    static_assert( is_volatile<float volatile const>::value == true, "fail" );
    
    static_assert( is_same<int, int>::value == true, "fail" );
    static_assert( is_same<int const, int>::value == false, "fail" );
    static_assert( is_same<char, int>::value == false, "fail" );
    static_assert( is_same<double, float>::value == false, "fail" );
    static_assert( is_same<char *, char>::value == false, "fail" );
    static_assert( is_same<long &, long>::value == false, "fail" );
    static_assert( is_same<signed int, int>::value == true, "fail" );
    static_assert( is_same<unsigned long, long>::value == false, "fail" );
    
    // test remove_*
    static_assert( is_const<remove_const<int>::type>::value == false, "fail" );
    static_assert( is_const<remove_const<int const>::type>::value == false, "fail" );
    static_assert( is_const<remove_const<int const volatile>::type>::value == false, "fail" );
    static_assert( is_volatile<remove_const<int const volatile>::type>::value == true, "fail" );
    
    static_assert( is_pointer<remove_pointer<int *>::type>::value == false, "fail" );
    static_assert( is_pointer<remove_pointer<int>::type>::value == false, "fail" );
    static_assert( is_pointer<remove_pointer<int * const>::type>::value == false, "fail" );
    static_assert( is_pointer<remove_pointer<int **>::type>::value == true, "fail" );
    
    static_assert( is_reference<remove_reference<int &>::type>::value == false, "fail" );
    static_assert( is_reference<remove_reference<int>::type>::value == false, "fail" );
    static_assert( is_reference<remove_reference<int & const>::type>::value == false, "fail" );
    
    static_assert( is_volatile<remove_volatile<int>::type>::value == false, "fail" );
    static_assert( is_volatile<remove_volatile<int volatile>::type>::value == false, "fail" );
    static_assert( is_volatile<remove_volatile<int const volatile>::type>::value == false, "fail" );
    static_assert( is_const<remove_volatile<int const volatile>::type>::value == true, "fail" );
    
    // test add_*
    static_assert( is_const<add_const<int>::type>::value == true, "fail" );
    static_assert( is_const<add_const<int volatile>::type>::value == true, "fail" );
    static_assert( is_const<add_const<int const>::type>::value == true, "fail" );
    
    static_assert( is_const<add_cv<int>::type>::value == true, "fail" );
    static_assert( is_volatile<add_cv<int>::type>::value == true, "fail" );
    static_assert( is_volatile<add_cv<int const>::type>::value == true, "fail" );
    static_assert( is_const<add_cv<int const>::type>::value == true, "fail" );
    
    static_assert( is_reference<add_reference<int &>::type>::value == true, "fail" );
    static_assert( is_reference<add_reference<int>::type>::value == true, "fail" );
    static_assert( is_reference<add_reference<int & const>::type>::value == true, "fail" );
    
    static_assert( is_volatile<add_volatile<int>::type>::value == true, "fail" );
    static_assert( is_volatile<add_volatile<int const>::type>::value == true, "fail" );
    static_assert( is_volatile<add_volatile<int volatile>::type>::value == true, "fail" );
    
    return( true );
}

int main( )
{
    int rc = 0;
    int original_count = heap_count( );
    
    try {
        if( !the_test( )    || !heap_ok( "t01" ) ) rc = 1;
    }
    catch( ... ) {
        std::cout << "Unexpected exception of unexpected type.\n";
        rc = 1;
    }
    
    if( heap_count( ) != original_count ) {
        std::cout << "Possible memory leak!\n";
        rc = 1;
    }
    return( rc );
}
