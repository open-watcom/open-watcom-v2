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


//
//  wcexcept.h    Definitions for exception base classes.  These classes are
//                Inherited by each of the WATCOM Container Classes
//
//  Copyright by WATCOM International Corp. 1988-1993.  All rights reserved.
//

#ifndef _WCEXCEPT_H_INCLUDED

#ifndef __cplusplus
#error wcexcept.h is for use with C++
#endif



//
//  The WCExcept class defines the exception handling for the WATCOM
//  Container non-iterative classes.  The current errors supported are:
//
//    empty_container - if an invalid operation on an empty container object
//                      is attempted, this error can be thrown
//    index_range    -- if an index value is not valid, this error can
//                      be thrown.
//    not_empty      -- if a container object which should be empty is being
//                      destroyed by the class destructor, this error can be
//                      thrown.
//    not_unique     -- if an attempt to insert a value into a set which
//                      previously contained an equivalent value is made, this
//                      error can be thrown.
//    out_of_memory  -- if an attempt to allocate memory fails, this error
//                      can be thrown.
//    resize_required - if an attempt was made to insert into a full vector,
//                      or index a position with index length or greater in
//                      WCValVector or WCPtrVector, this exception can be
//                      thrown. If this exception is disabled, the necessary
//                      resize will be performed.
//    zero_buckets   -- if an attempt to resize a hash table, hash set, or
//                      hash dictionary to zero buckets, this error can be
//                      thrown
//

class WCExcept {
private:
    int             exception_flags;

protected:
    inline void base_construct( const WCExcept * orig ) {
        exception_flags = orig->exception_flags;
    }
    void base_throw_empty_container() const;
    void base_throw_index_range() const;
    //
    // non-const since it disables the object's exceptions to try and prevent
    // more than one exception being thrown
    //
    void base_throw_not_empty();
    void base_throw_not_unique() const;
    void base_throw_out_of_memory() const;
    void base_throw_resize_required() const;
    void base_throw_zero_buckets() const;

public:
    inline WCExcept() : exception_flags(0) {};
    inline virtual ~WCExcept() {};

    enum wcstate {                      // Error state
        all_fine        = 0x0000,       // - no errors
        check_none      = all_fine,     // - don't throw any exceptions
        not_empty       = 0x0001,       // - container not empty on destruction
        index_range     = 0x0002,       // - index is out of range
        empty_container = 0x0004,       // - invalid request on empty container
        out_of_memory   = 0x0008,       // - allocation failed
        resize_required = 0x0010,       // - resize required for request
        not_unique      = 0x0020,       // - adding duplicate value to set
        zero_buckets    = 0x0040,       // - resizing hash to zero buckets

        // value to use to check for all errors (all exceptions enabled)
        check_all   = (not_empty|index_range|empty_container|out_of_memory
                      |resize_required|not_unique|zero_buckets)
    };

    typedef int wc_state;

    // For back compatiblity
    typedef wc_state wclist_state;

    // this object is thrown on an exception
    class failure : public __WATCOM_exception {
    private:
        wc_state    _cause;
    public:
        failure( wc_state state ) : _cause( state ) {};
        wc_state cause() const {
            return( _cause );
        };
    };

    inline wc_state exceptions() const {
        return( exception_flags );
    }

    inline wc_state exceptions( wc_state const flag_set ) {
        wc_state old_flags = exception_flags;
        exception_flags = flag_set;
        return( old_flags );
    }
};


// For back compatiblity
typedef WCExcept WCListExcept;




//
//  The WCIterExcept class defines the exception handling for WATCOM Container
//  iterator classes.  The current errors supported are:
//
//      iter_range  -- the iterator advance value for the += or -= operators
//                     cannot be non-positive
//      undef_item  -- if an iterator is outside the bounds of the object,
//                     or the iterator was never initialized with an object,
//                     this error can be thrown on call to the current member
//                     function (or the key and value member functions on
//                     an iterator for a dictionary).
//      undef_iter  -- if an iterator is outside the bounds of the object, or
//                     if the iterator was never initialized with a object,
//                     this error can be thrown on an invalid operation
//                     other than a call to the member function current (or
//                     the key and value member functions on an iterator for
//                     a dictionary).
//

class WCIterExcept {
private:
    int exception_flags;

protected:
    void base_throw_iter_range() const;
    void base_throw_undef_item() const;
    void base_throw_undef_iter() const;

public:
    inline WCIterExcept() : exception_flags(0) {};
    inline virtual ~WCIterExcept() {};

    enum wciterstate {                  // Error state
        all_fine    = 0x0000,           // - no errors
        check_none  = all_fine,         // - don't throw any exceptions
        undef_iter  = 0x0001,           // - iterator position is undefined
        undef_item  = 0x0002,           // - current iterator item is undefined
        iter_range  = 0x0004,           // - iterator advance value is bad

        // value to use to check for all errors
        check_all   = (undef_iter|undef_item|iter_range)
    };
    typedef int wciter_state;

    // this object is thrown on an exception
    class failure : public __WATCOM_exception {
    private:
        wciter_state    _cause;
    public:
        failure( wciter_state state ) : _cause( state ) {};
        wciter_state cause() const {
            return( _cause );
        };
    };

    inline wciter_state exceptions() const {
        return( exception_flags );
    }

    inline wciter_state exceptions( wciter_state flag_set ) {
        wciter_state old_flags = exception_flags;
        exception_flags = flag_set;
        return( old_flags );
    }
};


#define _WCEXCEPT_H_INCLUDED
#endif
