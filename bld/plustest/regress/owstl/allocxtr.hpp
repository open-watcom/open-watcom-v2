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
* Description:  This file additional/specialised allocators to be used for 
*               testing the Open Watcom conatainer templates
****************************************************************************/
#include<memory>
#include<new>

/* ==================================================================
 * LowMemAllocator class
 * Simulates low memory by throwing an exception after a set
 * number of allocations.  Can be used to check number of 
 * allocs/constructs are equal to number of deallocs/destructs.
 * Its state is common between all copies. (which allows it to be
 * accessed when inside a container, and means containers that hold
 * more than one copy (eg multiset) work as expected)
 */
struct LMAState{
    int     mNumAlloc;
    int     mNumConstruct;
    int     mNumDealloc;
    int     mNumDestroy;
    size_t  mTripLevel;
    bool    mTripOnAlloc;
    size_t  mRefCount;
};
template< class Type >
class LowMemAllocator : public std::allocator< Type >{
public:
    LowMemAllocator(size_t tl = 0) : mState(new State())
        { mState->mRefCount = 1;
          mState->mNumAlloc = 0; mState->mNumConstruct = 0;
          mState->mNumDealloc = 0; mState->mNumDestroy = 0;
          mState->mTripOnAlloc = true; mState->mTripLevel = tl;
        }

    template< class Type2 >
    LowMemAllocator( LowMemAllocator<Type2> const & x ) : mState(x.mState)
        { ++mState->mRefCount; }

    //to do: check standard... do we really need to redefine this if 
    //template version is there? without this it calls the implicit/compiler defined copy constructor
    LowMemAllocator( LowMemAllocator const & x ) : mState(x.mState) 
        { ++mState->mRefCount; }
    
    template< class Type2 >
    struct rebind{ typedef LowMemAllocator<Type2> other; };

    ~LowMemAllocator() { if( --mState->mRefCount == 0 ) delete mState; }

    LowMemAllocator& operator=( LowMemAllocator const & x ) {
        if( --mState->mRefCount == 0 ) delete mState;
        mState = x.mState;
        ++mState->mRefCount;
        return( *this );
    }

    pointer allocate( size_type n, std::allocator< void >::const_pointer = 0 )
    {
        ++mState->mNumAlloc;
        if( mState->mTripOnAlloc && mState->mNumAlloc > mState->mTripLevel ){
            throw std::bad_alloc();
        }
        return( reinterpret_cast< Type * >( operator new( n * sizeof( Type ) ) ) ); 
    }

    void deallocate( pointer p, size_type )
    {
        ++mState->mNumDealloc;
        operator delete( reinterpret_cast< unsigned char * >( p ) ); 
    }

    void construct( pointer p, const Type &value )
    {
        ++mState->mNumConstruct;
        if( !mState->mTripOnAlloc && mState->mNumConstruct > mState->mTripLevel ){
            throw std::bad_alloc();
        }
        new ( ( void * )p ) Type( value ); 
    }

    void destroy( pointer p )
    {
        ++mState->mNumDestroy;
        ( ( Type * )p )->~Type( ); 
    }
    
    void SetTripOnConstruct() { mState->mTripOnAlloc = false; }
    void SetTripOnAlloc()     { mState->mTripOnAlloc = true; }
    void Reset( size_t tl ) { 
        mState->mNumAlloc = 0;       mState->mNumConstruct = 0;
        mState->mNumDealloc = 0;     mState->mNumDestroy = 0;
        mState->mTripOnAlloc = true; mState->mTripLevel = tl;
    }
    int GetNumAllocs()      { return( mState->mNumAlloc ); }
    int GetNumConstructs()  { return( mState->mNumConstruct ); }
    int GetNumDeallocs()    { return( mState->mNumDealloc ); }
    int GetNumDestroys()    { return( mState->mNumDestroy ); }
    typedef LMAState State;
    State* mState;
private:
    //State* mState;
};



