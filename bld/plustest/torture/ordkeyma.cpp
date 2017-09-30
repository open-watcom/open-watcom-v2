/****************************************************************************
File: OrdKeyMap.cpp

Description:

Notes:

History:
****************************************************************************/     

#ifndef JAM_OrdKeyMap_CPP
#define JAM_OrdKeyMap_CPP

#include <OrdKeyMa.h> //## p

//************************************************************************
// JAM_OrdKeyMapComp member functions
//************************************************************************

template<class K, class V, class KeyComp>
JAM_OrdSetComp<K,KeyComp> JAM_OrdKeyMapComp<K,V,KeyComp>::keys() const
{
   JAM_OrdSetComp<K,KeyComp> keys;
   for (JAM_Iter< JAM_OrdSet< JAM_KeyValue<K,V,KeyComp> > > kv(&_data);
        !kv.offEnd(); ++kv)
      keys += kv()._key;
   return keys;
}

template<class K, class V, class KeyComp>
void JAM_OrdKeyMapComp<K,V,KeyComp>::enter(const K& key, const V& value)
{
   JAM_KeyValue<K,V,KeyComp> kv(key, value);
   JAM_Muter< JAM_OrdSet< JAM_KeyValue<K,V,KeyComp> > > i(&_data,_data.search(kv));
   if (i.offEnd())
      _data.enter(kv);   // enter new key-value
   else
      (*i)._value = value;   // replace value already in map
}


//**************************************************************************
// JAM_OrdKeyMapComp functions                                                     
//**************************************************************************

template<class K, class V, class KeyComp> 
   ostream& operator<<(ostream& os, const JAM_OrdKeyMapComp<K,V,KeyComp>& map)
{
   os << "[";
   JAM_OrdKeyMapCompIter<K,V,KeyComp> i(&map);
   while (i) {
      os << i.key() << " : " << i.value();
      if (++i) os << ", ";
      }
   os << "]\n";
   return os;
}

template<class K, class V, class KeyComp> 
   istream& operator>>(istream& is, JAM_OrdKeyMapComp<K,V,KeyComp>& map)
{
   is >> ws;
   JAM_assert(is.peek()=='[');
   is.get();
   is >> ws;
   while (!is.eof() && is.peek()!=']') {
      K key;
      V value;
      is >> key;

      is >> ws;
      if (is.peek()==':') { is.get(); is >> ws; }

      is >> value;

      is >> ws;
      if (is.peek()==',') { is.get(); is >> ws; }

      JAM_assert(is.good());
      map.enter(key, value);
      cout << "Read " << key << " : " << value << endl;
      }
   is.get();
   JAM_assert(is.good());
   return is;
}

#endif // JAM_OrdKeyMap_CPP

