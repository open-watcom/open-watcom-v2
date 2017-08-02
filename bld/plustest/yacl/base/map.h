

#ifndef _map_h_ /* Tue Jan 26 22:08:12 1993 */
#define _map_h_





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




#include "base/objseq.h"
#include "base/integer.h"

#ifdef __GNUC__
#pragma interface
#endif

template <class KeyClass, class ValueClass>
class __CLASSTYPE CL_MapIterator;


// A MapAssoc is simply a key-value pair that functions as a single entry
// in a map.

template <class KeyClass, class ValueClass>
class __CLASSTYPE CL_MapAssoc: public CL_Object {

public:
    KeyClass key;
    ValueClass value;

    CL_MapAssoc ();
    // Default constructor.

    CL_MapAssoc (const KeyClass& k, const ValueClass& v);
    // Build a MapAssoc with given key and value.
    
    CL_MapAssoc (const CL_MapAssoc<KeyClass,ValueClass>& a);
    // Copy constructor.
    
    void operator= (const CL_Object& o)
    { if (CheckClassType (o, "CL_MapAssoc::op="))
            *this = ((const CL_MapAssoc<KeyClass, ValueClass>&) o); };
    // Override method inherited from {\small\tt CL_Object}. The
    // implementation checks that o's class id is the same as ours, casts
    // down o and invokes the MapAssoc assignment.

    void operator= (const CL_MapAssoc<KeyClass,ValueClass>& o);

    short Compare (const CL_Object& o) const
        { return _Compare (o);};
    // This function is inlined to sidestep the gcc bug.

    CL_String AsString () const;

    // --------------------- Basic operations ---------------------
    
    CL_Object* Clone () const
        { return new CL_MapAssoc<KeyClass, ValueClass> (*this); };
    // Override the method inherited from {\small\tt CL_Object}.

    const char* ClassName () const {return "CL_MapAssoc";};
    // Override the method inherited from {\small\tt CL_Object}.
    

private:
    short _Compare (const CL_Object& o) const;
    // Non-virtual, to work around gcc bug

};




// This is a  Map class, that  maintains a key-value mapping.  Duplicate keys
// are {\it not\/}   allowed.  Keys must  be   objects that  define the  {\tt
// Compare()} method (or pointers to such objects), and values must belong to
// a class which provides a default constructor.  (A value object constructed
// with its  default constructor is thought  of as a  {\it null\/} value, and
// returned  when the map  lookup for a key fails.)   All methods that return
// boolean values return TRUE on success and FALSE on error.
// 
// A Map stores MapAssoc objects, which are key-value pairs.
// 
// A MapIterator object is also provided; this object allows inspecting
// the  key-value pairs (associations) contained in  a  map, in ascending
// order of  keys.  It  provides methods Reset(),  More() and  Next(), so
// that iteration in the following form is possible:
// \par\begin{verbatim}
//           CL_StringIntMapIterator iter (myMap);
//           CL_StringIntAssoc assoc;
//           for (iter.Reset(); iter.More(); ) {
//               assoc = iter.Next();
//               // Process the pair "assoc" here....
//           }
// \end{verbatim}
// Associations returned by the Next() method of  the iterator may NOT be
// modified. It is not  advisable to remove  elements from a map while an
// iterator on the map is active.
//
// When the Map is instantiated as a container for pointers (as are
// several maps -- see {\tt mapdef.h}), the map
// does {\it not\/} own the objects that the pointers point to, and
// therefore does not delete them when it is destroyed. The MapIterator
// can be used to iterate over the map's contents and destroy pointed-to
// objects.
//
// The implementation uses a sorted {\small\tt CL_Sequence}, so that it is
// possible to create maps with size up to $2^{26}$, or approximately 64
// million, even under MS-Windows, thus alleviating the 64K  limitation
// under MS-Windows (provided, of course, there is enough main memory
// available).



template <class KeyClass, class ValueClass>
class __CLASSTYPE CL_Map: public CL_Object {

public:

    // ---------------------- Construction and destruction ---------------

    CL_Map (CL_MapAssoc<KeyClass, ValueClass> *assoc_array, long count, 
            CL_ObjectIOFilter* key_builder = 0,
            CL_ObjectIOFilter* value_builder = 0);
    // Convenience constructor: build a map from a C-style array of
    // associations.
    
    CL_Map (CL_ObjectIOFilter* key_builder = 0,
            CL_ObjectIOFilter* value_builder = 0);
    // Default constructor: build an empty map. The builder objects are used
    // only if this map uses a pointer to a CL_Object (or derivative) for
    // either key or value; and they are used only when the map needs to be
    // restored from a stream. The builder objects are {\it not\/} owned by
    // this map, but must exist as long as the map does. 

    CL_Map (const CL_Map<KeyClass,ValueClass>& s);
    // Copy constructor.

    ~CL_Map () { _Destructor (); }
    // Destructor.
    
    //
    // ---------------------- Element access -----------------------------
    //

    long Size () const;
    // Return the number of entries in the map.
    
    virtual bool IncludesKey (const KeyClass& key) const;
    // Tell whether the map includes the given key.

    virtual ValueClass& operator[]  (const KeyClass& key);
    // Return the value associated with the given key. The
    // return value is a reference, and may be modified, resulting in
    // modification of the map. If the key is not found, this operator
    // returns a reference to an object of type ValueClass whose value
    // is the null value of the ValueClass.
    //
    // The implementation uses binary search when the number of keys exceeds
    // seven.

    virtual const CL_MapAssoc<KeyClass,ValueClass>& ItemWithRank (long i)
        const;
    // Given an index $i$ between 0 and Size()-1, return the Assoc of rank
    // $i$, i.e., the Assoc that has $i$ keys less than it in the map.
    // If $i \le 0$, this returns the Assoc with smallest key, and if $i
    // \ge {\tt Size()}$, this returns the one with the largest key.
    //
    // This is a const method; even if this
    // is a map of pointers, it is not safe to modify the object pointed
    // to by the return value because the map's internal representation
    // may be affected and its algorithms may perform incorrectly.
    //   Note that it is possible to iterate through the elements of the map
    // via calls to this method, varying the index from 0 to Size()-1;
    // however, depending on the implementation, this may lead to very
    // inefficient iteration. Use of the MapIterator is the recommended way
    // to inspect all elements of the set.
    
    virtual long RankOf (const KeyClass& k) const;
    // Return the number of MapAssoc's in this map whose keys less than k.
    // The key k need not be in the map. The ordering
    // relationship used is that defined by {\tt KeyClass};
    // if the latter does not define a Compare method or an {\tt
    // operator<} method, this method does not return anything useful.
    
    
    // ------------------------- Modification ---------------------------

    virtual bool Add  (const KeyClass& key, const ValueClass& value);
    // Add a key-value pair to the map. 

    virtual CL_MapAssoc<KeyClass, ValueClass> Remove  (const KeyClass& key);
    // Remove the given key and its associated value from the map. Return
    // the Assoc that was removed; return the null value of the MapAssoc if
    // the removal failed.

    virtual void MakeEmpty ();
    // Remove all the key-value pairs in the map. If this map uses
    // pointers in either key or value, the pointed-to objects are {\it
    // not\/} deleted.

    virtual void DestroyContents ();
    // Remove all key-value pairs  in the map. If this map uses
    // pointers in either key or value, the pointed-to objects {\it are\/}
    // deleted.
    
    void operator= (const CL_Map<KeyClass,ValueClass>&);
    // Assign the given map to ourselves.
    
    void operator= (const CL_Object& o);
    // Check that the given object has the same class id as this one,
    // and then perform a map assignment after casting down.
    

    // -------------------- Storage and retrieval ---------------------

    long StorableFormWidth () const;
    // Override the method inherited from {\small\tt CL_Object}.

    bool ReadFrom (const CL_Stream&);
    // Override the method inherited from {\small\tt CL_Object}.

    bool WriteTo  (CL_Stream&) const;
    // Override the method inherited from {\small\tt CL_Object}.

    void IntoStream (ostream& strm) const;
    // Override the method inherited from {\small\tt CL_Object}.

    
    //
    // -------------------- Basic inherited methods ---------------------
    //
    CL_Object* Clone () const
        {return new CL_Map<KeyClass,ValueClass> (*this);};
    // Override the method inherited from {\small\tt CL_Object}.
    
    virtual const char* ClassName() const
        { return "CL_Map";};
    // Override the method inherited from {\small\tt CL_Object}.
    
    virtual CL_ClassId ClassId () const
        {return _CL_Map_CLASSID;};
    // Override the method inherited from {\small\tt CL_Object}.




    // -------------------- End public protocol -------------------------

protected:

    // -------------------- Friend declarations ----------------------
    
    friend CL_MapIterator<KeyClass,ValueClass>;


    // -------------------- Protected methods -------------------------

    virtual CL_MapAssoc<KeyClass,ValueClass>*  _ReadAssoc (const CL_Stream&)
        const;
    // Read the data for a single MapAssoc, reconstruct it in a new object,
    // and return it. If the read fails, return a NULL pointer. This method
    // is called by {\tt ReadFrom}. The default implementation uses the
    // _keyBuilder and _valueBuilder variables if necessary.

    virtual bool _WriteAssoc (CL_Stream& s, const
                              CL_MapAssoc<KeyClass,ValueClass>& assoc)
        const;
    // Write a single MapAssoc value into the stream. This method is called
    // for each contained Assoc by the {\tt WriteTo} method. 

    // -------------------- Instance variables -----------------------
    
    CL_MapAssoc<KeyClass, ValueClass> _null;
    CL_ObjectSequence                 _data;
    CL_ObjectIOFilter*                _keyBuilder;
    CL_ObjectIOFilter*                _valueBuilder;

private:
    void _Destructor ();

    bool _ReadFrom (const CL_Stream&);

    bool _WriteTo  (CL_Stream&) const;
    
};




// This is MapIterator, that allows iteration over all entries in a map.

template <class KeyClass, class ValueClass>
class CL_MapIterator: public CL_Object {

public:
    CL_MapIterator (const CL_Map<KeyClass,ValueClass>& map);

    CL_MapIterator (const CL_MapIterator<KeyClass, ValueClass>& itr);
    // Copy constructor. The copy inspects the same map as {\tt itr}, and
    // (unless reset) begins  its iteration at the  Assoc at which {\tt itr}
    // is currently positioned.
    
    void Reset ();
    // Reset the iterator to the beginning (lowest-valued key). The next
    // call to Next() will return the Assoc with the lowest-valued key.

    virtual void BeginFromRank (long i);
    // Start the iteration so that the first call to {\tt Next} returns the
    // Assoc of rank i. Thus {\tt BeginFromRank(0)} is equivalent to {\tt
    // Reset()}.
    
    bool More  () const;
    // Return TRUE if there are more Assoc's in the map we're
    // inspecting.
    
    CL_MapAssoc<KeyClass, ValueClass>& Next ();
    // Return a reference to the next Assoc in the iteration. It is
    // NOT safe to modify the key in the returned Assoc, although the
    // value may be modified.



protected:
    const CL_Map<KeyClass,ValueClass>&  _map;
    long                                _index;

};





// ---------------------- Inline functions ------------------------





template <class KeyClass, class ValueClass>
inline long CL_Map<KeyClass, ValueClass>::Size () const
{
    return _data.Size();
}


template <class KeyClass, class ValueClass>
inline long CL_Map<KeyClass, ValueClass>::StorableFormWidth () const
{
    return sizeof (CL_ClassId) + _data.StorableFormWidth ();
}


template <class KeyClass, class ValueClass>
inline bool CL_Map<KeyClass, ValueClass>::ReadFrom (const CL_Stream& s)
{
    return _ReadFrom (s);
}

template <class KeyClass, class ValueClass>
inline bool CL_Map<KeyClass, ValueClass>::WriteTo  (CL_Stream& s) const
{
    return _WriteTo (s);
}



template <class KeyClass, class ValueClass>
inline void CL_Map<KeyClass, ValueClass>::operator= (const CL_Object& o)
{
    if (CheckClassType (o, "CL_Map::op="))
        *this = ((const CL_Map<KeyClass, ValueClass>&) o);
}







#ifndef _no_cl_map_typedefs_ /* Fri Nov 19 14:31:53 1993 */
#include "base/mapdef.h"
#endif /* _no_cl_map_typedefs_ */


#endif  /* _map_h_ */


