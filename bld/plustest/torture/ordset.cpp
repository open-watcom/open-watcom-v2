/****************************************************************************
File: OrdSet.cpp

Description:  class JAM_OrdSetComp<T,Comp>
              class JAM_OrdSet<T>

Usage:

Notes:
   T must define the == operator.
   
History:
25 Dec 1991 Jam      created from my <generic.h> macros
28 Apr 1992 Jam      made JAM_List Iter funcs non-inlined because of BC++ 3.0 bug
   
****************************************************************************/     

#ifndef JAM_OrdSet_CPP
#define JAM_OrdSet_CPP

#include <OrdSet.h>

//************************************************************************
// JAM_OrdSetComp member functions
//************************************************************************

template<class T, class Comp> int JAM_OrdSetComp<T,Comp>::find(const T& item, size_t& pos) const
{  /* binary search for item, return success and pos is at correct place */
   JAM_assert(_data.allocated()!=size_t(-1));  //###
   if (num()==0) {
      pos = 0;
      return 0;
      }

   size_t left = 0;
   size_t right = num()-1;

   for (;;) {
      /*assert(left<=right); sanity check */
      pos = (right-left)/2 + left;
      if (Comp::equal(_data[pos],item))
         return 1;
      else if (Comp::lessthan(_data[pos],item)) {
         if (pos==right) {
            ++pos;
            return 0;
            }
         else
            left = pos+1;
         }
      else /* (_data[pos]>item) */ {
         if (pos==left) {
            /* pos is at correct place for item */
            return 0;
            }
         else
            right = pos-1;
         }
      }  /* forever */
}

template<class T, class Comp>
JAM_OrdSetComp<T,Comp> JAM_OrdSetComp<T,Comp>::of(const T& item)
{
   JAM_OrdSetComp<T,Comp> tmp(1); tmp.add(item); return tmp;
}

template<class T, class Comp>
JAM_OrdSetComp<T,Comp> JAM_OrdSetComp<T,Comp>::of(const T& item1, const T& item2)
{
   JAM_OrdSetComp<T,Comp> tmp(2); tmp.add(item1); tmp.add(item2); return tmp;
}

template<class T, class Comp>
JAM_OrdSetComp<T,Comp> JAM_OrdSetComp<T,Comp>::of(const T& item1, const T& item2,
                                    const T& item3)
{
   JAM_OrdSetComp<T,Comp> tmp(3);
   tmp.add(item1);
   tmp.add(item2);
   tmp.add(item3);
   return tmp;
}

template<class T, class Comp> void JAM_OrdSetComp<T,Comp>::grow()
{
   JAM_assert(_data.allocated()!=size_t(-1)); //###
   size_t n = _data.length();
   JAM_assert(n<_data.max());    // make sure room to grow
   if (n<MINIMUM_SIZE) n = MINIMUM_SIZE;
   else if (n>_data.max()/2) n = _data.max();
   else n *= 2;
   _data.allocate(n);
}

template<class T, class Comp> void JAM_OrdSetComp<T,Comp>::remove(const T& beg, const T& end)
{
   JAM_assert(!Comp::lessthan(end,beg));
   size_t begpos, endpos;
   find(beg, begpos);
   if (begpos<_data.length()) {
      if (!find(end, endpos)) {
         if (endpos==0) return;     // beg..end range is not in _data
         else --endpos;
      }
      _data.remove(begpos, endpos);    // remove elements inclusively
   }
}

template<class T, class Comp> void JAM_OrdSetComp<T,Comp>::operator+=(const JAM_OrdSetComp<T,Comp>& other)
{
   if (!other.empty())
      *this = (*this) + other;
}

template<class T, class Comp> void JAM_OrdSetComp<T,Comp>::operator-=(const JAM_OrdSetComp<T,Comp>& other)
{
   if (!other.empty())
      *this = (*this) - other;
}

template<class T, class Comp> void JAM_OrdSetComp<T,Comp>::operator*=(const JAM_OrdSetComp<T,Comp>& other)
{
   if (other.empty()) clear();
   else *this -= (*this) - other;
}


//************************************************************************
// JAM_OrdSetComp functions                                                      */
//************************************************************************

template<class T, class Comp>
JAM_OrdSetComp<T,Comp> operator+(const JAM_OrdSetComp<T,Comp>& ordset,
                            const T& item)
{
   size_t x;
   if (ordset.find(item, x))
      return ordset;
   else {   // it must be added
      JAM_OrdSetComp<T,Comp> result(ordset._data.length()+1);
      if (x>0)    // insert any of ordset's elements before 'item'
         result._data.insert(ordset._data, 0, x-1, 0);
      result._data.insert(item, x);
      if (x<ordset._data.length())
         // insert any of ordset's elements after 'item'
         result._data.insert(ordset._data, x, ordset._data.length()-1, x+1);
      return result;
      }
}

template<class T, class Comp>
JAM_OrdSetComp<T,Comp> operator-(const JAM_OrdSetComp<T,Comp>& ordset,
                            const T& item)
{
   size_t x;
   if (ordset.find(item, x)) {
      JAM_OrdSetComp<T,Comp> result(ordset._data.length()-1);
      if (x>0)    // insert any of ordset's elements before 'item'
         result._data.insert(ordset._data, 0, x-1, 0);
      if (x+1<ordset._data.length())
         // insert any of ordset's elements after 'item'
         result._data.insert(ordset._data, x+1, ordset._data.length()-1, x);
      return result;
   } else {
      return ordset;
      }
}

template<class T, class Comp>
JAM_OrdSetComp<T,Comp> operator+(const JAM_OrdSetComp<T,Comp>& ordset1,
                            const JAM_OrdSetComp<T,Comp>& ordset2)
{
   if (ordset1.empty()) {
      return ordset2;
   } else if (ordset2.empty()) {
      return ordset1;
   } else {
      size_t i1=0, i2=0;
      size_t n1=ordset1.num(), n2=ordset2.num();
      JAM_OrdSetComp<T,Comp> tmp( n1+n2 );  // assume max size

      // go through each set simultaneously
      while (i1<n1 && i2<n2) {
         if (Comp::lessthan(ordset1[i1],ordset2[i2])) { // if set1 has smaller values
            tmp.add(ordset1[i1]);
            ++i1;
         } else { // set2 has smaller or equal values
            tmp.add(ordset2[i2]);
            if (Comp::equal(ordset1[i1],ordset2[i2]))
               ++i1;
            ++i2;
         }
      }

      // add any leftover
      for (; i1<n1; ++i1)
         tmp.add(ordset1[i1]);
      for (; i2<n2; ++i2)
         tmp.add(ordset2[i2]);

      return tmp;
   }
}

template<class T, class Comp> 
JAM_OrdSetComp<T,Comp> operator-(const JAM_OrdSetComp<T,Comp>& ordset1,
                            const JAM_OrdSetComp<T,Comp>& ordset2)
{
   if (ordset1.empty() || ordset2.empty()) {
      return ordset1;   // nothing to remove from or nothing to remove
   } else {
      size_t i1=0, i2=0;
      size_t n1=ordset1.num(), n2=ordset2.num();
      JAM_OrdSetComp<T,Comp> tmp(n1);    // assume maximum possible size

      // go through each set simultaneously
      while (i1<n1 && i2<n2) {
         if (Comp::lessthan(ordset1[i1],ordset2[i2])) { // if set1 has smaller values
            tmp.add(ordset1[i1]);
            ++i1;
         } else { // set2 has smaller or equal values
            if (Comp::equal(ordset1[i1],ordset2[i2]))  // don't add duplicates
               ++i1;
            ++i2;
         }
      }

      // add any leftover
      for (; i1<n1; ++i1)
         tmp.add(ordset1[i1]);

      return tmp;
   }
}

template<class T, class Comp> 
JAM_OrdSetComp<T,Comp> operator*(const JAM_OrdSetComp<T,Comp>& ordset1,
                            const JAM_OrdSetComp<T,Comp>& ordset2)
{
   size_t i1=0, i2=0;
   size_t n1=ordset1.num(), n2=ordset2.num();
   JAM_OrdSetComp<T,Comp> tmp(n1);    // assume maximum possible size

   // go through each set simultaneously
   while (i1<n1 && i2<n2) {
      if (Comp::lessthan(ordset1[i1],ordset2[i2]))   // if set1 has smaller values
         ++i1;
      else if (Comp::lessthan(ordset2[i2],ordset1[i1])) // if set2 has smaller values
         ++i2;
      else { // set1 and set2 has duplicate values
         tmp.add(ordset1[i1]);
         ++i1; ++i2;
         }
      }

   return tmp;
}

template<class T, class Comp> 
JAM_OrdSetComp<T,Comp> operator%(const JAM_OrdSetComp<T,Comp>& ordset1,
                            const JAM_OrdSetComp<T,Comp>& ordset2)
{
   size_t i1=0, i2=0;
   size_t n1=ordset1.num(), n2=ordset2.num();
   JAM_OrdSetComp<T,Comp> tmp( n1+n2 );     // assume maximum possible size

   // go through each set simultaneously
   while (i1<n1 && i2<n2) {
      if (Comp::lessthan(ordset1[i1],ordset2[i2])) {  // if set1 has smaller values
         tmp.add(ordset1[i1]);
         ++i1;
      } else if (Comp::lessthan(ordset2[i2],ordset1[i1])) { // if set2 has smaller values
         tmp.add(ordset2[i2]);
         ++i2;
      } else { // set1 and set2 has duplicate values
         ++i1; ++i2;
         }
      }

   // add any leftover
   for (; i1<n1; ++i1)
      tmp.add(ordset1[i1]);
   for (; i2<n2; ++i2)
      tmp.add(ordset2[i2]);

   return tmp;
}

template<class T, class Comp>
int operator==(const JAM_OrdSetComp<T,Comp>& ordset1,
               const JAM_OrdSetComp<T,Comp>& ordset2)
{
   size_t n = ordset1.num();
   if (n!=ordset2.num()) return 0;
   for (size_t i=0; i<n; i++)
      if (!Comp::equal(ordset1[i],ordset2[i])) return 0;
   return 1;
}

template<class T, class Comp>
   ostream& operator<<(ostream& os, const JAM_OrdSetComp<T,Comp>& ordset)
{
   os << "{ ";
   for (int i=0; i<ordset.num(); ++i) {
      os << ordset._data[i];
      if (i<ordset.num()-1) os << ", ";
      }
   os << " }";
   return os;
}

template<class T, class Comp>
   istream& operator>>(istream& is, JAM_OrdSetComp<T,Comp>& /*ordset*/)
{
   JAM_assert(0); return is;
}

#endif // JAM_OrdSetComp_CPP

