/****************************************************************************
File: OrdKeyMap.h

Description: class JAM_OrderedKeyMapComp<K,V,Comp>
             class JAM_OrderedKeyMap<K,V>
   A Map is an ordered set of (key,value) pairs.

Usage:

Notes:
   T must define a copy-ctor, assignment operator, equality operator,
   and the less than comparison operator '<'.

   Changes should not be made to the map while iterators are alive
   unless only one iterator is alive and the changes are made through
   that iterator.

History:
1992 Feb 20 Jam      created from JAM_List, name changes, etc.
1992 Feb 22 Jam      added JAM_MapIter
1992 Mar 20 Jam      remove stream ops as friends so not instantiated
1992 Jun 16 Jam      redid stream ops as friends for parse stuff and
                     added as friends to JAM_KeyValue for instantiation
1992 Jul 09 Kevin    added non-const elem()
1992 Aug 03 Jam      remade JAP_Map friend of JAM_MapIter, and derived
                     privately -- apparently BC++ 3.1 fixed 3.0 bug
1992 Oct 07 Jam      renamed JAM_Map JAM_OrdKeyMap and made OrdSet data
                     member instead of deriving from it
1993 Mar 23 Jam      remiter() takes a non-const state; might become invalid
1993 Mar 30 Jam      OrdKeyMapCompMuter corrected and OrdKeyMapMuter added

****************************************************************************/

#ifndef JAM_OrdKeyMap_H
#define JAM_OrdKeyMap_H

#include <iostream.h>
#include <assert.hpp>
#include <OrdSet.h>
#include <Iter.h>
#include <Comparat.h> //## or

//**************************************************************************
// JAM_KeyValue definition                                                   
//**************************************************************************

template<class K, class V, class KeyComp> struct JAM_KeyValue {
   K _key;
   V _value;
   JAM_KeyValue() {}
   JAM_KeyValue(const K& key) : _key(key) {}
   JAM_KeyValue(const K& key, const V& value) : _key(key), _value(value) {}
   JAM_KeyValue(const JAM_KeyValue<K,V,KeyComp>& kv)
      : _key(kv._key), _value(kv._value) {} //## should not be necessary

   int operator==(const JAM_KeyValue<K,V,KeyComp>& kv) const
      { return KeyComp::equal(_key,kv._key); }
   int operator<(const JAM_KeyValue<K,V,KeyComp>& kv) const
      { return KeyComp::lessthan(_key,kv._key); }
   friend ostream& operator<<(ostream& os, const JAM_KeyValue<K,V,KeyComp>& kv);
   friend istream& operator>>(istream& is, JAM_KeyValue<K,V,KeyComp>& kv);
};


//**************************************************************************
// JAM_OrdKeyMapComp definition
//**************************************************************************

template<class K, class V, class KeyComp> class JAM_OrdKeyMapComp {
public:
   JAM_OrdKeyMapComp() {}

   JAM_OrdKeyMapComp(const JAM_OrdKeyMapComp<K,V,KeyComp>& m)
                : _data(m._data) {} //## should not be necessary

   //## Borland bug doesn't allow access decl.
   void clear() { _data.clear(); }

   int num() const { return _data.num(); }

   int empty() const { return _data.empty(); }

   JAM_OrdSetComp<K,KeyComp> keys() const;

   int contains(const K& key) const
      { return _data.contains(JAM_KeyValue<K,V,KeyComp>(key)); }

   void enter(const K& key, const V& value);

   V& elem(const K& key)
      { JAM_Muter< JAM_OrdSet< JAM_KeyValue<K,V,KeyComp> > > i(&_data, _data.search(JAM_KeyValue<K,V,KeyComp>(key)));
        JAM_assert(!i.offEnd()); return (*i)._value; }

   const V& elem(const K& key) const
      { JAM_Iter< JAM_OrdSet< JAM_KeyValue<K,V,KeyComp> > > i(&_data, _data.search(JAM_KeyValue<K,V,KeyComp>(key)));
        JAM_assert(!i.offEnd()); return (*i)._value; }

   void remove(const K& key) {
      _data.remove(JAM_KeyValue<K,V,KeyComp>(key));
      }
   // removes all entries where keybeg<=key<=keyend
   void remove(const K& keybeg, const K& keyend)
      { _data.remove(
         JAM_KeyValue<K,V,KeyComp>(keybeg), JAM_KeyValue<K,V,KeyComp>(keyend)); }

   friend ostream& operator<<(ostream& os, const JAM_OrdKeyMapComp<K,V,KeyComp>& map);
   friend istream& operator>>(istream& os, JAM_OrdKeyMapComp<K,V,KeyComp>& map);

// JAM_Iter<Container> functionality //

   typedef JAM_OrdSet< JAM_KeyValue<K,V,KeyComp> > Set; // forward everything to set

   typedef size_t IterState;  // should be same as Set::IterState
   typedef Set::IterItemType IterItemType;
   typedef Set::IterItemTypeP IterItemTypeP;
   typedef Set::MuterItemType MuterItemType;
   typedef Set::MuterItemTypeP MuterItemTypeP;
   static int valid(const IterState& i)
      { return Set::valid(i); }
   void gofirst(IterState& i) const
      { _data.gofirst(i); }
   void golast(IterState& i) const
      { _data.golast(i); }
   void next(IterState& i) const
      { _data.next(i); }
   void prev(IterState& i) const
      { _data.prev(i); }
   IterItemType val(const IterState& i) const
      { return _data.val(i); }
   IterItemTypeP valp(const IterState& i) const
      { return _data.valp(i); }
   MuterItemType ref(const IterState& i)
      { return _data.ref(i); }
   MuterItemTypeP refp(const IterState& i)
      { return _data.refp(i); }

   void remiter(IterState& i)
   //: removes item at i
      { _data.remiter(i); }

   void insiter(IterState& /*i*/, const JAM_KeyValue<K,V,KeyComp>& /*item*/)
   //: ## shouldn't even be defined, but BC++ 3.1 requires
      { JAM_crash("Can't insiter into OrdKeyMap"); }

// JAM_Iter functions //

   //## should return JAM_Iter but can't use recursive templates in BC++ 3.1
   IterState search(const K& key) const
   //: returns an iterator positioned at item or offEnd() if item not in set
      { return _data.search(JAM_KeyValue<K,V,KeyComp>(key)); }

protected:
   JAM_OrdSet< JAM_KeyValue<K,V,KeyComp> > _data;

};


//**************************************************************************
// JAM_OrdKeyMapCompIter definition
//**************************************************************************

template<class K, class V, class KeyComp>
class JAM_OrdKeyMapCompIter : public JAM_Iter< JAM_OrdKeyMapComp<K,V,KeyComp> > {
public:
   JAM_OrdKeyMapCompIter() : JAM_Iter< JAM_OrdKeyMapComp<K,V,KeyComp> >() {}

   JAM_OrdKeyMapCompIter(const JAM_OrdKeyMapComp<K,V,KeyComp>* map)
      : JAM_Iter< JAM_OrdKeyMapComp<K,V,KeyComp> >(map) {}

   JAM_OrdKeyMapCompIter(const JAM_OrdKeyMapComp<K,V,KeyComp>* map, size_t pos)
      : JAM_Iter< JAM_OrdKeyMapComp<K,V,KeyComp> >(map, pos) {}

   const K& key() const
      { return (*(*this))._key; }

   const V& value() const
      { return (*(*this))._value; }
};


//**************************************************************************
// JAM_OrdKeyMapCompMuter definition
//**************************************************************************

template<class K, class V, class KeyComp>
class JAM_OrdKeyMapCompMuter : public JAM_Muter< JAM_OrdKeyMapComp<K,V,KeyComp> > {
public:
   JAM_OrdKeyMapCompMuter() : JAM_Muter< JAM_OrdKeyMapComp<K,V,KeyComp> >() {}

   JAM_OrdKeyMapCompMuter(JAM_OrdKeyMapComp<K,V,KeyComp>* map)
      : JAM_Muter< JAM_OrdKeyMapComp<K,V,KeyComp> >(map) {}

   const K& key() const
      { return (*(*this))._key; }

   V& value() const
      { return (*(*this))._value; }
};


//**************************************************************************
// JAM_OrdKeyMapComp functions
//**************************************************************************

template<class K, class V, class KeyComp> inline
ostream& operator<<(ostream& os, const JAM_KeyValue<K,V,KeyComp>& kv)
   { os << kv._key << " " << kv._value; return os; }
template<class K, class V, class KeyComp> inline
istream& operator>>(istream& is, JAM_KeyValue<K,V,KeyComp>& kv)
   { is >> kv._key >> kv._value; return is; }


//**************************************************************************
// JAM_OrdKeyMapComp inline member functions
//**************************************************************************



//**************************************************************************
// JAM_OrdKeyMap definition                                                   
//**************************************************************************

template<class K, class V> class JAM_OrdKeyMap : public JAM_OrdKeyMapComp<K,V,JAM_Comparator<K> > {
public:
        JAM_OrdKeyMap() {} //## should not be necessary
        JAM_OrdKeyMap(const JAM_OrdKeyMap<K,V>& m)   //## should not be necessary
                : JAM_OrdKeyMapComp<K,V,JAM_Comparator<K> >((const JAM_OrdKeyMapComp<K,V,JAM_Comparator<K> >&)m) {}
};


//**************************************************************************
// JAM_OrdKeyMapIter definition
//**************************************************************************

template<class K, class V>
class JAM_OrdKeyMapIter : public JAM_OrdKeyMapCompIter<K,V,JAM_Comparator<K> > {
public:
   JAM_OrdKeyMapIter() {}

   JAM_OrdKeyMapIter(const JAM_OrdKeyMap<K,V>* map)
      : JAM_OrdKeyMapCompIter<K,V,JAM_Comparator<K> >(map) {}

   JAM_OrdKeyMapIter(const JAM_OrdKeyMap<K,V>* map, size_t pos)
      : JAM_OrdKeyMapCompIter<K,V,JAM_Comparator<K> >(map, pos) {}
};


template<class K, class V>
class JAM_OrdKeyMapMuter : public JAM_OrdKeyMapCompMuter<K,V,JAM_Comparator<K> > {
public:
   JAM_OrdKeyMapMuter() {}

   JAM_OrdKeyMapMuter(JAM_OrdKeyMap<K,V>* map)
      : JAM_OrdKeyMapCompMuter<K,V,JAM_Comparator<K> >(map) {}

//##   JAM_OrdKeyMapMuter(const JAM_OrdKeyMap<K,V>* map, size_t pos)
//##      : JAM_OrdKeyMapCompMuter<K,V,JAM_Comparator<K> >(map, pos) {}
};


//**************************************************************************
// Non-inline functions
//**************************************************************************

#include <OrdKeyMa.cpp> //## p

// backward compatibility hack
#define JAM_Map JAM_OrdKeyMap
#define JAM_MapIter JAM_OrdKeyMapIter

#endif // JAM_OrdKeyMap_H

