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
* Description:  This file contains "exotic" instantiation tests for the
*               basic_string template. Most users won't do stuff like
*               this, but it's supposed to work regardless. These tests
*               are for the users who are doing these things.
*
****************************************************************************/

#include <string.h>
#include <cctype>
#include <iostream>
#include <limits>
#include <string>

#include "sanity.cpp"

namespace std {

  // ****************************************************************
  // Character traits specialization for type double as "characters."
  // ****************************************************************
  template< >
  struct char_traits< double > {
    typedef double  char_type;
    typedef double  int_type;

    static void assign( char_type &c1, const char_type &c2 )
      { c1 = c2; }

    static bool eq( const char_type &c1, const char_type &c2 )
      { return( c1 == c2 ); }

    static bool lt( const char_type &c1, const char_type &c2 )
      { return( c1 < c2 ); }

    static int compare( const char_type *s1, const char_type *s2, size_t n )
    {
      for( size_t i = 0; i < n; ++i ) {
        if( *s1 < *s2 ) return( -1 );
        if( *s1 > *s2 ) return(  1 );
        ++s1; ++s2;
      }
      return( 0 );
    }

    static size_t length( const char_type *s )
    {
      size_t count = 0;
      while( *s != double( ) ) { ++count; ++s; }
      return( count );
    }

    static const char_type *find( const char_type *s, size_t n, const char_type &a )
    {
      const char_type *result = 0;
      for( size_t i = 0; i < n; ++i ) {
        if( *s == a ) {
          result = s;
          break;
        }
        ++s;
      }
      return( result );
    }

    static char_type *move( char_type *s1, const char_type *s2, size_t n )
    {
      return( static_cast< char_type * >
              ( memmove(s1, s2, n * sizeof( char_type ) ) ) );
    }

    static char_type *copy( char_type *s1, const char_type *s2, size_t n )
    {
      return( static_cast< char_type * >
              ( memcpy(s1, s2, n * sizeof( char_type ) ) ) );
    }

    static char_type *assign( char_type *s, size_t n, char_type a )
    {
      char_type *p = s;
      for( size_t i = 0; i < n; ++i ) {
        *p = a;
        ++p;
      }
      return( s );
    }

    static int_type not_eof( const int_type &c )
      { return( (c != std::numeric_limits< double >::max( ) ) ?
          c : static_cast< int_type >( 0.0 ) ); }

    static char_type to_char_type( const int_type &c )
      { return( static_cast< char_type >(c) ); }

    static int_type to_int_type( const char_type &c )
      { return( static_cast< int_type >(c) ); }

    static bool eq_int_type( const int_type &c1, const int_type &c2 )
      { return( c1 == c2 ); }

    static int_type eof( )
      { return( std::numeric_limits< double >::max( ) ); }
  };

} // End of namespace std.
typedef std::basic_string< double > dstring;


// ***************************************************************
// Character traits specialization for a structured character type.
// ****************************************************************

// std::basic_string can only be instantiated with a POD type.
struct token {
  int  symbol;          // Let's use this as a key field for comparisions.
  char short_name[12];  // The other fields are arbitrary.
  bool processed;

  token() { symbol = -1; }  // This defines "end-of-string".
  token(int s, char *p, bool f) : symbol( s ), processed( f )
    { std::strcpy( short_name, p ); }
};

struct token_traits {
  typedef token char_type;
  typedef token int_type;

  static void assign( char_type &c1, const char_type &c2 )
    { c1 = c2; }

  static bool eq( const char_type &c1, const char_type &c2 )
    { return( c1.symbol == c2.symbol ); }

  static bool lt( const char_type &c1, const char_type &c2 )
  { return( c1.symbol < c2.symbol ); }

  static int compare( const char_type *s1, const char_type *s2, size_t n )
  {
    for( size_t i = 0; i < n; ++i ) {
      if( s1->symbol < s2->symbol ) return( -1 );
      if( s1->symbol > s2->symbol ) return(  1 );
      ++s1; ++s2;
    }
    return( 0 );
  }

  static size_t length( const char_type *s )
  {
    size_t count = 0;
    while( s->symbol != -1 ) { ++count; ++s; }
    return( count );
  }

  static const char_type *find( const char_type *s, size_t n, const char_type &a )
  {
    const char_type *result = 0;
    for( size_t i = 0; i < n; ++i ) {
      if( s->symbol == a.symbol ) {
        result = s;
        break;
      }
      ++s;
    }
    return( result );
  }

  static char_type *move( char_type *s1, const char_type *s2, size_t n )
  {
    return( static_cast< char_type * >
            ( memmove(s1, s2, n * sizeof( char_type ) ) ) );
  }

  static char_type *copy( char_type *s1, const char_type *s2, size_t n )
  {
    return( static_cast< char_type * >
            ( memcpy(s1, s2, n * sizeof( char_type ) ) ) );
  }

  static char_type *assign( char_type *s, size_t n, char_type a )
  {
    char_type *p = s;
    for( size_t i = 0; i < n; ++i ) {
      *p = a;
      ++p;
    }
    return( s );
  }

  static char_type to_char_type( const int_type &c )
  { return( static_cast< char_type >(c) ); }

  static int_type to_int_type( const char_type &c )
  { return( static_cast< int_type >(c) ); }

  static bool eq_int_type( const int_type &c1, const int_type &c2 )
  { return( c1.symbol == c2.symbol ); }
};
typedef std::basic_string< token, token_traits > tstring;


// The following tests are designed to exercise the various special-
// izations to see if the traits type is being used properly. These test
// provide some confidence that "unusual" string types can be generated
// from std::basic_string as allowed for by the standard. These tests
// are not particularly exhaustive, however.

bool wstring_test( )
{
  bool rc = true;

  std::wstring s1( 10, L'x' );
  std::wstring s2( s1 );

  if( s1.size( ) != 10 || INSANE( s1 ) ) {
    std::cout << "wstring FAIL 0001\n"; rc = false;
  }
  if( s2.size( ) != 10 || s1 != s2 || INSANE( s2 ) ) {
    std::cout << "wstring FAIL 0002\n"; rc = false;
  }

  s2 = L"This is a wide character string";
  if( s2.size( ) != 31 ||
      s2 != L"This is a wide character string" || INSANE( s2 ) ) {
    std::cout << "wstring FAIL 0003\n"; rc = false;
  }

  s2 = L'z';
  if( s2.size( ) != 1 || s2 != L"z" || INSANE( s2 ) ) {
    std::cout << "wstring FAIL 0004\n"; rc = false;
  }

  std::wstring s3( 3, L'x' ); s3[2] = L'z';
  std::wstring s4( 1, L'y' );
  s3.insert( 1, s4 );
  if( s3.size( ) != 4 || s3 != L"xyxz" || INSANE( s3 ) ) {
    std::cout << "wstring FAIL 0005\n"; rc = false;
  }

  std::wstring s5( L"Hello, World" );
  std::wstring s6( L"ell" );
  if( s5.find( s6 ) != 1 ) {
    std::cout << "wstring FAIL 0006\n"; rc = false;
  }

  return( rc );
}

bool dstring_test( )
{
  bool rc = true;

  double raw_string[] = {
    1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, double( )
  };
  double raw_value[] = { -1.0, double( ) };

  dstring s1( 10, 1.0 );
  dstring s2( s1 );

  if( s1.size( ) != 10 || INSANE( s1 ) ) {
    std::cout << "dstring FAIL 0001\n"; rc = false;
  }
  if( s2.size( ) != 10 || s1 != s2 || INSANE( s2 ) ) {
    std::cout << "dstring FAIL 0002\n"; rc = false;
  }

  s2 = raw_string;
  if( s2.size( ) != 12 ||
      s2 != raw_string || INSANE( s2 ) ) {
    std::cout << "dstring FAIL 0003\n"; rc = false;
  }

  s2 = -1.0;
  if( s2.size( ) != 1 || s2 != raw_value || INSANE( s2 ) ) {
    std::cout << "dstring FAIL 0004\n"; rc = false;
  }

  dstring s3( 3, 1.0 ); s3[2] = 2.0;
  dstring s4( 1, 3.0 );
  double result[] = { 1.0, 3.0, 1.0, 2.0, double( ) };

  s3.insert( 1, s4 );
  if( s3.size( ) != 4 || s3 != result || INSANE( s3 ) ) {
    std::cout << "dstring FAIL 0005\n"; rc = false;
  }

  double haystack[] = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, double( ) };
  double needle[] = { 2.0, 3.0, 4.0, double( ) };

  dstring s5( haystack );
  dstring s6( needle );
  if( s5.find( s6 ) != 1 ) {
    std::cout << "dstring FAIL 0006\n"; rc = false;
  }

  return( rc );
}

bool istring_test( )
{
  using watcom::istring;

  bool rc = true;

  istring s1( 10, 'x' );
  istring s2( s1 );

  if( s1.size( ) != 10 || INSANE( s1 ) ) {
    std::cout << "istring FAIL 0001\n"; rc = false;
  }
  if( s2.size( ) != 10 || s1 != s2 || INSANE( s2 ) ) {
    std::cout << "istring FAIL 0002\n"; rc = false;
  }

  s2 = "This is a string";
  if( s2.size( ) != 16 ||
      s2 != "THIS IS A STRING" || INSANE( s2 ) ) {
    std::cout << "istring FAIL 0003\n"; rc = false;
  }

  s2 = 'Z';
  if( s2.size( ) != 1 || s2 != "z" || INSANE( s2 ) ) {
    std::cout << "istring FAIL 0004\n"; rc = false;
  }

  istring s3( 3, 'x' ); s3[2] = 'z';
  istring s4( 1, 'y' );
  s3.insert( 1, s4 );
  if( s3.size( ) != 4 || s3 != "XYxz" || INSANE( s3 ) ) {
    std::cout << "istring FAIL 0005\n"; rc = false;
  }

  istring s5( "Hello, World" );
  istring s6( "ELL" );
  if( s5.find( s6 ) != 1 ) {
    std::cout << "istring FAIL 0006\n"; rc = false;
  }

  return( rc );
}

bool tstring_test( )
{
  bool rc = true;

  token raw_string[] = {
    token( 1, "1",  true ), token( 2, "2", false ), token( 3, "3",  true ),
    token( 4, "4", false ), token( 5, "5",  true ), token( 6, "6", false ),
    token( 7, "7",  true ), token( 8, "8", false ), token( 9, "9",  true ),
    token(10,"10", false ), token(11,"11",  true ), token(12,"12", false ),
    token( )
  };
  token raw_value[] = { token( 2, "****", false ), token( ) };

  token value_1( 1, "v1", true );
  token value_2( 2, "v2", true );

  tstring s1( 10, value_1 );
  tstring s2( s1 );

  if( s1.size( ) != 10 || INSANE( s1 ) ) {
    std::cout << "tstring FAIL 0001\n"; rc = false;
  }
  if( s2.size( ) != 10 || s1 != s2 || INSANE( s2 ) ) {
    std::cout << "tstring FAIL 0002\n"; rc = false;
  }

  s2 = raw_string;
  if( s2.size( ) != 12 ||
      s2 != raw_string || INSANE( s2 ) ) {
    std::cout << "tstring FAIL 0003\n"; rc = false;
  }

  s2 = value_2;
  if( s2.size( ) != 1 || s2 != raw_value || INSANE( s2 ) ) {
    std::cout << "tstring FAIL 0004\n"; rc = false;
  }

  tstring s3( 3, raw_string[0] ); s3[2] = value_2;
  tstring s4( 1, raw_string[2] );
  token result[] = { token(1, ".", true), token(3, ".", true),
                     token(1, ".", true), token(2, ".", true), token( ) };

  s3.insert( 1, s4 );
  if( s3.size( ) != 4 || s3 != result || INSANE( s3 ) ) {
    std::cout << "tstring FAIL 0005\n"; rc = false;
  }

  token haystack[] = { token(1, "-",  true), token(2, "+", false),
                       token(3, "*",  true), token(4, "/",  true),
                       token(5, "%", false), token(6, "#",  true),
                       token( ) };
  token needle[] = { token(2, "A",  true), token(3, "B", false),
                     token(4, "C", false), token( ) };

  tstring s5( haystack );
  tstring s6( needle );
  if( s5.find( s6 ) != 1 ) {
    std::cout << "tstring FAIL 0006\n"; rc = false;
  }

  return( rc );
}


int main( )
{
  int rc = 0;
  int original_count = heap_count( );

  try {
    if( !wstring_test( )    || !heap_ok( "t01" ) ) rc = 1;
    if( !dstring_test( )    || !heap_ok( "t02" ) ) rc = 1;
    if( !istring_test( )    || !heap_ok( "t03" ) ) rc = 1;
    if( !tstring_test( )    || !heap_ok( "t04" ) ) rc = 1;
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
