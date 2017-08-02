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
* Description:  This file contains functional tests for std::string.
*               Ideally it should carefully exercise every method.
*               Although currently most methods are checked for basic
*               operation, many more tests could (and should) be added.
*
****************************************************************************/

#include <iostream>
#include <string>

#include "sanity.cpp"

// The size at when a std::string first has to reallocate. Many of the
// tests below "hard code" this value in the sense that test string
// literals are used that are just smaller or just larger than this
// size. It would be better to generate those test strings at run time
// after looking up the target string's capacity. That way changes to
// std::string's memory allocation policy would not require changes to
// this test program (at least, that would be the hope).
//
const std::string::size_type base_size = 8;

bool construct_test( )
{
  bool rc = true;
  std::string s1;
  std::string s2( "Hello, World" );
  std::string s3( "Hello, World", 3 );
  std::string s4( s2 );
  std::string s5( s2, 7, 2 );
  std::string s6( s2, 7, 1024 );
  std::string s7( s2, 1 );
  std::string s8( 16, 'x' );

  if( s1.size( ) !=  0 || s1 != ""             || INSANE( s1 ) ) FAIL
  if( s2.size( ) != 12 || s2 != "Hello, World" || INSANE( s2 ) ) FAIL
  if( s3.size( ) !=  3 || s3 != "Hel"          || INSANE( s3 ) ) FAIL
  if( s4.size( ) != 12 || s4 != "Hello, World" || INSANE( s4 ) ) FAIL
  if( s5.size( ) !=  2 || s5 != "Wo"           || INSANE( s5 ) ) FAIL
  if( s6.size( ) !=  5 || s6 != "World"        || INSANE( s6 ) ) FAIL
  if( s7.size( ) != 11 || s7 != "ello, World"  || INSANE( s7 ) ) FAIL
  if( s8.size( ) != 16 || s8 != "xxxxxxxxxxxxxxxx" || INSANE( s8 ) ) FAIL

  return( rc );
}


bool assign_test( )
{
  bool rc = true;
  std::string s1( 5, 'x' );
  const char *shrt_string = "Shorty";
  const char *long_string = "This string is longer than 8 characters";
  const std::string s2( shrt_string );
  const std::string s3( long_string );
  std::string s4( 5, 'x' );
  std::string s5( 5, 'x' );
  std::string s6( 5, 'x' );
  std::string s7( 5, 'x' );
  std::string s8( 5, 'x' );
  std::string s9( 5, 'x' );
  const char *raw = "This is a very, very long string";

  s1 = s2;
  if( s1.size( ) !=  6 || s1 != shrt_string || INSANE( s1 ) ) FAIL

  s1 = s3;
  if( s1.size( ) != 39 || s1 != long_string || INSANE( s1 ) ) FAIL

  s4 = shrt_string;
  if( s4.size( ) !=  6 || s4 != shrt_string || INSANE( s4 ) ) FAIL

  s4 = long_string;
  if( s4.size( ) != 39 || s4 != long_string || INSANE( s4 ) ) FAIL

  s5 = 'y';
  if( s5.size( ) !=  1 || s5 != "y" || INSANE( s5 ) ) FAIL

  s6.assign( s3, 35, 5 );
  if( s6.size( ) !=  4 || s6 != "ters" || INSANE( s6 ) ) FAIL

  s6.assign( s3, 5, 16 );
  if( s6.size( ) != 16 || s6 != "string is longer" || INSANE( s6 ) ) FAIL

  s7.assign( raw, 4 );
  if( s7.size( ) !=  4 || s7 != "This" || INSANE( s7 ) ) FAIL

  s7.assign( raw, 10 );
  if( s7.size( ) != 10 || s7 != "This is a " || INSANE( s7 ) ) FAIL

  s8.assign( "Shorty" );
  if( s8.size( ) !=  6 || s8 != "Shorty" || INSANE( s8 ) ) FAIL

  s8.assign( long_string );
  if( s8.size( ) != 39 || s8 != long_string || INSANE( s8 ) ) FAIL

  s9.assign( 7, 'x' );
  if( s9.size( ) !=  7 || s9 != "xxxxxxx" || INSANE( s9 ) ) FAIL

  s9.assign( 8, 'y' );
  if( s9.size( ) !=  8 || s9 != "yyyyyyyy" || INSANE( s9 ) ) FAIL

  return( rc );
}


bool access_test( )
{
  bool rc = true;
  std::string s1( "Hello, World" );
  std::string s2( "Hello, World" );

  if( s1[ 0] != 'H' ) FAIL
  if( s1[11] != 'd' ) FAIL

  s1[ 0] = 'x';
  s1[11] = 'y';
  if( s1 != "xello, Worly" ) FAIL

  if( s2.at(  0 ) != 'H' ) FAIL
  if( s2.at( 11 ) != 'd' ) FAIL

  s2.at(  0 ) = '-';
  s2.at( 11 ) = '+';
  if( s2 != "-ello, Worl+" ) FAIL

  try {
    char ch = s2.at( 12 );
    FAIL
  }
  catch( std::out_of_range ) {
    // Okay
  }

  return( rc );
}


bool relational_test( )
{
  bool rc = true;
  std::string s1( "abcd"  );
  std::string s2( "abcd"  );
  std::string s3( "abcc"  );
  std::string s4( "abce"  );
  std::string s5( "abc"   );
  std::string s6( "abcde" );

  // Operator==
  if( !( s1 == s2 ) ) FAIL
  if(  ( s1 == s5 ) ) FAIL

  // Operator !=
  if(  ( s1 != s2 ) ) FAIL
  if( !( s5 != s1 ) ) FAIL

  // Operator<
  if(  ( s1 < s2 ) ) FAIL
  if( !( s3 < s1 ) ) FAIL
  if(  ( s4 < s1 ) ) FAIL
  if( !( s5 < s1 ) ) FAIL
  if(  ( s4 < s6 ) ) FAIL

  // Operator>
  if( !( s4 > s3 ) ) FAIL
  if(  ( s1 > s6 ) ) FAIL
  
  // Operator <=
  if( !( s1 <= s2 && s3 <= s2 ) ) FAIL
  if(  ( s2 <= s3 || s6 <= s5 ) ) FAIL

  // Operator>=
  if( !( s2 >= s1 && s6 >= s5 ) ) FAIL
  if(  ( s3 >= s4 || s5 >= s4 ) ) FAIL

  return( rc );
}


bool capacity_test( )
{
  bool rc = true;

  std::string s1;
  std::string s2("Hello, World!");

  if( s1.capacity( ) == 0) FAIL
  if( s2.size( ) > s2.capacity( ) ) FAIL
  if( s1.empty( )    == false ) FAIL
  if( s2.empty( )    == true  ) FAIL

  s2.clear( );
  if( s2.empty( ) == false ) FAIL

  std::string s3("Hello");
  s3.resize( 2 );
  if( s3.size( ) != 2 || s3 != "He" ) FAIL

  s3.resize( 5, 'x' );
  if( s3.size( ) != 5 || s3 != "Hexxx" ) FAIL

  s3.resize( 40, 'y' );
  if( s3.size( ) != 40 ||
      s3 != "Hexxxyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy" ) FAIL

  std::string s4("Hello");
  std::string::size_type new_s4capacity = ( 7 * s4.capacity( ) ) / 2;
  s4.reserve( new_s4capacity );
  if( s4.capacity( ) < new_s4capacity || s4.size( ) != 5 || s4 != "Hello" ) {
    FAIL
  }
  return( rc );
}


// This function should probably be some kind of template so it can be
// readily used to check iterators from any kind of sequence container.
//
bool iterator_test( )
{
  bool rc = true;
  std::string s1( "Hello, World" );
  std::string::iterator p1;
  std::string::iterator p2;

  p1 = s1.begin( );
  if( *p1 != 'H' ) FAIL

  ++p1;
  if( *p1 != 'e' ) FAIL

  --p1;
  if( *p1 != 'H' ) FAIL

  p1++;
  if( *p1 != 'e' ) FAIL

  p1--;
  if( *p1 != 'H' ) FAIL

  p2 = s1.end( );
  --p2;
  if( *p2 != 'd' ) FAIL

  ++p2;
  if( p2 != s1.end( ) ) FAIL

  if ( p2 < p1 ) FAIL

  const std::string s2( "Hello, World" );
  std::string::const_iterator p3;
  std::string::const_iterator p4;

  p3 = s2.begin( );
  if( *p3 != 'H' ) FAIL

  ++p3;
  if( *p3 != 'e' ) FAIL

  --p3;
  if( *p3 != 'H' ) FAIL

  p3++;
  if( *p3 != 'e' ) FAIL

  p3--;
  if( *p3 != 'H' ) FAIL

  p4 = s2.end( );
  --p4;
  if( *p4 != 'd' ) FAIL

  ++p4;
  if( p4 != s2.end( ) ) FAIL

  if ( p4 < p3 ) FAIL

  return( rc );
}


bool append_test( )
{
  bool rc = true;

  // The sizes of the test strings used here are intended to explore
  // both appending without reallocation and appending with re-
  // allocation. A string can not be reused between tests because
  // an enlarged capacity is never reduced.

  std::string s1( "123456" );
  std::string s2( "x" );
  s1 += s2;
  if( s1 != "123456x"  || s1.size( ) != 7 || INSANE( s1 ) ) FAIL
  s1 += s2;
  if( s1 != "123456xx" || s1.size( ) != 8 || INSANE( s1 ) ) FAIL

  std::string s3( "123456" );
  s3 += "x";
  if( s3 != "123456x"  || s3.size( ) != 7 || INSANE( s3 ) ) FAIL
  s3 += "y";
  if( s3 != "123456xy" || s3.size( ) != 8 || INSANE( s3 ) ) FAIL

  std::string s4( "123456" );
  s4 += 'x';
  if( s4 != "123456x"  || s4.size( ) != 7 || INSANE( s4 ) ) FAIL
  s4 += 'z';
  if( s4 != "123456xz" || s4.size( ) != 8 || INSANE( s4 ) ) FAIL

  std::string s5( "123456" );
  std::string s6( "Hello, World!" );
  s5.append( s6, 12, 1 );
  if( s5 != "123456!"  || s5.size( ) != 7 || INSANE( s5 ) ) FAIL
  s5.append( s6, 0, 3 );
  if( s5 != "123456!Hel" || s5.size( ) != 10 || INSANE( s5 ) ) FAIL

  std::string s7( "123456" );
  s7.append( "fizzle", 1 );
  if( s7 != "123456f"  || s7.size( ) != 7 || INSANE( s7 ) ) FAIL
  s7.append( "fizzle", 3 );
  if( s7 != "123456ffiz" || s7.size( ) != 10 || INSANE( s7 ) ) FAIL

  std::string s8( "123456" );
  s8.append( "x" );
  if( s8 != "123456x"  || s8.size( ) != 7 || INSANE( s8 ) ) FAIL
  s8.append( "abc" );
  if( s8 != "123456xabc" || s8.size( ) != 10 || INSANE( s8 ) ) FAIL

  std::string s9( "123456" );
  s9.append( 1, 'x' );
  if( s9 != "123456x" || s9.size( ) != 7 || INSANE( s9 ) ) FAIL
  s9.append( 3, 'y' );
  if( s9 != "123456xyyy" || s9.size( ) != 10 || INSANE( s9 ) ) FAIL

  std::string s10( "123456" );
  s10.push_back( 'z' );
  if( s10 != "123456z" || s10.size( ) != 7 || INSANE( s10 ) ) FAIL
  s10.push_back( 'a' );
  if( s10 != "123456za" || s10.size( ) != 8 || INSANE( s10 ) ) FAIL

  return( rc );
}


bool insert_test( )
{
  bool rc = true;

  std::string s0( "INSERTED" );
  std::string s1( "Hello, World!" );
  s1.insert( 2, s0 );
  if( s1 != "HeINSERTEDllo, World!" || s1.size( ) != 21 || INSANE( s1 ) ) {
    FAIL
  }

  std::string s2( "Hello, World!" );
  s2.insert( 0, s0 );
  if( s2 != "INSERTEDHello, World!" || s2.size( ) != 21 || INSANE( s2 ) ) {
    FAIL
  }

  std::string s3( "Hello, World!" );
  s3.insert( 13, s0 );
  if( s3 != "Hello, World!INSERTED" || s3.size( ) != 21 || INSANE( s3 ) ) {
    FAIL
  }

  std::string s4( "Hello, World!" );
  s4.insert( 0, s0, 2, 2 );
  if( s4 != "SEHello, World!" || s4.size( ) != 15 || INSANE( s4 ) ) {
    FAIL
  }

  std::string s5( "Hello, World!" );
  s5.insert( 0, s0, 2, 128 );
  if( s5 != "SERTEDHello, World!" || s5.size( ) != 19 || INSANE( s5 ) ) {
    FAIL
  }

  // Do multiple insertions to verify reallocations.
  // This should probably also be done for the other insert methods as well.
  std::string s6( "Hello" );
  char input = 'a';
  for( int i = 0; i < 10; ++i ) {
      std::string::iterator p = s6.insert( s6.begin( ), input );
      if( *p != input ) FAIL;
      if( s6.size( ) != 6 + i ) FAIL;
      if( INSANE( s6 ) ) FAIL;
      ++input;
  }
  if( s6 != "jihgfedcbaHello" ) FAIL;

  std::string s7( "Hello, World!" );
  s7.insert( s7.end( ), 3, 'z' );
  if( s7 != "Hello, World!zzz" || s7.size( ) != 16 || INSANE( s7 ) ) {
    FAIL
  }

  // Need to test other insert methods.

  return( rc );
}


bool erase_test( )
{
  bool rc = true;

  std::string s1( "Hello, World!" );
  s1.erase( );
  if( s1 != "" || s1.size( ) != 0 || INSANE( s1 ) ) FAIL

  std::string s2( "Hello, World!" );
  s2.erase( 2, 3 );
  if( s2 != "He, World!" || s2.size( ) != 10 || INSANE( s2 ) ) FAIL

  std::string s3( "Hello, World!" );
  s3.erase( 7, 6 );
  if( s3 != "Hello, " || s3.size( ) != 7 || INSANE( s3 ) ) FAIL

  std::string s4( "Hello, World!" );
  s4.erase( s4.begin( ) );
  if( s4 != "ello, World!" || s4.size( ) != 12 || INSANE( s4 ) ) FAIL

  std::string s5( "Hello, World!" );
  s5.erase( s5.begin( ) + 2, s5.begin( ) + 5 );
  if( s5 != "He, World!" || s5.size( ) != 10 || INSANE( s5 ) ) FAIL

  return( rc );
}


bool replace_test( )
{
  bool rc = true;
  const std::string s1( "Insert me!" );

  std::string t1( "ABCDEFGHIJKLMNOPQRSTUVWXYZ" );
  t1.replace( 0, std::string::npos, s1, 0, std::string::npos );
  if( t1 != "Insert me!" || INSANE( t1 ) ) FAIL

  std::string t2( "ABCDEFGHIJKLMNOPQRSTUVWXYZ" );
  t2.replace( 0, 1, s1, 0, std::string::npos );
  if( t2 != "Insert me!BCDEFGHIJKLMNOPQRSTUVWXYZ" || INSANE( t2 ) ) FAIL

  std::string t3( "ABCDEFGHIJKLMNOPQRSTUVWXYZ" );
  t3.replace( 1, 0, s1, 0, 1 );
  if( t3 != "AIBCDEFGHIJKLMNOPQRSTUVWXYZ" || INSANE( t3 ) ) FAIL

  std::string t4( "ABCDEFGHIJKLMNOPQRSTUVWXYZ" );
  t4.replace( 26, 0, s1, 0, std::string::npos );
  if( t4 != "ABCDEFGHIJKLMNOPQRSTUVWXYZInsert me!" || INSANE( t4 ) ) FAIL

  std::string t5( "ABCDEFGHIJKLMNOPQRSTUVWXYZ" );
  t5.replace( 4, 3, s1, 2, 5 );
  if( t5 != "ABCDsert HIJKLMNOPQRSTUVWXYZ" || INSANE( t5 ) ) FAIL

  std::string t6( "Shorty" );
  t6.replace( 1, 2, s1, 0, 2 );
  if( t6 != "SInrty" || INSANE( t6 ) ) FAIL

  // Need to test other replace methods.

  return( rc );
}


bool iterator_replace_test( )
{
  bool rc = true;
  const std::string s1( "Insert me!" );

  std::string t1( "ABCDEFGHIJKLMNOPQRSTUVWXYZ" );
  t1.replace( t1.begin( ), t1.end( ), s1 );
  if( t1 != "Insert me!" || INSANE( t1 ) ) FAIL

  std::string t2( "ABCDEFGHIJKLMNOPQRSTUVWXYZ" );
  t2.replace( t2.begin( ), t2.begin( ) + 1, s1 );
  if( t2 != "Insert me!BCDEFGHIJKLMNOPQRSTUVWXYZ" || INSANE( t2 ) ) FAIL

  std::string t3( "ABCDEFGHIJKLMNOPQRSTUVWXYZ" );
  t3.replace( t3.end( ), t3.end( ), s1 );
  if( t3 != "ABCDEFGHIJKLMNOPQRSTUVWXYZInsert me!" || INSANE( t3 ) ) FAIL

  std::string t4( "ABCDEFGHIJKLMNOPQRSTUVWXYZ" );
  t4.replace( t4.begin( ) + 4, t4.begin( ) + 7, s1 );
  if( t4 != "ABCDInsert me!HIJKLMNOPQRSTUVWXYZ" || INSANE( t4 ) ) FAIL

  std::string t5( "Shorty" );
  t5.replace( t5.begin( ), t5.begin( ) + 2, 2, 'x' );
  if( t5 != "xxorty" || INSANE( t5 ) ) FAIL

  // Need to test other iterator replace methods.

  return( rc );
}


bool copy_test( )
{
  bool rc = true;

  char buffer[128];
  std::string s1( "Hello, World!" );

  buffer[ s1.copy( buffer, 2, 0 ) ] = '\0';
  if( std::strcmp( buffer, "He" ) != 0 ) FAIL

  buffer[ s1.copy( buffer, s1.size( ), 0 ) ] = '\0';
  if( std::strcmp( buffer, "Hello, World!" ) != 0 ) FAIL

  buffer[ s1.copy( buffer, 3, 2 ) ] = '\0';
  if( std::strcmp( buffer, "llo" ) != 0 ) FAIL

  buffer[ s1.copy( buffer, 10, 7 ) ] = '\0';
  if( std::strcmp( buffer, "World!" ) != 0 ) FAIL

  buffer[ s1.copy( buffer, 0, 13 ) ] = '\0';
  if( std::strcmp( buffer, "" ) != 0 ) FAIL

  try {
    buffer[ s1.copy( buffer, 1, 14 ) ] = '\0';
    FAIL
  }
  catch( std::out_of_range ) {
    // Okay
  }
  return( rc );
}


bool swap_test( )
{
  bool rc = true;

  std::string s1("ABC");
  std::string s2("XYZ");

  s1.swap( s2 );
  if( s1 != "XYZ" || s2 != "ABC" ) FAIL

  #ifdef __NEVER
  std::swap( s1, s2 );
  if( s1 != "ABC" || s2 != "XYZ" ) FAIL
  #endif

  return( rc );
}


bool cstr_test( )
{
        bool  rc = true;
  const char *p;

  std::string s1( "Hello, World!" );
  p = s1.c_str( );
  if( std::strcmp( p, "Hello, World!" ) != 0 ) FAIL

  std::string s2;
  p = s2.c_str( );
  if ( std::strcmp( p, "" ) != 0 ) FAIL

  return( rc );
}


bool find_test( )
{
  bool rc = true;

  const std::string s1( "Hello, World" );
  if( s1.find( "Hello",  0 ) != 0                 ) FAIL
  if( s1.find( "Hello",  1 ) != std::string::npos ) FAIL
  if( s1.find( "World",  0 ) != 7                 ) FAIL
  if( s1.find( "Hello", 11 ) != std::string::npos ) FAIL
  if( s1.find( "Hello", 12 ) != std::string::npos ) FAIL
  if( s1.find( "Hello", 13 ) != std::string::npos ) FAIL
  if( s1.find( "",      12 ) != 12                ) FAIL

  return( rc );
}


bool rfind_test( )
{
  bool rc = true;

  const std::string s1( "Hello, World" );
  if( s1.rfind( "World" ) != 7 ) FAIL
  if( s1.rfind( "Hello" ) != 0 ) FAIL
  if( s1.rfind( "ell", 7 ) != 1 ) FAIL
  if( s1.rfind( "Fizzle" ) != std::string::npos ) FAIL
  if( s1.rfind( "Hello, World..." ) != std::string::npos ) FAIL

  return( rc );
}


bool find_first_of_test()
{
  bool rc = true;

  const std::string s1( "Hello, World!" );
  if( s1.find_first_of( "eoW" )    !=  1 ) FAIL
  if( s1.find_first_of( "eoW", 1 ) !=  1 ) FAIL
  if( s1.find_first_of( "eoW", 2 ) !=  4 ) FAIL
  if( s1.find_first_of( "!" )      != 12 ) FAIL
  if( s1.find_first_of( "!", 13 )  != std::string::npos ) FAIL
  if( s1.find_first_of( "z#+" )    != std::string::npos ) FAIL

  return( rc );
}


bool find_last_of_test()
{
  bool rc = true;

  const std::string s1( "Hello, World!" );
  if( s1.find_last_of( "eoW" )    !=  8 ) FAIL
  if( s1.find_last_of( "eoW", 1 ) !=  1 ) FAIL
  if( s1.find_last_of( "oWe", 2 ) !=  1 ) FAIL
  if( s1.find_last_of( "!" )      != 12 ) FAIL
  if( s1.find_last_of( "!", 13 )  != 12 ) FAIL
  if( s1.find_last_of( "z#+" )    != std::string::npos ) FAIL

  return( rc );
}


bool find_first_not_of_test()
{
  bool rc = true;

  const std::string good_chars("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
  const std::string s1("xAWQKSMQIFJJWXGV"); // Be sure both 'A' and 'Z'.
  const std::string s2("KFNNAWEZxZNEKGDW");
  const std::string s3("QMVVXZKRIGJJWTAx");
  const std::string s4("UUDHHAKVVLZVFLQP");
  const std::string s5("");

  if( s1.find_first_not_of( good_chars )     !=  0 ) FAIL
  if( s2.find_first_not_of( good_chars )     !=  8 ) FAIL
  if( s3.find_first_not_of( good_chars )     != 15 ) FAIL
  if( s4.find_first_not_of( good_chars )     != std::string::npos ) FAIL
  if( s2.find_first_not_of( good_chars,  8 ) !=  8 ) FAIL
  if( s3.find_first_not_of( good_chars, 15 ) != 15 ) FAIL
  if( s3.find_first_not_of( good_chars, 16 ) != std::string::npos ) FAIL
  if( s3.find_first_not_of( good_chars, 17 ) != std::string::npos ) FAIL
  if( s5.find_first_not_of( good_chars )     != std::string::npos ) FAIL
  if( s4.find_first_not_of( s5 )             != 0 ) FAIL

  return( rc );
}


bool find_last_not_of_test()
{
  bool rc = true;

  const std::string good_chars("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
  const std::string s1("xAWQKSMQIFJJWXGV");  // Be sure both 'A' and 'Z'.
  const std::string s2("KFNNAWEZxZNEKGDW");
  const std::string s3("QMVVXZKRIGJJWTAx");
  const std::string s4("UUDHHAKVVLZVFLQP");
  const std::string s5("");

  if( s1.find_last_not_of( good_chars )     !=  0 ) FAIL
  if( s2.find_last_not_of( good_chars )     !=  8 ) FAIL
  if( s3.find_last_not_of( good_chars )     != 15 ) FAIL
  if( s4.find_last_not_of( good_chars )     != std::string::npos ) FAIL
  if( s2.find_last_not_of( good_chars,  8 ) !=  8 ) FAIL
  if( s3.find_last_not_of( good_chars, 15 ) != 15 ) FAIL
  if( s3.find_last_not_of( good_chars, 16 ) != 15 ) FAIL
  if( s3.find_last_not_of( good_chars, 17 ) != 15 ) FAIL
  if( s5.find_last_not_of( good_chars )     != std::string::npos ) FAIL
  if( s4.find_last_not_of( s5 )             != 15 ) FAIL

  return( rc );
}


bool substr_test()
{
  bool rc = true;

  const std::string s1("Hello, World!");

  if( s1.substr( )      != "Hello, World!" ) FAIL
  if( s1.substr( 1 )    != "ello, World!"  ) FAIL
  if( s1.substr( 0, 1 ) != "H"             ) FAIL
  if( s1.substr( 0, 0 ) != ""              ) FAIL
  if( s1.substr( 12 )   != "!"             ) FAIL
  if( s1.substr( 3, 4 ) != "lo, "          ) FAIL
  if( s1.substr( 13 )   != ""              ) FAIL
  try {
    if( s1.substr( 14 ) != "" ) FAIL
  }
  catch( std::out_of_range ) {
    // Okay.
  }
  return( rc );
}


// Main Program
// ============

int main( )
{
  int rc = 0;
  int original_count = heap_count( );

  try {
    if( !construct_test( )         || !heap_ok( "t01" ) ) rc = 1;
    if( !assign_test( )            || !heap_ok( "t02" ) ) rc = 1;
    if( !access_test( )            || !heap_ok( "t03" ) ) rc = 1;
    if( !relational_test( )        || !heap_ok( "t04" ) ) rc = 1;
    if( !capacity_test( )          || !heap_ok( "t05" ) ) rc = 1;
    if( !iterator_test( )          || !heap_ok( "t06" ) ) rc = 1;
    if( !append_test( )            || !heap_ok( "t07" ) ) rc = 1;
    if( !insert_test( )            || !heap_ok( "t08" ) ) rc = 1;
    if( !erase_test( )             || !heap_ok( "t09" ) ) rc = 1;
    if( !replace_test( )           || !heap_ok( "t10" ) ) rc = 1;
    if( !iterator_replace_test( )  || !heap_ok( "t11" ) ) rc = 1;
    if( !copy_test( )              || !heap_ok( "t12" ) ) rc = 1;
    if( !swap_test( )              || !heap_ok( "t13" ) ) rc = 1;
    if( !cstr_test( )              || !heap_ok( "t14" ) ) rc = 1;
    if( !find_test( )              || !heap_ok( "t15" ) ) rc = 1;
    if( !rfind_test( )             || !heap_ok( "t16" ) ) rc = 1;
    if( !find_first_of_test( )     || !heap_ok( "t17" ) ) rc = 1;
    if( !find_last_of_test( )      || !heap_ok( "t18" ) ) rc = 1;
    if( !find_first_not_of_test( ) || !heap_ok( "t19" ) ) rc = 1;
    if( !find_last_not_of_test( )  || !heap_ok( "t20" ) ) rc = 1;
    if( !substr_test( )            || !heap_ok( "t21" ) ) rc = 1;
  }
  catch( std::out_of_range e ) {
    std::cout << "Unexpected out_of_range exception: " << e.what( ) << "\n";
    rc = 1;
  }
  catch( std::length_error e ) {
    std::cout << "Unexpected length_error exception: " << e.what( ) << "\n";
    rc = 1;
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
