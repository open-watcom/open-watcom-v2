

#ifndef _set_h_
#define _set_h_





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



// This   is   a  template-based   Set   class, that  maintains   a  set of
// objects. Duplicate objects  are, of course,  not allowed.  The Set class
// allows addition and removal of objects and membership tests.
// 
// It  is also assumed that  (a) the  default  constructor for the BaseType
// class produces a value that can  be treated as  the "null value" of that
// class, (b)   the BaseType  class  supports a  copy  constructor   and an
// assignment operator, and (c)  the BaseType class supports the    Compare
// method. (This is because the set uses  a sorted array representation for
// maximum efficiency.)   Primitive  objects    that do not     support the
// Compare    method,  such as  long and int,  are provided  with "template
// override" methods in basicops.h.
// 
// A SetIterator object is also provided; this object allows inspecting the
// objects contained in  a set, in   ascending order.  It provides  methods
// Reset(), More() and Next(), so  that iteration in  the following form is
// possible:
// \par\begin{verbatim}
//           CL_StringSetIterator iter (my_set);
//           CL_String s;
//           for (iter.Reset(); iter.More(); ) {
//               s = iter.Next();
//               // Process the string s here....
//           }
// \end{verbatim}
// 
// Objects returned  by  the  Next()  method of  the  iterator  may NOT  be
// modified. It is not advisable to add or remove elements from a set while  an
// iterator on the set is active.
// 
// When the Set is instantiated as a container for pointers (as are
// {\tt CL_Set<CL_ObjectPtr>} or CL_ObjectSet), the set
// does {\it not\/} own the objects that the pointers point to, and
// therefore does not delete them when it is destroyed. The {\tt
// DestroyContents} method is provided on {\tt CL_ObjectSet} to
// provide explicit control over destruction of contents.
//
// The implementation uses a sorted {\small\tt CL_Sequence}, so that it is
// possible to create sets with size up to $2^{26}$, or approximately 64
// million, even under MS-Windows, thus alleviating the 64K  limitation
// under MS-Windows (provided, of course, there is enough main memory
// available).



#include "base/object.h"
#include "base/sequence.h"

#ifdef __GNUC__
#pragma interface
#endif



template <class BaseType> class CL_SetIterator;

template <class BaseType>
class __CLASSTYPE CL_Set: public CL_Object {

public:

    // ------------------------ Creation and destruction --------------

    CL_Set (CL_ObjectIOFilter* filter = 0);
    // Construct an empty set. Remember the given filter as the means of
    // reading and writing contained objects to streams. If specified, the
    // filter object must exist whenever the set needs to save or restore
    // itself from a stream; the set does not take responsibility for the
    // (memory used by the) filter object.

    // CL_Set (const CL_Sequence<BaseType>& s); Borland doesn't like this!
    // Create a set from the given Sequence, using the assignment operator.
    
    CL_Set (const CL_Set<BaseType>&);
    // Copy constructor. If the template parameter {\small\tt BaseType>}
    // is a first-class object, the copy constructor of the {\small\tt
    // BaseType} is used to copy each entry in the set; if it's a
    // pointer, just the pointer is copied.

    ~CL_Set ();
    // Destructor.

    //
    // ------------------------ Element access ---------------------------

    virtual long Size() const;
    // Return the number of objects in the set.

    virtual bool Add (const BaseType& o);
    // Add an object to the set. Return true on success.

    void operator+= (const BaseType& value);
    // Add the given value into the set.

    virtual BaseType Remove (const BaseType& o);
    // Remove the object equal to o from the set (if it's there). Return
    // the removed object if successful, and the null value of the base
    // type if failed. If this set is a pointer-based set, the return
    // value must be destroyed by the caller of this method.

    virtual bool Includes (const BaseType& o) const;
    // Determine if object o is in the set.

    virtual void MakeEmpty ();
    // Empty the set (i.e., remove all its elements). If this is a
    // pointer-based set, the contained objects are {\it not\/} deleted.

    // ------------------- Miscellaneous methods -------------------------

    virtual long RankOf (const BaseType& v) const;
    // Return the number of elements in this set that are less than v.
    // The parameter v need not be in the set. The ordering
    // relationship used is that defined by the base
    // type; if the latter does not define a Compare method or an {\tt
    // operator<} method, this method does not return anything useful.
    
    virtual const BaseType& ItemWithRank (long i) const;
    // Given an index $i$ between 0 and Size()-1, return the element of rank
    // $i$, i.e., the element that has $i$ elements less than it in the set.
    // If $i \le 0$, this returns the 
    // smallest element, and if $i \ge {\tt Size()}$, this returns the
    // largest element.
    //
    // This is a const method; even if this
    // is a set of pointers, it is not safe to modify the object pointed
    // to by the return value because the set's internal representation
    // may be affected and its algorithms may perform incorrectly.
    //   Note that it is possible to iterate through the elements of the set
    // via calls to this method, varying the index from 0 to Size()-1;
    // however, depending on the implementation, this may lead to very
    // inefficient iteration. Use of the SetIterator is the recommended way
    // to inspect all elements of the set.

    const BaseType& Smallest () const {return ItemWithRank(0);};
    // Return the smallest element in the set; return the null value of
    // the base type if the set is empty.
    
    const BaseType& Largest () const {return ItemWithRank(Size()-1);};
    // Return the largest element in the set; return the null value of
    // the base type if the set is empty.
    
    // ------------------- Manipulation of sets ------------------------

    virtual CL_Sequence<BaseType> AsSequence() const;
    // Return a sequence containing our data in ascending order.
    
    operator CL_Sequence<BaseType>  () const;
    // Return a sequence containing our data in ascending order. The
    // implementation simply invokes AsSequence() and returns its value.
    
    virtual bool operator== (const CL_Set<BaseType>& o) const;
    // Check if object o is the same as this set.

    virtual bool operator== (const CL_Object& o) const;
    // Check if object o is the same as this set.

    
    // ----- Assignments

    virtual void operator= (const CL_Set<BaseType>& s);
    // Assign the  given set to this one. If the template
    // parameter {\small\tt BaseType>} 
    // is a first-class object, the copy constructor of the {\small\tt
    // BaseType} is used to copy each entry in the set s; if it's a
    // pointer, just the pointer is copied.


    virtual void operator= (const CL_Sequence<BaseType>&);
    // Assign  the given sequence to  this set, with  duplicates removed.  If
    // the {\small\tt  BaseType} of this sequence is  a pointer, the pointers
    // are copied  into the returned  set; if the  {\small\tt  BaseType} is a
    // first-class object,  the copy  constructor of the {\small\tt BaseType}
    // is used   to  copy the  objects   into the returned  set.   Removal of
    // duplicates is done using BaseType's == operator.

    void operator= (const CL_Object& o);
    // Override method inherited from CL_Object.


    virtual CL_Set<BaseType> operator+   (const CL_Set<BaseType>& o) const;
    // Return the set containing all elements in either this set or o. The
    // implementation uses an algorithm that performs $m + n$ pointer
    // moves and as many comparisons, where $m$ and $n$ are the sizes of
    // this set and {\small\tt o}.
    
    virtual void operator+= (const CL_Set<BaseType>& o);
    // Add all elements in o to this set; return a reference to this set.
    
    virtual CL_Set<BaseType> operator* (const CL_Set<BaseType>& o) const; 
    // Intersection: Return the set containing the elements common between
    // this set and o. The
    // implementation uses an algorithm that performs $m + n$ pointer
    // moves and as many comparisons, where $m$ and $n$ are the sizes of
    // this set and {\small\tt o}.
        
    virtual void operator*= (const CL_Set<BaseType>& o);
    // Replace this set by its intersection with o.
    
    virtual CL_Set<BaseType> operator-   (const CL_Set<BaseType>& o) const;
    // Difference: return the set obtained by removing from this set those
    // elements that are common with o. The
    // implementation uses an algorithm that performs $m + n$ pointer
    // moves and as many comparisons, where $m$ and $n$ are the sizes of
    // this set and {\small\tt o}.
    
    virtual void operator-= (const CL_Set<BaseType>& o);
    // Remove from this set the elements in common with o.

    virtual bool IncludesAll (const CL_Set<BaseType>& s) const;
    // Return true if this set contains all elements of the given set. The
    // algorithm performs as many comparisons as the sum of the sizes of
    // this set and the given set.
    
    //
    // ---------------------- Storage and retrieval ----------------------
    //

    long StorableFormWidth() const;
    // Override the method inherited from {\small\tt CL_Object}.

    bool ReadFrom (const CL_Stream&);
    // Override the method inherited from {\small\tt CL_Object}.

    bool WriteTo  (CL_Stream&) const;
    // Override the method inherited from {\small\tt CL_Object}.

    

    //
    // ------------------------- Basic methods --------------------
    //
    CL_Object* Clone () const
        {return new CL_Set<BaseType> (*this);};
    // Override the method inherited from {\small\tt CL_Object}.
    
    const char* ClassName() const {return "CL_Set";};
    // Override the method inherited from {\small\tt CL_Object}.
    
    CL_ClassId ClassId() const
        { return _CL_Set_CLASSID;};
    // Override the method inherited from {\small\tt CL_Object}.



    // -------------------- End public protocol ---------------------------

    
protected:

    // ---------------------- Friend declarations -----------------------
    
    friend CL_SetIterator<BaseType>;

    
    // ----------------------- Instance variables -----------------------
    
    void*              _idata;   // Representation of the Set
    CL_ObjectIOFilter* _filter;
    BaseType           _null;    // I wish this could be a static instance
                                 // var, but GCC doesn't implement static
                                 // template inst vars :-(

    
    // ------------------------ Protected methods ----------------------
    
    CL_Set (void* p);
    // Protected constructor, for use by derived classes. This constructor
    // assumes that its parameter p is the data representation, and sets
    // _idata to p.

    virtual bool       _ReadElement (const CL_Stream& s, BaseType& e);
    // Read an element e of the set from the stream. This method
    // is used by {\tt ReadFrom}. The return value is TRUE if the
    // operation succeeds, and FALSE otherwise. The default implementation
    // simply calls {\tt CL_RestoreFrom} in {\tt basicops.h}.
    
    virtual bool       _WriteElement (CL_Stream& s, const BaseType& e) const;
    // Write an element e of the set to the stream. This method
    // is used by {\tt WriteTo}. The return value is TRUE if the
    // operation succeeds, and FALSE otherwise. The default implementation
    // simply calls {\tt CL_SaveTo} in {\tt basicops.h}.
    
    
private:
    
    bool _OpEqual (const CL_Set<BaseType>&) const;
    // To sidestep the GCC bug.

    void _Destructor ();

    bool _ReadFrom (const CL_Stream&);

    bool _WriteTo  (CL_Stream&) const;

};




template <class BaseType>
class __CLASSTYPE CL_SetIterator: public CL_Object {

public:

    CL_SetIterator (const CL_Set<BaseType>& s);
    // Constructor. The parameter specifies the set to be inspected.

    CL_SetIterator (const CL_SetIterator<BaseType>& itr);
    // Copy constructor. The copy inspects the same set as {\tt itr}, and
    // (unless reset) begins  its iteration at the same object that itr is
    // currently positioned.
    
    virtual void Reset();

    virtual void BeginFromRank (long i);
    // Start the iteration so that the first call to {\tt Next} returns the
    // element of rank i. Thus {\tt BeginFromRank(0)} is equivalent to {\tt
    // Reset()}.
    
    virtual bool More() const;

    virtual const BaseType& Next();

    virtual const char* ClassName () const {return "CL_SetIterator";};
    
//     bool operator== (const CL_SetIterator<BaseType>&) const;
//     virtual bool operator== (const CL_Object& o) const
//        { return *this == ((const CL_SetIterator<BaseType>&) o);};


protected:
    const CL_Set<BaseType>& _set;
    long                    _index;

private:

    // These added to work around the GNU bug

    //    bool _OpEqual (const CL_SetIterator<BaseType>&) const;
  
};





// template <class BaseType>
// inline CL_Set<BaseType>::CL_Set  (const CL_Sequence<BaseType>& seq)
// {
//     *this = seq;
// }


template <class BaseType>
inline bool CL_Set<BaseType>::operator== (const CL_Set<BaseType>& o) const
{
    return _OpEqual (o);
}




template <class BaseType>
inline long CL_Set<BaseType>::StorableFormWidth () const
{
    return sizeof (CL_ClassId) +
        (*(CL_Sequence<BaseType> *)_idata).StorableFormWidth ();
}


template <class BaseType>
inline bool CL_Set<BaseType>::ReadFrom (const CL_Stream& s)
{
    return _ReadFrom (s);
}

template <class BaseType>
inline bool CL_Set<BaseType>::WriteTo  (CL_Stream& s) const
{
    return _WriteTo (s);
}


template <class BaseType>
inline CL_Set<BaseType>::~CL_Set  ()
{
    _Destructor ();
}


// template <class T>
// inline bool CL_SetIterator<T>::operator==
//     (const CL_SetIterator<T>& o) const
// {
//     return _OpEqual (o);
// }


template <class BaseType>
inline long CL_Set<BaseType>::Size () const
{
    return (*(CL_Sequence<BaseType>*)_idata).Size();
}



template <class BaseType>
inline CL_Set<BaseType>::operator CL_Sequence<BaseType>  () const
{
    return AsSequence ();
}


template <class BaseType>
inline bool CL_Set<BaseType>::operator== (const CL_Object& o) const
{
    return ClassId() == o.ClassId() && *this == ((const CL_Set<BaseType>&) o);
}

template <class BaseType>
inline void CL_Set<BaseType>::operator= (const CL_Object& o)
{
    if (CheckClassType (o, "CL_Set::op="))
        *this = ((const CL_Set<BaseType>&) o);
}


template <class BaseType>
inline CL_Sequence<BaseType> CL_Set<BaseType>::AsSequence () const
{
    return *(CL_Sequence<BaseType>*)_idata;
}

#ifndef _no_cl_set_typedefs_
// #include "base/setdef.h"
#endif /* _no_cl_set_typedefs_ */


#endif /* _set_h_ */


