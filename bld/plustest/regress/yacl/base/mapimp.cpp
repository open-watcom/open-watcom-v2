



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




#ifndef _mapimp_cxx_
#define _mapimp_cxx_  /* Prevent multiple includes */

#ifdef __GNUC__
#pragma implementation
#endif

#define _no_cl_map_typedefs_
#include "base/map.h"
#include "base/basicops.h"
#include "base/stream.h"

#include <iostream.h>


#ifdef __BORLANDC__
#pragma warn -lvc
#endif




// -------------------- MapAssoc methods -----------------------------


template <class Key, class Value>
short CL_MapAssoc<Key, Value>::_Compare (const CL_Object& o) const
{
    return CL_Basics<Key>::Compare
        (key, ((const CL_MapAssoc<Key,Value>&) o).key);
}


template <class KeyClass, class ValueClass>
CL_MapAssoc<KeyClass, ValueClass>::CL_MapAssoc ()
{
    key   = CL_Basics<KeyClass>  ::NullValue();
    value = CL_Basics<ValueClass>::NullValue();
}


template <class KeyClass, class ValueClass>
CL_MapAssoc<KeyClass, ValueClass>::CL_MapAssoc
(const KeyClass& k, const ValueClass& v)
{
    key = k; value = v;
}

template <class KeyClass, class ValueClass>
CL_MapAssoc<KeyClass, ValueClass>::CL_MapAssoc
(const CL_MapAssoc<KeyClass,ValueClass>& a)
{
    key = a.key; value = a.value;
}

template <class KeyClass, class ValueClass>
void CL_MapAssoc<KeyClass, ValueClass>::operator=
(const CL_MapAssoc<KeyClass,ValueClass>& o)
{
    if (this != &o) {
        if (PrepareToChange()) {
            key   = o.key;
            value = o.value;
            Notify();
        }
    }
};



template <class KeyClass, class ValueClass>
CL_String CL_MapAssoc<KeyClass, ValueClass>::AsString () const
{
    return CL_Basics<KeyClass>::PrintableForm(key) + " --> "
        + CL_Basics<ValueClass>::PrintableForm (value);
}








// ------------------------ Map methods ---------------------------





template <class KeyClass, class ValueClass>
CL_Map<KeyClass, ValueClass>::CL_Map
    (CL_ObjectIOFilter* key_builder,
     CL_ObjectIOFilter* value_builder)
{
    _null = CL_Basics<CL_MapAssoc<KeyClass,ValueClass> >::NullValue ();
    // Maybe when gcc supports static template instance vars, I'll make
    // _null a static inst var.
    _keyBuilder = key_builder;
    _valueBuilder = value_builder;
}





template <class KeyClass, class ValueClass>
CL_Map<KeyClass, ValueClass>::CL_Map
    (CL_MapAssoc <KeyClass, ValueClass> *assoc, long count,
     CL_ObjectIOFilter* key_builder,
     CL_ObjectIOFilter* value_builder)
{
    ValueClass p;
    _keyBuilder = key_builder;
    _valueBuilder = value_builder;
    if (!assoc)
        return;
    for (long i = 0; i < count; i++) {
        Add (assoc[i].key, assoc[i].value);
    }
}


template <class KeyClass, class ValueClass>
CL_Map<KeyClass, ValueClass>::CL_Map
(const CL_Map<KeyClass, ValueClass>& m)
: _data (m.Size())
{
    long n = m.Size();
    for (long i = 0; i < n; i++)
        _data[i] = new CL_MapAssoc<KeyClass, ValueClass>
            (* (CL_MapAssoc<KeyClass, ValueClass>*) m._data[i]);
    _keyBuilder   = m._keyBuilder;
    _valueBuilder = m._valueBuilder;
}

         



//
// ---------------------- Element access -----------------------------
//

// IncludesKey: Tell whether the map includes the given key.
template <class KeyClass, class ValueClass>
bool CL_Map<KeyClass, ValueClass>::IncludesKey (const KeyClass& key) const
{
    long index = 0;
    CL_MapAssoc<KeyClass,ValueClass> a;
    a.key = key;
    return (_data.BinarySearch (&a, index));
}


// op[]: return the value associated with the given key. The
// return value is a reference, and may be modified, resulting in
// modification of the map.
template <class KeyClass, class ValueClass>
ValueClass& CL_Map<KeyClass, ValueClass>::operator []
    (const KeyClass& key)
{
    long index = 0;
    CL_MapAssoc<KeyClass,ValueClass> a;
    ValueClass p;

    a.key = key;
    if (_data.BinarySearch (&a, index)) {
        return ((CL_MapAssoc<KeyClass,ValueClass> *)_data[index])->value;
    }
    else {
        _null.value = CL_Basics<ValueClass>::NullValue ();
        return _null.value;
    }
}

template <class Key, class Value>
const CL_MapAssoc<Key, Value>& CL_Map<Key, Value>::ItemWithRank (long i) const
{
    i = maxl (0, minl (i, Size() - 1));
    return *(CL_MapAssoc<Key, Value>*) _data[i];
}



template <class Key, class Value>
long CL_Map<Key, Value>::RankOf (const Key& key) const
{
    long index = 0;
    Value p;
    CL_MapAssoc<Key,Value> a (key, p);
    bool b = _data.BinarySearch (&a, index);
    return b ? index : index+1;
}


//
// ------------------------- Modification ---------------------------
//


template <class KeyClass, class ValueClass>
bool CL_Map<KeyClass, ValueClass>::Add (const KeyClass& key,
                                        const ValueClass& value) 
{
    if (!PrepareToChange ())
        return FALSE;
    CL_MapAssoc<KeyClass,ValueClass>* p =
        new CL_MapAssoc<KeyClass,ValueClass> (key, value);

    if (!p)
        return FALSE;
    long index = 0;
    if (_data.BinarySearch (p, index)) {
        delete p;
        return FALSE;
    }
    if (_data.Insert (p, index)) {
        Notify ();
        return TRUE;
    }
    return FALSE;
}


    

template <class KeyClass, class ValueClass>
CL_MapAssoc<KeyClass, ValueClass> CL_Map<KeyClass, ValueClass>::Remove
     (const KeyClass& key)
{
    if (!PrepareToChange ())
        return CL_Basics<CL_MapAssoc<KeyClass, ValueClass> >::NullValue();
    CL_MapAssoc<KeyClass,ValueClass> a;
    a.key = key;

    long i;
    if (!_data.BinarySearch (&a, i))
        return CL_Basics<CL_MapAssoc<KeyClass, ValueClass> >::NullValue();
    CL_MapAssoc<KeyClass,ValueClass>* p =
        (CL_MapAssoc<KeyClass,ValueClass>*) _data[i];
    if (_data.Remove (i)) {
        CL_MapAssoc<KeyClass, ValueClass> a = *p;
        delete p;
        Notify ();
        return a;
    }
    return CL_Basics<CL_MapAssoc<KeyClass, ValueClass> >::NullValue();
}




template <class KeyClass, class ValueClass>
void CL_Map<KeyClass, ValueClass>::operator=
    (const CL_Map<KeyClass,ValueClass>& o)
{
    if (this == &o)
        return;
    if (!PrepareToChange())
        return;
    _data.DestroyContents ();
    if (!_data.ChangeSize (o._data.Size()))
        return;
    for (long i = 0; i < o._data.Size(); i++)
        _data[i] = new CL_MapAssoc<KeyClass, ValueClass>
            (*(CL_MapAssoc<KeyClass, ValueClass>*) o._data[i]);
    Notify();
}



template <class KeyClass, class ValueClass>
void CL_Map<KeyClass, ValueClass>::_Destructor ()
{
    _data.DestroyContents ();
}


template <class KeyClass, class ValueClass>
void CL_Map<KeyClass, ValueClass>::MakeEmpty ()
{
    _data.DestroyContents ();
}


template <class KeyClass, class ValueClass>
void CL_Map<KeyClass, ValueClass>::DestroyContents ()
{
    long size = _data.Size();
    for (long i = 0; i < size; i++) {
        CL_MapAssoc<KeyClass, ValueClass>* p =
            (CL_MapAssoc<KeyClass, ValueClass>*) _data[i];
        CL_Basics<KeyClass>::ReallyDestroy (p->key);
        CL_Basics<ValueClass>::ReallyDestroy (p->value);
    }
    _data.DestroyContents ();
}



template <class KeyClass, class ValueClass>
void CL_Map<KeyClass, ValueClass>::IntoStream (ostream& strm) const
{
    register long size = _data.Size();
    for (register long i = 0; i < size; i++) {
        CL_MapAssoc<KeyClass, ValueClass>* p =
            (CL_MapAssoc<KeyClass, ValueClass>*) _data[i];
        strm << p->AsString() << endl;
    }
}



template <class KeyClass, class ValueClass>
CL_MapAssoc<KeyClass, ValueClass>*
CL_Map<KeyClass, ValueClass>::_ReadAssoc (const CL_Stream& s) const
{
    CL_MapAssoc<KeyClass, ValueClass>* m = new CL_MapAssoc
        <KeyClass, ValueClass>;
    if (!m)
        return NULL;
    if (!CL_RestoreFrom (m->key,   s, _keyBuilder) ||
        !CL_RestoreFrom (m->value, s, _valueBuilder))
        return NULL;
    return m;
}

template <class KeyClass, class ValueClass>
bool CL_Map<KeyClass, ValueClass>::_ReadFrom (const CL_Stream& s)
{
    _data.DestroyContents ();
    long sz;
    if (!s.Read (sz) || !_data.ChangeSize (sz))
        return FALSE;
    KeyClass k;
    ValueClass v;
    for (long i = 0; i < sz; i++) {
        CL_MapAssoc<KeyClass, ValueClass>* m = _ReadAssoc (s);
        if (!m)
            return FALSE;
        _data[i] = m;
    }
    return TRUE;
}




template <class KeyClass, class ValueClass>
bool CL_Map<KeyClass, ValueClass>::_WriteAssoc
    (CL_Stream& s, const CL_MapAssoc<KeyClass,ValueClass>& assoc) const
{
    return CL_SaveTo (assoc.key, s, _keyBuilder) &&
        CL_SaveTo (assoc.value, s, _valueBuilder);
}

template <class KeyClass, class ValueClass>
bool CL_Map<KeyClass, ValueClass>::_WriteTo (CL_Stream& s) const
{
    long size = _data.Size();
    if (!s.Write (size))
        return FALSE;
    for (long i = 0; i < size; i++) {
        CL_MapAssoc<KeyClass, ValueClass>* m = (CL_MapAssoc<KeyClass,
                                                ValueClass>*) _data[i];
        if (!m || !_WriteAssoc (s, *m))
            return FALSE;
    }
    return TRUE;
}








// -------------------------- MapIterator methods ---------------------

template <class KeyClass, class ValueClass>
CL_MapIterator<KeyClass, ValueClass>::CL_MapIterator
    (const CL_Map<KeyClass,ValueClass>& map)
: _map (map)
{
    _index = 0;
}


template <class KeyClass, class ValueClass>
CL_MapIterator<KeyClass, ValueClass>::CL_MapIterator
    (const CL_MapIterator<KeyClass, ValueClass>& itr)
: _map (itr._map)
{
    _index = itr._index;
}


template <class KeyClass, class ValueClass>
void CL_MapIterator<KeyClass, ValueClass>::Reset()
{
    _index = 0;
}


template <class KeyClass, class ValueClass>
void CL_MapIterator<KeyClass, ValueClass>::BeginFromRank (long l)
{
    _index = maxl (l, 0);
}


template <class KeyClass, class ValueClass>
bool CL_MapIterator<KeyClass, ValueClass>::More() const
{
    return _index < _map.Size();
}




template <class KeyClass, class ValueClass>
CL_MapAssoc<KeyClass, ValueClass>& CL_MapIterator<KeyClass, ValueClass>::Next()
{
    if (_index < _map.Size()) {
        CL_MapAssoc<KeyClass, ValueClass>* p =
            (CL_MapAssoc<KeyClass, ValueClass>*) _map._data[_index];
        _index++;
        return *p;
    }
    return (CL_MapAssoc<KeyClass, ValueClass>&) _map._null;
    //-----^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ cast away const
//     static CL_MapAssoc<KeyClass,ValueClass> nullValue =
//         CL_Basics<CL_MapAssoc<KeyClass,ValueClass> >::NullValue();
//     return nullValue;
}


    
    
// template <class KeyClass, class ValueClass>
// CL_MapAssoc<KeyClass, ValueClass>&
// CL_MapIterator<KeyClass, ValueClass>::Previous ()
// {
//     if (_index >= 0) {
//         CL_MapAssoc<KeyClass, ValueClass>* p =
//             (CL_MapAssoc<KeyClass, ValueClass>*) _map._data[_index];
//         _index--;
//         return *p;
//     }
//     static CL_MapAssoc<KeyClass,ValueClass> nullValue =
//         CL_Basics<CL_MapAssoc<KeyClass,ValueClass> >::NullValue();
//     return nullValue;
// }


    
    

#endif /* _mapimp_cxx_ */


