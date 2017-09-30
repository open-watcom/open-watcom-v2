

#ifndef _sequence_h_ /* Tue Dec 22 11:41:35 1992 */
#define _sequence_h_





/*
 *
 *          Copyright (C) 1994, M. A. Sridhar
 *  
 *
 *     This software is Copyright M. A. Sridhar, 1994. You are free
 *     to copy, modify or distribute this software  as you see fit,
 *     and to use  it  for  any  purpose, provided   this copyright
 *     notice and the following   disclaimer are included  with all
 *     copies.
 *
 *                        DISCLAIMER
 *
 *     The author makes no warranties, either expressed or implied,
 *     with respect  to  this  software, its  quality, performance,
 *     merchantability, or fitness for any particular purpose. This
 *     software is distributed  AS IS.  The  user of this  software
 *     assumes all risks  as to its quality  and performance. In no
 *     event shall the author be liable for any direct, indirect or
 *     consequential damages, even if the  author has been  advised
 *     as to the possibility of such damages.
 *
 */




// A Sequence is a container object, representing a sequence whose elements
// can be  accessed and modified  via the subscript  operator. In addition,
// the Sequence container supports the  Add, Insert and Remove operations
// that provide for  automatic growth  and shrinkage   of the  sequence  as
// necessary, and the  InsertByRank method  for  insertion in sorted  order
// assuming that the sequence is sorted.
// 
// The Sequence  object also provides Sort() as  well as LinearSearch() and
// BinarySearch() methods for sorting and searching for elements.
// 
// The Sequence  is  a template  class,  and its base  type  is required to
// support  the Compare method  as  well as a   copy constructor, a default
// constructor (that takes no parameters) and an assignment operator.
// 
// Methods that modify the   sequence (i.e., Insert,  InsertByRank, Remove,
// Add, Sort) will return  without modifying the sequence  if any of  the
// pre-change dependents refuses permission. However, the sequence does not
// check for modification of the    contained objects (those returned   via
// {\tt operator[]}).
//
// When the Sequence is instantiated  as a container for pointers (as  are
// {\small\tt CL_Sequence <CL_ObjectPtr>} or CL_ObjectSequence), the
// sequence does {\it not\/} own the objects  that the pointers  point to,
// and  therefore 
// does not  delete them when  it is  destroyed.  The {\tt DestroyContents}
// method  is provided  on  {\tt  CL_ObjectSequence}  to  provide  explicit
// control over destruction of contents.
//
// The implementation uses a "segmented array" technique that allows
// creating sequences with size up to $2^{26}$, or approximately 64 million,
// even under MS-Windows, thus alleviating the 64K  limitation under
// MS-Windows (provided, of course, there is enough main memory available).

#include "base/iofilter.h"


#ifdef __GNUC__
#pragma interface
#endif



template <class T> class CL_Set;
class CL_IntegerSet;



template <class BaseType>
class __CLASSTYPE CL_Sequence: public CL_Object {

public:
    //
    // ------------------------ Creation and destruction --------------
    //
    CL_Sequence (long initial_size = 0, CL_ObjectIOFilter* builder = 0);
    // Create a sequence with given size. The second parameter is used
    // only if this is a sequence of pointers, and then only for restoring
    // this sequence from a stream. If specified, the iofilter object must
    // exist whenever the sequence needs to save or restore itself from a
    // stream; the sequence does not take responsibility for the (memory
    // used by the) iofilter object.

    CL_Sequence (const BaseType data[], long count,
                 CL_ObjectIOFilter* builder = 0);
    // Convenience constructor: create this sequence from a C-style array.
    
    CL_Sequence (const CL_Sequence<BaseType>& seq);
    // Copy constructor. If the template parameter {\small\tt BaseType>}
    // is a first-class object, the copy constructor of the {\small\tt
    // BaseType} is used to copy each entry in the sequence; if it's a
    // pointer, just the pointer is copied. Also, the iofilter pointer of
    // the  parameter is copied into this object.

    ~CL_Sequence();
    // Destructor.


    //
    // ------------------------ Element access ---------------------------

    long Size() const;
    // Return the number of elements in the sequence. This is an inline
    // method that takes small constant time.

    virtual BaseType& operator[] (long i) const;
    // Return the i-th element. The index i must be in the range 0
    // to Size()-1; this is checked by the method, and a fatal error is
    // caused if the index is out of range. The return value is
    // a reference that may be modified by the caller.
    //
    // This method is implemented with merely two shift operations and two
    // indexed accesses on the segmented sequence, so it is quite efficient.

    virtual bool Insert (const BaseType& o, long index = -1);
    // Insert the given element into the sequence, immediately to the right of
    // the given index, and expand the sequence size by 1. Return TRUE if
    // successful, FALSE if failed for some reason (e.g. the index was
    // less than -1 or greater than Size()-1). Specifying an index of
    // -1 inserts the element at the left end of the sequence.
    //
    // The implementation performs {\small\tt Size() - index} pointer
    // movements. These are pointer movements even when the {\small\tt
    // BaseType} is a first-class object.

    virtual long InsertByRank (const BaseType& o);
    // Insert the given object o into the sequence at the smallest position
    // i such that all elements in the sequence between indices 0 and i-1
    // are less than o, according to the Compare method on o. (Note that
    // this formulation does not assume that the sequence is sorted,
    // although InsertByRank maintains sorted order if it were.) Return the
    // index i at which it was inserted, after increasing the size of the
    // sequence by 1.
    //    This method returns -1 if memory allocation failed or a
    // pre-change dependent refused permission to change.
    //
    // The implementation performs {\small\tt Size() - index} pointer
    // movements, where {\small\tt index} is the position at which the
    // insertion occurs. These are pointer movements even when the {\small\tt
    // BaseType} is a first-class object.
    
    virtual long Add (const BaseType& o);
    // Append an element to the end of the sequence, expanding the sequence
    // automatically if needed. Return the index at which the element
    // was added.
    //
    // This method is very efficient (constant-time) in most cases; it
    // needs extra time only when the size has to increase, and that
    // happens very infrequently.

    virtual BaseType Remove (long i);
    // Remove the i-th element of the sequence, and close the hole, so
    // that all elements formerly at indices higher than i will now
    // have their indices decremented by 1. Return the removed element,
    // and return the null value of the BaseType if removal failed (e.g.,
    // for an invalid index i).
    //
    // The implementation performs {\small\tt Size() - index} pointer
    // movements. These are pointer movements even when the {\small\tt
    // BaseType} is a first-class object.

    virtual BaseType ExtractLeftmost ();
    // Remove and return the leftmost element of the sequence. Return the
    // null value of the base type if failed (e.g. because the sequence is
    // empty).
    //
    // The implementation performs {\small\tt Size()} pointer
    // movements. These are pointer movements even when the {\small\tt
    // BaseType} is a first-class object.

    virtual BaseType ExtractRightmost ();
    // Remove and return the rightmost element of the sequence. Return the
    // null value of the base type if failed.
    //
    // This method is very efficient (constant-time) in most cases; it
    // needs extra time only when the size has to decrease, and that
    // happens very infrequently.


    virtual long LinearSearch (const BaseType& o) const;
    // Do a linear search for the given object in the sequence. Return
    // the index at which it was found, or -1 if not found.

    virtual bool BinarySearch (const BaseType& o, long& index) const;
    // Assuming that the sequence is sorted, search for a given element,
    // and return a boolean whether it was found. Return the index of
    // the greatest element not exceeding the given element in the
    // second parameter. This method performs a binary search for
    // sequences larger than 7 elements, but does not check whether the
    // sequence is sorted; so it must only be used on sequences that are
    // known to be sorted. (See the {\small\tt Sort} method.)

    
    
    // ------------------- Sequence methods ------------------------

    virtual void operator= (const CL_Sequence<BaseType>&);
    // Assign to this sequence from the given sequence. If the template
    // parameter {\small\tt BaseType>} 
    // is a first-class object, the copy constructor of the {\small\tt
    // BaseType} is used to copy each entry in the sequence; if it's a
    // pointer, just the pointer is copied.


    void operator= (const CL_Object& o);
    // Check that the given object has the same class id as the sequence,
    // and then perform a sequence assignment after casting down.
    

    virtual CL_Sequence<BaseType>& operator+= (const CL_Sequence<BaseType>& s);
    // Concatenate the given sequence onto the end of this sequence. If
    // the template parameter {\small\tt BaseType>} 
    // is a first-class object, the copy constructor of the {\small\tt
    // BaseType} is used to copy each entry in the sequence {\small\tt s};
    // if it's a pointer, just the pointer is copied.

#ifndef __GNUC__ // Temporarily disable these methods to work around
                 // the GCC bug
    virtual CL_Sequence<BaseType> Subsequence (const CL_IntegerSet& s)
        const;
    // Return the subsequence  of this sequence containing those
    // elements whose positions are in the given set.
        
    virtual CL_Sequence<BaseType> operator- (const CL_IntegerSet& s) const;
    // Return the sequence obtained by removing from this sequence those
    // elements whose indices are in the given set. If the {\small\tt
    // BaseType} of this sequence is a pointer,
    // the pointers are copied into the returned
    // sequence; if the {\small\tt BaseType} is a first-class objects, the copy
    // constructor of the {\small\tt BaseType} is used to copy the objects
    // into the returned sequence.


    virtual void operator-= (const CL_IntegerSet& s);
    // Remove from this sequence those elements whose indices are in the
    // given set. The implementation uses no more than n = Size() pointer
    // movements, regardless of how big s is.

#endif    // __GNUC__
  
    
    
    virtual bool ChangeSize (long new_size);
    // Expand ourselves to the given size. If new_size is less than
    // the current size, this operation truncates the sequence, i.e. the
    // elements with higher indices are lost. If the new sequence is
    // longer, the additional elements are initialized with the null value
    // of the {\small\tt BaseType} (i.e., the NULL pointer for pointer
    // types, and the value returned by the default constructor for
    // first-class objects).
    //    The method returns TRUE if successful, FALSE if no more memory.

    virtual void MakeEmpty ();
    // Delete all contained objects, and set our size to 0. If this is a
    // sequence of Object pointers, the contained objects are {\it not\/}
    // deleted.

    virtual bool ShiftRightAt (long pos, long amount = 1);
    // Shift all elements, beginning at position {\tt pos}, right by
    // {\tt amount} cells; then set the cells {\tt pos} through {\tt
    // pos+amount-1}  to
    // the null value of the base type. The new sequence will have its size
    // increased by {\tt amount}. The value {\tt pos} must be in the range 0 to
    // {\tt Size()-1}.
    //    This method returns TRUE on success, FALSE on failure (e.g., memory
    // allocation failure).
    //
    // The implementation performs {\small\tt Size() - pos} pointer
    // movements. These are pointer movements even when the {\small\tt
    // BaseType} is a first-class object.
    
    virtual bool ShiftLeftAt (long pos, long amount = 1);
    // Shift all elements, beginning at position "pos" upto our last
    // element, left by "amount" cells; this causes the sequence to lose the
    // elements that were at positions pos-amount through pos-1.
    // The new sequence will have its size decreased by "amount".
    //    Return TRUE on success, false on failure (e.g., memory
    // allocation failure).
    //
    // The implementation performs {\small\tt Size() - pos} pointer
    // movements. These are pointer movements even when the {\small\tt
    // BaseType} is a first-class object.
    

    virtual bool Sort ();
    // Sort the elements into ascending order. Return FALSE if either the
    // sequence was already sorted, memory allocation failed, or one of
    // the pre-change dependents refused permission, and TRUE otherwise.
    // The _Compare method is used for comparison. The implementation uses 
    // the _QuickSort method.

    virtual bool IsSorted () const;
    // Is this sequence sorted?


    // -------------------- Storage and retrieval ---------------------

    long StorableFormWidth () const;
    // Override the method inherited from {\small\tt CL_Object}.

    bool ReadFrom (const CL_Stream&);
    // Override the method inherited from {\small\tt CL_Object}.

    bool WriteTo  (CL_Stream&) const;
    // Override the method inherited from {\small\tt CL_Object}.

    
    // -------------------- Basic methods --------------------

    CL_Object* Clone () const;
    // Override the method inherited from {\small\tt CL_Object}.
    
    const char* ClassName() const {return "CL_Sequence";};
    // Override the method inherited from {\small\tt CL_Object}.

    CL_ClassId ClassId() const { return _CL_Sequence_CLASSID;};
    // Override the method inherited from {\small\tt CL_Object}.



    // -------------------- End public protocol ---------------------------

    
protected:

    //
    // Instance variables
    //
    void*              _idata;
    long               _size;
    CL_ObjectIOFilter* _builder;

    virtual bool       _QuickSort (long left, long right);
    // Apply QuickSort on the  segment of the sequence beginning at {\tt
    // left} and ending at {\tt right}. Return FALSE if this segment was
    // already sorted, and TRUE otherwise. This method does not perform
    // notification.

    virtual bool       _ReadElement (const CL_Stream& s, long i);
    // Read the i-th element of the sequence from the stream. This method
    // is used by {\tt ReadFrom}. The return value is TRUE if the
    // operation succeeds, and FALSE otherwise. The default implementation
    // simply calls {\tt CL_RestoreFrom} in {\tt basicops.h}.
    
    virtual bool       _WriteElement (CL_Stream& s, long i) const;
    // Write the i-th element of the sequence to the stream. This method
    // is used by {\tt WriteTo}. The return value is TRUE if the
    // operation succeeds, and FALSE otherwise. The default implementation
    // simply calls {\tt CL_SaveTo} in {\tt basicops.h}.
    
    virtual short      _Compare (const BaseType&, const BaseType&) const;
    // Compare two objects. All comparisons needed by all methods in the
    // Sequence are done by this method. The default implementation uses
    // the CL_Basics<BaseType>::Compare function.
    


private:

    bool           _SaveTo      (CL_ObjectPtr&, CL_Stream&) const;

    bool           _ShiftRightAt (long pos, long amount);
    // Do the ShiftRight without notifying clients.

    bool          _ShiftLeftAt  (long pos, long amount);
    // Do the ShiftLeft without notifying clients.

    short          _ChangeSize (long new_size);
    // Do the ChangeSize without notifying clients.
    
    short          _DoInsert (const BaseType& o, long index);
    // Do the Insert without notifying clients.

    void           _DoMakeEmpty ();
    // Do the MakeEmpty without notifying clients.
    
    // void           _Destructor ();
    // Added to sidestep the problems with gcc.

};







template <class BaseType>
inline long CL_Sequence<BaseType>::Size () const
{
    return _size;
}


template <class BaseType>
inline void CL_Sequence<BaseType>::operator= (const CL_Object& o)
{
    if (CheckClassType (o, "CL_Sequence::op="))
        *this = ((const CL_Sequence<BaseType>&) o);
}


template <class BaseType>
inline long CL_Sequence<BaseType>::Add (const BaseType& o)
{
    long pos = Size()-1;
    return Insert (o, pos) ? pos+1 : -1;
}




template <class BaseType>
inline CL_Object* CL_Sequence<BaseType>::Clone () const
{
    return new CL_Sequence<BaseType> (*this);
}



#endif  /* _sequence_h_ */

