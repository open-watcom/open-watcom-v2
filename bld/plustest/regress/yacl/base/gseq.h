

#ifndef _gseq_h_ /* Sun Aug 14 13:33:41 1994 */
#define _gseq_h_



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


#include "base/defs.h"
#include "base/cmparatr.h"


class CL_CellSequence {

public:
    CL_CellSequence (long initial_cap = 0);
    
    ~CL_CellSequence ();

    CL_VoidPtr& operator[] (long index) const;
    
    bool ResizeTo (long new_size);


protected:
    struct SegDesc {
        short _cap;  // Number of cells
        long* _data; // The cells themselves
    };
    SegDesc*               _segs;
    short                  _numSegs;
    long                   _totalCap;
    long                   _size;
};

class CL_GenericSequence: public CL_CellSequence {

public:
    CL_GenericSequence (const CL_AbstractComparator& cmp, long size = 0);
    
    ~CL_GenericSequence ();


    long Size() const {return _size;};
    
    bool Add (CL_VoidPtr p) {return Insert (p, _size);};

    bool Insert (CL_VoidPtr p, long position = -1);

    virtual CL_VoidPtr Remove (long i);
    // Remove the i-th element of the sequence, and close the hole, so
    // that all elements formerly at indices higher than i will now
    // have their indices decremented by 1. Return the removed element, or
    // NULL if i was invalid.
    //
    // The implementation performs {\small\tt Size() - index} pointer
    // movements. 

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
    // movements. 
    
    virtual bool ShiftLeftAt (long pos, long amount = 1);
    // Shift all elements, beginning at position "pos" upto our last
    // element, left by "amount" cells; this causes the sequence to lose the
    // elements that were at positions pos-amount through pos-1.
    // The new sequence will have its size decreased by "amount".
    //    Return TRUE on success, false on failure (e.g., memory
    // allocation failure).
    //
    // The implementation performs {\small\tt Size() - pos} pointer
    // movements. 


    virtual long LinearSearch (CL_VoidPtr o) const;
    // Do a linear search for the given object in the sequence. Return
    // the index at which it was found, or -1 if not found.

    virtual bool BinarySearch (CL_VoidPtr o, long& index) const;
    // Assuming that the sequence is sorted, search for a given element,
    // and return a boolean whether it was found. Return the index of
    // the greatest element not exceeding the given element in the
    // second parameter. This method performs a binary search for
    // sequences larger than 7 elements, but does not check whether the
    // sequence is sorted; so it must only be used on sequences that are
    // known to be sorted. (See the {\small\tt Sort} method.)


    virtual bool Sort ();
    // Sort the elements into ascending order. Return FALSE if either the
    // sequence was already sorted, or memory allocation failed and TRUE
    // otherwise. The implementation uses the _QuickSort method.

    virtual bool IsSorted () const;
    // Is this sequence sorted?

protected:

    //
    // Instance variables
    //

    const CL_AbstractComparator& _cmp;

    virtual bool       _QuickSort (long left, long right);
    // Apply QuickSort on the  segment of the sequence beginning at {\tt
    // left} and ending at {\tt right}. Return FALSE if this segment was
    // already sorted, and TRUE otherwise.

    virtual short      _Compare (CL_VoidPtr p1, CL_VoidPtr p2) const;
    
};





CL_VoidPtr& CL_CellSequence::operator[] (long index) const
{
#ifdef YACLDEBUG
    assert ((index >= 0 && index < _size),
            ("CellSeq::op[]: invalid index %ld size %ld", index, _size));
#endif
    return (CL_VoidPtr&) (_segs[index >> 13]._data[index & 0x1fff]);
}


#endif /* _gseq_h_ */
