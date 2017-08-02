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
* Description: This file contains wrapper classes that limit the powers of
*              pointers to each of the five iterator catagories defined in
*              the standard. Using these classes in the OWSTL tests helps
*              check that no inappropriate assumptions about iterator
*              operations are being made.
*
****************************************************************************/

#ifndef ITCAT_H
#define ITCAT_H

#include <iterator>

// Input iterator wrapper.
// 
// Input iterators are such that when one is copied, both copies continue
// to interact. In particular, advancing one input iterator advances the
// copy as well. The class below attempts to simulate this behavior by
// storing a pointer in space that is held in common with all iterators
// copied from a given iterator. This class uses standard reference
// counting techniques to manage the common storage.
//
template< class T >
class InpIt : public std::iterator< std::input_iterator_tag, T > {
private:
  struct common {
    T  *ptr;
    int count;

    common( T *p, int c ) : ptr(p), count(c) { }
  };

  // Used to deal with expressions like *it++.
  struct post_proxy {
    T  *ptr;

    post_proxy( T *p ) : ptr(p) { }
    const T &operator*( ) { return( *ptr ); }
  };

public:
  InpIt( ) : c( new common( 0, 1 ) ) { }
  // Input iterators don't really have default constructors. However,
  // removing the default constructor here causes problems in some of the
  // algorithm tests. In particular when pair< InpIt<int>, InpIt<int> > is
  // defined, the compiler attempts to instantiate pair's default construc-
  // tor even if it is never used. Once this is fixed in the compiler, the
  // default constructor here can be removed and the strength of the tests
  // using InpIt will be increased.

  explicit InpIt( T *p ) : c( new common( p, 1 ) ) { }
  InpIt( const InpIt &other ) : c( other.c ) { ++(c->count); }
  InpIt &operator=( const InpIt &other )
   { if(--(c->count) == 0) delete c; c = other.c; ++(c->count); return *this; }
 ~InpIt( ) { if( --(c->count) == 0 ) delete c; }

  bool operator==( const InpIt &right ) { return( c->ptr == right.c->ptr ); }
  bool operator!=( const InpIt &right ) { return( c->ptr != right.c->ptr ); }

  // Return const so that writes through the iterator will fail to compile.
  const T &operator*( ) { return( *c->ptr ); }
  const T *operator->( ) { return( c->ptr ); }

  InpIt &operator++( ) { ++(c->ptr); return( *this ); }
  post_proxy operator++( int )
    { post_proxy tmp(c->ptr); ++(c->ptr); return( tmp ); }

  T *get( ) { return( c->ptr ); }

private:
  common *c;
};


// Output iterator wrapper.
//
// Output iterators are similar to input iterators in that advancing an
// iterator will advance all copies of that iterator. They also provide
// write-only access. To implement this, operator* returns a proxy that
// supports a suitable assignment operation. Attempts to read from *it,
// however, will fail with a type mismatch because there is no conversion
// from OutIt<T>::proxy to T.
//
template< class T >
class OutIt : public std::iterator< std::output_iterator_tag, T > {
private:
  struct common {
    T  *ptr;
    int count;

    common( T *p, int c ) : ptr(p), count(c) { }
  };

public:
  explicit OutIt( T *p ) : c( new common( p, 1 ) ) { }
  OutIt( const OutIt &other ) : c( other.c ) { ++(c->count); }
  OutIt &operator=( const OutIt &other )
   { if(--(c->count) == 0) delete c; c = other.c; ++(c->count); return *this; }
 ~OutIt( ) { if( --(c->count) == 0 ) delete c; }
  OutIt &operator=( const T &value )
    { *c->ptr = value; ++(c->ptr); return *this; }

  // Output iterators have no equality comparisons.

  OutIt &operator*( ) { return( *this ); }
  // Output iterators have no operator-> (which makes sense).

  OutIt &operator++( ) { return( *this ); }
  OutIt  operator++( int ) { return( *this ); }

  T *get( ) { return( c->ptr ); }

private:
  common *c;
};


// Forward iterator wrapper.
//
template< class T >
class FwdIt : public std::iterator< std::forward_iterator_tag, T > {
public:
  FwdIt( ) : ptr( 0 ) { }  // Set to null so accidental use will crash.
  explicit FwdIt( T *p ) : ptr( p ) { }
  FwdIt( const FwdIt &other ) : ptr( other.ptr ) { }
  // Automatically generated assignment operator is acceptable.

  bool operator==( const FwdIt &right ) { return( ptr == right.ptr ); }
  bool operator!=( const FwdIt &right ) { return( ptr != right.ptr ); }

  T &operator*( ) { return( *ptr ); }
  T *operator->( ) { return( ptr ); }

  FwdIt &operator++( ) { ++ptr; return( *this ); }
  FwdIt  operator++( int ) { FwdIt tmp(*this); ++ptr; return( tmp ); }

  T *get( ) { return( ptr ); }

private:
  T *ptr;
};

// Bidirectional iterator wrapper.
//
template< class T >
class BidIt : public std::iterator< std::bidirectional_iterator_tag, T > {
public:
  BidIt( ) : ptr( 0 ) { }  // Set to null so accidental use will crash.
  explicit BidIt( T *p ) : ptr( p ) { }
  BidIt( const BidIt &other ) : ptr( other.ptr ) { }
  // Automatically generated assignment operator is acceptable.

  bool operator==( const BidIt &right ) { return( ptr == right.ptr ); }
  bool operator!=( const BidIt &right ) { return( ptr != right.ptr ); }

  T &operator*( ) { return( *ptr ); }
  T *operator->( ) { return( ptr ); }

  BidIt &operator++( ) { ++ptr; return( *this ); }
  BidIt  operator++( int ) { BidIt tmp(*this); ++ptr; return( tmp ); }

  BidIt &operator--( ) { --ptr; return( *this ); }
  BidIt  operator--( int ) { BidIt tmp(*this); --ptr; return( tmp ); }

  T *get( ) { return( ptr ); }

private:
  T *ptr;
};

#endif
