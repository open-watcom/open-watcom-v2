/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2006 The Open Watcom Contributors. All Rights Reserved.
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
* Description: This file contains the functional tests for smart pointers shared_ptr and weak_ptr
*
****************************************************************************/

#include <memory>
#include <iostream>
#include <string>

#include "sanity.cpp"

class A { 
public:
    int tag;
    A( ) : tag( 0 ) { }
    virtual ~A( ) { }
};

class B : public A {
public:
    B( bool& _deleted ) : deleted( _deleted ) { }
    ~B( ) { deleted = true; }
private:
    bool& deleted;
};

class C : public A { };

class U { };

class D { 
public:
    template< class T >
    void operator( ) ( T *p ) const
        { delete p; }
};

bool shared_construct_destruct_test( )
{
    // shared_ptr( )
    {
        std::shared_ptr< A > s1;
        if( !(s1.use_count( ) == 0 && s1.get( ) == 0) ) FAIL;
    }

    // template< class Y > explicit shared_ptr( Y *p )
    {
        bool deleted = false;
        B *p = new B( deleted );
        {
            std::shared_ptr< A > s( p );
            if( !(s.use_count( ) == 1 && s.get( ) == p) ) FAIL;
        }
        if( !deleted ) FAIL;
    }

    // template< class Y, class D > shared_ptr( Y *p, D d )
    {
        bool deleted = false;
        B *p = new B( deleted );
        {
            std::shared_ptr< A > s( p, D( ) );
            if( !(s.use_count( ) == 1 && s.get( ) == p) ) FAIL;
        }
        if( !deleted ) FAIL;
    }

    // shared_ptr( shared_ptr const &r )
    {
        std::shared_ptr< A > s1;
        std::shared_ptr< A > s2 = s1;
        if( !(s2.use_count( ) == 0 && s2.get( ) == 0) ) FAIL;

        std::shared_ptr< A > s3( new A );
        std::shared_ptr< A > s4 = s3;
        if( !(s3.use_count( ) == s4.use_count( ) && s3.get( ) == s4.get( )) ) FAIL;
        if( s3.use_count( ) != 2 ) FAIL;
    }

    // template< class Y > shared_ptr( shared_ptr< Y > const &r )
    {
        std::shared_ptr< B > s1;
        std::shared_ptr< A > s2( s1 );
        if( !(s2.use_count( ) == 0 && s2.get( ) == 0) ) FAIL;

        bool deleted = false;
        {
            std::shared_ptr< B > s3( new B( deleted ) );
            {
                std::shared_ptr< A > s4( s3 );
                if( !(s3.use_count( ) == s4.use_count( ) && s3.get( ) == s4.get( )) ) FAIL;
                if( s3.use_count( ) != 2 ) FAIL;
            }
            if( s3.use_count( ) != 1 ) FAIL;
            if( deleted ) FAIL;
        }
        if( !deleted ) FAIL;
    }

    // template< class Y > explicit shared_ptr( weak_ptr< Y > const &r )
    {
        bool deleted = false;
        std::weak_ptr< B > w1;
        {
            std::shared_ptr< B > s1( new B( deleted ) );
            w1 = s1;
            std::shared_ptr< A > s2( w1 );
            if( !(s2.use_count( ) == w1.use_count( ) && s2.get( ) == s1.get( )) ) FAIL;
            if( s2.use_count( ) != 2 ) FAIL;
        }
        if( !deleted ) FAIL;

        bool caught_bad_weak_ptr = false;
        try {
            std::shared_ptr< A > s3( w1 );
        }
        catch( std::bad_weak_ptr& e ) {
            caught_bad_weak_ptr = true;
            if( std::string( e.what( ) ) != "bad_weak_ptr" ) FAIL; 
        }
        if( !caught_bad_weak_ptr ) FAIL;
    }
    
    // template< class Y > explicit shared_ptr( auto_ptr< Y > &r ) : ptr( r.get( ) )
    {
        bool deleted = false;
        std::auto_ptr< B > a1( new B( deleted ) );
        {
            A *a1_ptr = a1.get( );
            std::shared_ptr< A > s1( a1 );
            if( s1.get( ) != a1_ptr ) FAIL;
            if( !(s1.use_count( ) == 1 && a1.get( ) == 0) ) FAIL;
            if( deleted ) FAIL;
        }
        if( !deleted ) FAIL;
    }

    return( true );
}

bool shared_assignment_test( )
{
    // shared_ptr& operator=( shared_ptr const& r )
    {
        bool deleted1 = false;
        bool deleted2 = false;
        {
            std::shared_ptr< B > s1( new B( deleted1 ) );
            std::shared_ptr< B > s2( new B( deleted2 ) );
            s1 = s2;
            if( !deleted1 ) FAIL;
            if( s1.get( ) != s2.get( ) || s1.use_count( ) != 2 ) FAIL;
            s1 = s2;
            if( s1.get( ) != s2.get( ) || s1.use_count( ) != 2 ) FAIL;
            s1 = s1;
            if( s1.get( ) != s2.get( ) || s1.use_count( ) != 2 ) FAIL;    
        }
        if( !deleted2 ) FAIL;
    }

    // template< class Y > shared_ptr& operator=( shared_ptr< Y > const &r )
    {
        bool deleted1 = false;
        bool deleted2 = false;
        {
            std::shared_ptr< A > s1( new B( deleted1 ) );
            std::shared_ptr< B > s2( new B( deleted2 ) );
            s1 = s2;
            if( !deleted1 ) FAIL;
            if( s1.get( ) != s2.get( ) || s1.use_count( ) != 2 ) FAIL;
            s1 = s2;
            if( s1.get( ) != s2.get( ) || s1.use_count( ) != 2 ) FAIL;
            s1 = s1;
            if( s1.get( ) != s2.get( ) || s1.use_count( ) != 2 ) FAIL;
        }
        if( !deleted2 ) FAIL;
    }

    // template< class Y > shared_ptr& operator=( auto_ptr< Y > &r )
    {
        bool deleted1 = false;
        bool deleted2 = false;
        {
            std::shared_ptr< A > s1( new B( deleted1 ) );
            std::auto_ptr< B > a1( new B( deleted2 ) );
            s1 = a1;
            if( !deleted1 ) FAIL;
            if( deleted2 ) FAIL;
            if( s1.use_count( ) != 1 ) FAIL;
            if( a1.get( ) ) FAIL;
        }
        if( !deleted2 ) FAIL;
    }
    
    return( true );
}

bool shared_modifiers_test( )
{
    // void swap( shared_ptr &r )
    {
        std::shared_ptr< A > s1( new A );
        std::shared_ptr< A > s1_1 = s1; // to increase use_count
        A *ptr1 = s1.get( );
        long use_count1 = s1.use_count( );

        bool deleted = false;
        std::shared_ptr< A > s2( new B( deleted ) );
        A *ptr2 = s2.get( );
        long use_count2 = s2.use_count( );

        s1.swap( s2 );
        if( deleted ) FAIL;
        if( !(s1.get( ) == ptr2 && s1.use_count( ) == use_count2 && 
            s2.get( ) == ptr1 && s2.use_count( ) == use_count1) ) FAIL;
    }

    // void reset( )
    {
        bool deleted = false;
        std::shared_ptr< B > s1( new B( deleted ) );
        s1.reset( );
        if( !(s1.get( ) == 0 && s1.use_count( ) == 0 && deleted) ) FAIL;
    }

    // template< class Y > void reset( Y *p )
    {
        bool deleted1 = false;
        bool deleted2 = false;
        {
            std::shared_ptr< A > s1( new B( deleted1 ) );
            B *b = new B( deleted2 );
            s1.reset( b );
            if( !deleted1 || deleted2 ) FAIL;
            if( !(s1.get( ) == b && s1.use_count( ) == 1) ) FAIL;
        }
        if( !deleted2 ) FAIL;
    }

    // template< class Y, class D > void reset( Y *p, D d )
    {
        bool deleted1 = false;
        bool deleted2 = false;
        {
            std::shared_ptr< A > s1( new B( deleted1 ) );
            B *b = new B( deleted2 );
            s1.reset( b, D( ) );
            if( !deleted1 || deleted2 ) FAIL;
            if( !(s1.get( ) == b && s1.use_count( ) == 1) ) FAIL;
        }
        if( !deleted2 ) FAIL;
    }

    return( true );
}

bool shared_observers_test( )
{
    // T *get( ) const                    
    {
        A *a = new A;
        std::shared_ptr< A > s1( a );
        std::shared_ptr< A > s2;
        if( !(s1.get( ) == a && s2.get( ) == 0) ) FAIL;
    }

    // T &operator*( ) const
    {
        A *a = new A;
        std::shared_ptr< A > s1( a );
        A& b = *s1;
        b.tag = 5;
        if( !(a->tag == 5) ) FAIL;
    }

    // T *operator->( ) const
    {
        A *a = new A;
        std::shared_ptr< A > s1( a );
        a->tag = 5;
        if( s1->tag != 5 ) FAIL;
    }

    // long use_count( ) const
    {
        std::shared_ptr< A > s1;
        if( s1.use_count( ) != 0 ) FAIL;
        std::shared_ptr< A> s2( new A );
        if( s2.use_count( ) != 1 ) FAIL;
        s1 = s2;
        if( s2.use_count( ) != 2 || s1.use_count( ) != 2 ) FAIL;
    }

    // bool unique( ) const
    {
        std::shared_ptr< A > s1;
        if( s1.unique( ) ) FAIL;
        std::shared_ptr< A> s2( new A );
        if( !s2.unique( ) ) FAIL;
        s1 = s2;
        if( s1.unique( ) || s2.unique( ) ) FAIL;
    }

    // operator bool( ) const
    {
        std::shared_ptr< A > s1;
        if( s1 ) FAIL;
        std::shared_ptr< A> s2( new A );
        if( !s2 ) FAIL;
    }
    
    return( true );
}

bool shared_comparison_test( )
{
    //template< class T, class U > bool operator==( shared_ptr< T > const &a, 
    //                                              shared_ptr< U > const &b )
    //template< class T, class U > bool operator!=( shared_ptr< T > const &a,
    //                                              shared_ptr< U > const &b )
    {
        bool deleted = false;
        std::shared_ptr< B > s1( new B( deleted ) );
        std::shared_ptr< A > s2( s1 );
        if( !( s1 == s2 ) ) FAIL;
        if( s1 != s2 ) FAIL;
        std::shared_ptr< A > s3( new A );
        if( std::operator==( s1, s3 ) ) FAIL; // if we comment this line the next will FAIL for some reasons
        if( s1 == s3 ) FAIL;
        if( !std::operator!=( s1, s3 ) ) FAIL; // if we comment this line the next will FAIL for some reasons
        if( !(s1 != s3) ) FAIL;
    }


    //template< class T, class U > bool operator<( shared_ptr< T > const &a, 
    //                                             shared_ptr< U > const &b )
    {
        // let's check strict weak ordering...
        {
            std::shared_ptr< A > s1( new A );
            std::shared_ptr< U > s2( new U );
            std::shared_ptr< D > s3( new D );

            bool b1 = s1 < s2;
            bool b1_ = s2 < s1;
            bool b2 = s1 < s3;
            bool b2_ = s3 < s1;
            bool b3 = s2 < s3;
            bool b3_ = s3 < s2;

            // irreflexivity
            if( s1 < s1 ) FAIL;
            if( s2 < s2 ) FAIL;
            if( s3 < s3 ) FAIL;

            // antisymmetry
            if( b1 && b1_ ) FAIL;
            if( b2 && b2_ ) FAIL;
            if( b3 && b3_ ) FAIL;
            
            // transitivity
            if( b1 && b3 && !b2 ) FAIL;
            if( b1_ && b2 && !b3 ) FAIL;
            if( b2 && b3_ && !b1 ) FAIL;
            if( b2_ && b1 && !b3_ ) FAIL;
            if( b3 && b2_ && !b1_ ) FAIL;
            if( b3_ && b1_ && !b2_ ) FAIL;
        }
        // transitivity of equivalence
        {
            std::shared_ptr< A > s2( new A );
            std::shared_ptr< A > s1 = s2;
            std::shared_ptr< A > s3 = s1;
            bool e1 = !( s1 < s2 ) && !( s2 < s1 );
            bool e2 = !( s2 < s3 ) && !( s3 < s2 );
            bool e3 = !( s1 < s3 ) && !( s3 < s1 );
            if( !e1 ) FAIL;
            if( !e3 ) FAIL;
            if( !e2 ) FAIL;
        }
    }

    return( true );
}


// will be uncommented once iostreams are implemented
//bool shared_io_test( )
//{
//    std::shared< A > a( new A );
//    std::ostringstream oss1;
//    oss1 << a;
//    std::ostringstream oss2;
//    oss2 << a.get( );
//    if( oss1.str( ) != oss2.str( ) ) FAIL;
//}

bool shared_specialized_test( )
{
    // template< class T >
    // void swap( shared_ptr< T > &a, shared_ptr< T > &b )
    A *a1 = new A;
    A *a2 = new A;
    std::shared_ptr< A > s1( a1 );
    std::shared_ptr< A > s2( a2 );
    std::shared_ptr< A > s3( s2 );
    long c1 = s1.use_count( );
    long c2 = s2.use_count( );
    std::swap( s1, s2 );
    if( s1.get( ) != a2 ) FAIL;
    if( s2.get( ) != a1 ) FAIL;
    if( s1.use_count( ) != c2 ) FAIL;
    if( s2.use_count( ) != c1 ) FAIL;

    return( true );
}

// will be implemented once the template feature is resolved (it is required now that for function template all template arguments
// must be in the function argument list)
bool shared_casts_test( )
{
    return( true );
}

bool weak_construct_destruct_test( )
{
    {
        std::weak_ptr< A > w;
        if( w.use_count( ) != 0 ) FAIL;
    }

    {
        bool deleted = false;
        {
            std::shared_ptr< B > s( new B( deleted ) );
            {
                std::weak_ptr< A > w1( s );
                if( !(s.use_count( ) == w1.use_count() && s.get( ) == w1.get( )) ) FAIL;

                std::weak_ptr< B > w2( s );
                if( !(s.use_count( ) == w2.use_count() && s.get( ) == w2.get( )) ) FAIL;

                std::weak_ptr< A > w3( w2 );
                if( !(w3.use_count( ) == w2.use_count() && w3.get( ) == w2.get( )) ) FAIL;

                std::weak_ptr< A > w4( w3 );
                if( !(w4.use_count( ) == w3.use_count() && w4.get( ) == w3.get( )) ) FAIL;
            }
            if( deleted ) FAIL;
        }
        if( !deleted ) FAIL;
    }


    return( true );
}

bool weak_assignment_test( )
{
    //weak_ptr &operator=( weak_ptr const &r )
    {
        std::shared_ptr< A > s( new A );
        std::weak_ptr< A > w1( s );
        std::weak_ptr< A > w2;
        w2 = w1;
        if( w2.get( ) != w1.get( ) || w2.use_count( ) != w1.use_count( ) ) FAIL;
    }

    // template< class Y > weak_ptr &operator=( weak_ptr< Y > const &r )
    {
        std::shared_ptr< C > s( new C );
        std::weak_ptr< C > w1( s );
        std::weak_ptr< A > w2;
        w2 = w1;
        if( w2.get( ) != w1.get( ) || w2.use_count( ) != w1.use_count( ) ) FAIL;
    }

    // template< class Y > weak_ptr &operator=( shared_ptr< Y > const &r )
    {
        std::shared_ptr< C > s( new C );
        std::weak_ptr< A > w;
        w = s;
        if( w.get( ) != s.get( ) || w.use_count( ) != s.use_count( ) ) FAIL;
    }
    
    return( true );
}

bool weak_modifiers_test( )
{
    //void swap( weak_ptr &r )
    {
        std::shared_ptr< A > s1( new A );
        std::shared_ptr< A > s1_1 = s1; // to increase use_count
        A *ptr1 = s1.get( );
        long use_count1 = s1.use_count( );

        bool deleted = false;
        std::shared_ptr< A > s2( new B( deleted ) );
        A *ptr2 = s2.get( );
        long use_count2 = s2.use_count( );

        std::weak_ptr< A > w1( s1 );
        std::weak_ptr< A > w2( s2 );

        w1.swap( w2 );
        if( deleted ) FAIL;
        if( !(w1.get( ) == ptr2 && w1.use_count( ) == use_count2 && 
            w2.get( ) == ptr1 && w2.use_count( ) == use_count1) ) FAIL;
    }

    // void reset( )
    {
        bool deleted = false;
        std::shared_ptr< B > s( new B( deleted ) );
        std::weak_ptr< B > w = s;
        w.reset( );
        if( !(w.get( ) == 0 && w.use_count( ) == 0 && !deleted) ) FAIL;
    }

    return( true );
}

bool weak_observers_test( )
{
    // long use_count( ) const
    {
        std::weak_ptr< A > w;
        if( w.use_count( ) != 0 ) FAIL;
        std::shared_ptr< A > s1( new A );
        w = s1;
        if( w.use_count( ) != 1 ) FAIL;
        std::shared_ptr< A > s2( s1 );
        if( w.use_count( ) != 2 ) FAIL;
        std::shared_ptr< A > s3 ( w );
        if( w.use_count( ) != 3 ) FAIL;
    }

    // bool expired( ) const
    {
        std::weak_ptr< A > w;
        if( !w.expired( ) ) FAIL;
        {
            std::shared_ptr< A > s( new A );
            w = s;
            if( w.expired( ) ) FAIL;
        }
        if( !w.expired( ) ) FAIL;
    }

    // shared_ptr< T > lock( ) const
    {
        std::weak_ptr< A > w;
        std::shared_ptr< A > s1 = w.lock( );
        if( s1.use_count( ) != 0 || s1.get( ) != 0 ) FAIL;
        std::shared_ptr< A > s2( new A );
        w = s2;
        std::shared_ptr< A > s3 = w.lock( );
        if( s2.use_count( ) != s3.use_count( ) || s2.get( ) != s3.get( ) ) FAIL;
    }

    return( true );
}

bool weak_comparison_test( )
{
    {
        std::shared_ptr< A > s1( new A );
        std::weak_ptr< A > w1( s1 );
        std::shared_ptr< U > s2( new U );
        std::weak_ptr< U > w2( s2 );
        std::shared_ptr< D > s3( new D );
        std::weak_ptr< D > w3( s3 );

        bool b1 = w1 < w2;
        bool b1_ = w2 < w1;
        bool b2 = w1 < w3;
        bool b2_ = w3 < w1;
        bool b3 = w2 < w3;
        bool b3_ = w3 < w2;

        // irreflexivity
        if( w1 < w1 ) FAIL;
        if( w2 < w2 ) FAIL;
        if( w3 < w3 ) FAIL;

        // antisymmetry
        if( b1 && b1_ ) FAIL;
        if( b2 && b2_ ) FAIL;
        if( b3 && b3_ ) FAIL;
        
        // transitivity
        if( b1 && b3 && !b2 ) FAIL;
        if( b1_ && b2 && !b3 ) FAIL;
        if( b2 && b3_ && !b1 ) FAIL;
        if( b2_ && b1 && !b3_ ) FAIL;
        if( b3 && b2_ && !b1_ ) FAIL;
        if( b3_ && b1_ && !b2_ ) FAIL;
    }
    // transitivity of equivalence
    {
        std::shared_ptr< A > s2( new A );
        std::weak_ptr< A > w2( s2 );
        std::weak_ptr< A > w1 = w2;
        std::weak_ptr< A > w3 = w1;
        bool e1 = !( w1 < w2 ) && !( w2 < w1 );
        bool e2 = !( w2 < w3 ) && !( w3 < w2 );
        bool e3 = !( w1 < w3 ) && !( w3 < w1 );
        if( !e1 ) FAIL;
        if( !e3 ) FAIL;
        if( !e2 ) FAIL;
    }
    
    return( true );
}

bool weak_specialized_test( )
{
    //template< class T >
    //void swap( weak_ptr< T > &a, weak_ptr< T > &b )
    A *a1 = new A;
    A *a2 = new A;
    std::shared_ptr< A > s1( a1 );
    std::shared_ptr< A > s2( a2 );
    std::shared_ptr< A > s3( s2 );
    long c1 = s1.use_count( );
    long c2 = s2.use_count( );
    std::weak_ptr< A > w1( s1 );
    std::weak_ptr< A > w2( s2 );
    std::swap( w1, w2 );
    if( w1.get( ) != a2 ) FAIL;
    if( w2.get( ) != a1 ) FAIL;
    if( w1.use_count( ) != c2 ) FAIL;
    if( w2.use_count( ) != c1 ) FAIL;

    return( true );
}

class E : public std::enable_shared_from_this< E > { };

bool enable_shared_from_this_test( )
{
    E *e1 = new E;
    std::shared_ptr< E > s1( e1 );
    std::shared_ptr< E > s2 = e1->shared_from_this( );
    if( e1 != s2.get( ) ) FAIL;
    
    // check that copy constructor and assignment operator of std::enable_shared_from_this have no effects
    E *e2 = new E( *e1 );
    bool caught = false;
    try
    {
        std::shared_ptr< E > s3 = e2->shared_from_this( );
    }
    catch( std::bad_weak_ptr& )
    {
        caught = true;
    }
    delete e2;
    if( !caught ) FAIL;

    E *e3 = new E;
    *e3 = *e1;
    caught = false;
    try
    {
        std::shared_ptr< E > s4 = e3->shared_from_this( );
    }
    catch( std::bad_weak_ptr& )
    {
        caught = true;
    }
    delete e3;
    if( !caught ) FAIL;
    
    return( true );
}

int main( )
{
    int rc = 0;
    int original_count = heap_count( );

    try {
        if( !shared_construct_destruct_test( )        || !heap_ok( "t01" ) ) rc = 1;
        if( !shared_assignment_test( )                || !heap_ok( "t02" ) ) rc = 1;
        if( !shared_modifiers_test( )                 || !heap_ok( "t03" ) ) rc = 1;
        if( !shared_observers_test( )                 || !heap_ok( "t04" ) ) rc = 1;
        if( !shared_comparison_test( )                || !heap_ok( "t05" ) ) rc = 1;
        //if( !shared_io_test( )                        || !heap_ok( "t06" ) ) rc = 1;
        if( !shared_specialized_test( )               || !heap_ok( "t07" ) ) rc = 1;
        if( !shared_casts_test( )                     || !heap_ok( "t08" ) ) rc = 1;
        if( !weak_construct_destruct_test( )          || !heap_ok( "t09" ) ) rc = 1;
        if( !weak_assignment_test( )                  || !heap_ok( "t10" ) ) rc = 1;
        if( !weak_modifiers_test( )                   || !heap_ok( "t11" ) ) rc = 1;
        if( !weak_observers_test( )                   || !heap_ok( "t12" ) ) rc = 1;
        if( !weak_comparison_test( )                  || !heap_ok( "t13" ) ) rc = 1;
        if( !weak_specialized_test( )                 || !heap_ok( "t14" ) ) rc = 1;
        if( !enable_shared_from_this_test( )          || !heap_ok( "t15" ) ) rc = 1;
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
