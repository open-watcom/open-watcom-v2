

#ifndef _bitset_h_ /* Mon Nov  8 09:02:26 1993 */
#define _bitset_h_





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


// This is a class encapsulating the notion of a {\it BitSet,} a set of
// integers represented by a bit vector. It can store a set of integers in
// the range 0..max-1, where max is the maximum cardinality possible. The
// maximum cardinality is the parameter to the constructor.
//
// The bit-vector representation allows very efficient storage and access
// algorithms for small, dense sets of integers. (Dense sets are those in
// which the number of elements is a significant proportion of the maximum
// cardinality.)
//



#include "base/intset.h"
#include "base/bytstrng.h"


class CL_BitSet: public CL_Object {

public:

    // --------------------- Construction and destruction -------

    CL_BitSet  (long max_size = 128);
    // Construct a bitset that can contain at most the elements between 0
    // and max_size-1.

    CL_BitSet (long low, long hi, long max_size = 128);
    // Build a set with containing all the numbers in the range
    // low through hi. If {\tt lo < hi}, the empty set is built.
    
    CL_BitSet  (CL_ByteArray& b);
    // Restore this bitset from the given byte array. The latter should
    // contain the passive representation of a BitSet, produced by the
    // WriteTo method on a ByteStream.

    CL_BitSet  (const CL_BitSet& b);
    // Copy constructor.

    ~CL_BitSet();
    // Destructor.
    
    

    // ----------------------- Element methods ------------------

    virtual bool Add     (const long& value);
    // Add a value. Return TRUE on success, FALSE if the element was already
    // in the set.

    virtual long Remove    (const long& value);
    // Remove a value. Return the removed value if it was already in the
    // set, and 0 otherwise. (This signature matches the one inherited
    // from {\tt IntegerSet}.) This signature 

    virtual bool Includes  (const long& value) const;
    // Check whether the given element is in the set.

    virtual long Smallest () const;
    // Return the smallest integer in the set. Return -1 if the set is
    // empty.
    
    virtual long Largest () const;
    // Return the largest integer in the set. Return -1 if the set is
    // empty.
    
    virtual long RankOf (const long& v) const;
    // Return the number of elements in this set that are less than v.
    // The parameter v need not be in the set.
    
    virtual long ItemWithRank (long i) const;
    // Given an index $i$ between 0 and Size()-1, return the element of rank
    // $i$, i.e., the element that has $i$ elements less than it in the set.
    // If $i \le 0$, this returns the 
    // smallest element, and if $i \ge {\tt Size()}$, this returns the
    // largest element. If the set is empty, return -1.
    //
    //   Note that it is possible to iterate through the elements of the set
    // via calls to this method, varying the index from 0 to {\tt Size()-1};
    // however, this is very inefficient iteration. Use of the BitSetIterator
    // is the recommended way to inspect all elements of the BitSet.

    virtual long Successor (long i) const;
    // Return the least number in the set that is larger than the parameter.
    // Return -1 if the parameter equals or exceeds the largest value in the
    // set.
    
    virtual long SmallestNonMember () const;
    // Return the smallest element that is {\it not\/} in the set. Return -1 if
    // the set is universal.
    
    bool IsEmpty   () const;
    // Return TRUE if the bitset is empty, FALSE otherwise.

    virtual long Size     () const;
    // Return the current cardinality of the set.

    virtual CL_BitSet operator+ (long value) const;
    // Return a new set which contains our contents augmented with the given
    // value.
    
    // ----------------------- Set methods ----------------------

    virtual void MakeEmpty ();
    // Make this the empty set.
    
    virtual void MakeUniversal ();
    // Make this the universal set (i.e., the set containing all elements
    // between 0 and our maximum size possible).

    virtual bool IsUniversal () const;
    // Return TRUE if this is this the universal set, FALSE otherwise.

    virtual CL_IntegerSet AsSet () const;
    // Convert this bitset into a set of long values, and return the result.

    operator CL_IntegerSet() const {return AsSet(); };
    // Convert this bitset into a set of long values, and return the result.

    // ----- Comparisons:
    
    virtual bool operator== (const CL_BitSet& o) const;
    
    virtual bool operator== (const CL_IntegerSet& o) const;
    
    // ----- Assignment operators:
    
    virtual void operator= (const CL_Object& o);
    // Check that the class id of the given object is the same as ours,
    // and assign the result to this class. An invalid class id results in
    // a runtime error message.
                     
    virtual CL_BitSet& operator= (const CL_BitSet& s);
    // Assign the given BitSet to this object.

    // ----- Binary set operators:
    
    // The binary set operations union, intersection and difference all
    // yield sets whose maximum size is the larger of the maximum sizes of
    // the two operands.

    virtual CL_BitSet operator+ (const CL_BitSet& s) const;
    // Return the union of this set and s.

    virtual void operator+= (const CL_BitSet& o);
    // Add the elements of o to this set.

    virtual CL_BitSet operator- (const CL_BitSet& o) const;
    // Difference: return the set obtained by removing from this set those
    // elements that are common with o.

    virtual void operator-= (const CL_BitSet& o);
    // Remove from this set the elements in common with o.

    virtual CL_BitSet operator* (const CL_BitSet& o) const;
    // Intersection: Return the set containing the elements common between
    // this set and o.

    virtual void operator*= (const CL_BitSet& o);
    // Replace this set by its intersection with o.

    virtual CL_BitSet operator~ () const;
    // Complementation: Return the set containing those elements not in
    // this set, but in the universal set.

    virtual bool IncludesAll (const CL_BitSet& o) const;
    // Return TRUE if this BitSet contains all the elements that o does, and
    // FALSE otherwise.
    
    virtual bool IncludesAll (const CL_IntegerSet& o) const;
    // Return TRUE if this BitSet contains all the elements that o does, and
    // FALSE otherwise.
    
    // --------------------- Saving and restoring -----------------------
    
    long StorableFormWidth () const;
    
    virtual bool ReadFrom (const CL_Stream&);

    virtual bool WriteTo  (CL_Stream&) const;


    // -------------------- Basic inherited methods ---------------------

    const char* ClassName() const { return "CL_BitSet";};

    CL_ClassId ClassId () const {return _CL_BitSet_CLASSID;};

    bool operator== (const CL_Object& o) const;

    CL_String AsString () const {return AsSet().AsString (); };
    
    // --------------------- End public protocol ------------------------

    
protected:

    friend class CL_BitSetIterator;
    
    ulong*  _rep;
    long    _size;  // Current cardinality
    long    _count; // # ulongs in representation

    CL_BitSet (long wordCount, ulong array[]);

    CL_BitSet _DoOp (const CL_BitSet&, void (*o)(ulong&, ulong)) const;

};






// This is an object that allows iteration over a BitSet, and provides the
// usual iteration methods Reset, More and Next.
//
// {\bf Caution.} Do {\it not\/} instantiate a BitSetIterator on an
// IntegerSet; even though the BitSet is derived from IntegerSet, the
// internal representations of the two are very different.


class CL_BitSetIterator: public CL_Object {

public:
    CL_BitSetIterator (const CL_BitSet& o);
    // Constructor: tells us which set we're inspecting.

    void Reset();
    // Reset the cursor to the beginning of the set.

    void BeginFromRank (long l);
    // Begin the iteration from the element of rank l. The first call to
    // Next() returns the element of rank l.
    
    bool More () const;
    // Are there more elements in the iteration?

    long Next();
    // Return the next element in sequence. Return -1 if no more. Elements
    // are returned in ascending order.

protected:
    const CL_BitSet& _set;         // Our set
    long             _index;       // Index of word being looked at
    long             _count;       // Number of elements returned so far
    long             _tblIndex;    // Index into table of bits
};






inline bool CL_BitSet::IsEmpty () const
{
    return Size() == 0;
}

inline long CL_BitSet::Size     () const
{
    return _size;
}

inline void CL_BitSet::operator = (const CL_Object& o)
{
    if (CheckClassType (o, "CL_BitSet::op= (CL_Object&)"))
        *this = ((const CL_BitSet&) o);
};



inline bool CL_BitSet::operator == (const CL_Object& o) const
{
    if (ClassId() != o.ClassId())
        return FALSE;
    return *this == ((const CL_BitSet&) o);
};


inline void CL_BitSet::operator += (const CL_BitSet& o)
{
    *this = *this + o;
}

inline void CL_BitSet::operator -= (const CL_BitSet& o)
{
    *this = *this - o;
}

inline void CL_BitSet::operator *= (const CL_BitSet& o)
{
    *this = *this * o;
}


#endif /* _bitset_h_ */


