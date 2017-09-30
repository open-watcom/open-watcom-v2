/****************************************************************************
File: OrdSet.h

Description:  class JAM_OrdSetComp<T,Comp>
              class JAM_OrdSet<T>

Usage:

Notes:
   T must define a copy-ctor, assignment operator, equality operator,
   and the less than comparison operator '<'.
   A set should not be changed while an iterator is active over it!
   Can probably remove function remarr().

History:
1991 Dec 20 Jam      created from my <generic.h> macros
1992 Feb 20 Jam      uncoupled from JAM_List, major performance enhancing
1992 Mar 20 Jam      renamed `remove(int idx)' `remarr(int idx)' because
                     of ambiguity when T==int
1992 Mar 20 Jam      removed stream operators because so not always instantiated
1992 Apr 28 Jam      moved inlines inside class because of BC++ 3.0 bug
1992 Jun 16 Jam      redid stream ops as friends for parse stuff
1992 Jul 09 Kevin    added non-const operator[]()
1992 Jul 09 Kevin    added pos() member function to Iterator [reconsider--Jam]
1992 Oct 05 Jam      put in jam subdirectory
1992 Oct 06 Jam      made to use Vector instead of Array
1992 Oct 06 Jam      made to use Iter instead of specialized OrderedSetIterator
1992 Oct 07 Jam      renamed from JAM_OrderedSet to JAM_OrdSet
1992 Oct 07 Jam      made JAM_OrdSetComp version which takes a Comparator
1993 Mar 23 Jam      remiter() takes a non-const state and calls Vec::remiter
1993 Mar 30 Jam      added friend i/o ops to OrdSet because of ambiguity errors
****************************************************************************/

#ifndef JAM_OrdSet_H
#define JAM_OrdSet_H

#include <iostream.h>
#include <assert.hpp>
#include <Vector.h>
#include <Iter.h>
#include <Comparat.h> //## or

//**************************************************************************
// JAM_OrdSetComp definition
//**************************************************************************

template<class T, class Comp> class JAM_OrdSetComp {
public:
   JAM_OrdSetComp() {}

   JAM_OrdSetComp(const JAM_OrdSetComp<T,Comp>& ordset)
      : _data(ordset._data) {}

   ~JAM_OrdSetComp() {}

   void operator=(const JAM_OrdSetComp<T,Comp>& ordset)
      { _data = ordset._data; }

   void clear() { _data.clear(); }

// Inquiry member functions //

   friend ostream& operator<<(ostream& os, const JAM_OrdSetComp<T,Comp>& ordset);
   friend istream& operator>>(istream& is, JAM_OrdSetComp<T,Comp>& ordset);

   size_t num() const { return _data.length(); }

   int empty() const { return num()==0; }

   friend int operator==(const JAM_OrdSetComp<T,Comp>& ordset1,
                         const JAM_OrdSetComp<T,Comp>& ordset2);

   int contains(const T& item) const
      { size_t dummy; return find(item, dummy); }


// Modifier member functions //

   void enter(const T& item)
      { size_t pos; if (!find(item, pos)) insarr(pos, item); }

   void remove(const T& item)
      { size_t pos; if (find(item, pos)) _data.remove(pos); }

   // removes all elements for which beg<=elem<=end
   void remove(const T& beg, const T& end);

   void operator+=(const T& item)
      { enter(item); }

   void operator-=(const T& item)
      { remove(item); }

   void operator+=(const JAM_OrdSetComp<T,Comp>& ordset);

   void operator-=(const JAM_OrdSetComp<T,Comp>& ordset);

   // remove any elements that are not also in ordset: this -= this-ordset
   void operator*=(const JAM_OrdSetComp<T,Comp>& ordset);

   friend JAM_OrdSetComp<T,Comp> operator+(const JAM_OrdSetComp<T,Comp>& ordset,
                                      const T& item);

   friend JAM_OrdSetComp<T,Comp> operator-(const JAM_OrdSetComp<T,Comp>& ordset,
                                      const T& item);

   friend JAM_OrdSetComp<T,Comp> operator+(const JAM_OrdSetComp<T,Comp>& ordset1,
                                      const JAM_OrdSetComp<T,Comp>& ordset2);

   friend JAM_OrdSetComp<T,Comp> operator-(const JAM_OrdSetComp<T,Comp>& ordset1,
                                      const JAM_OrdSetComp<T,Comp>& ordset2);

   // intersection: ordset1-(ordset1-ordset2) + ordset2-(ordset2-ordset1)
   friend JAM_OrdSetComp<T,Comp> operator*(const JAM_OrdSetComp<T,Comp>& ordset1,
                                      const JAM_OrdSetComp<T,Comp>& ordset2);

   // non-intersection: (ordset1-ordset2) + (ordset2-ordset1)
   friend JAM_OrdSetComp<T,Comp> operator%(const JAM_OrdSetComp<T,Comp>& ordset1,
                                      const JAM_OrdSetComp<T,Comp>& ordset2);

// Easier set creation functions //

   static JAM_OrdSetComp<T,Comp> of(const T& item); 
   static JAM_OrdSetComp<T,Comp> of(const T& item1,const T& item2); 
   static JAM_OrdSetComp<T,Comp> of(const T& item1,const T& item2,const T& item3);


// JAM_Iter<Container> functionality //

   typedef JAM_Vector<T> Vec;    // forward everything to vector

   typedef size_t IterState;  // should be same as Vec::IterState
   typedef Vec::IterItemType IterItemType;
   typedef Vec::IterItemTypeP IterItemTypeP;
   typedef Vec::MuterItemType MuterItemType;
   typedef Vec::MuterItemTypeP MuterItemTypeP;
   static int valid(const IterState& i)
      { return Vec::valid(i); }
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

   void insiter(IterState& /*i*/, const T& /*item*/)
   //: ## shouldn't even be defined, but BC++ 3.1 requires
      { JAM_crash("Can't insiter into OrdSet"); }

// JAM_Iter functions //

   //## should return JAM_Iter but can't use recursive templates in BC++ 3.1
   IterState search(const T& item) const
   //: returns an iterator positioned at item or offEnd() if item not in set
      { size_t pos; if (find(item, pos)) return pos; else return size_t(-1); }

protected:
   enum { MINIMUM_SIZE = 8 };    // smallest worthwhile array size
public: //### RULES.CPP said _data not accessible in operator<<
   JAM_Vector<T> _data;

   JAM_OrdSetComp(size_t size)
      : _data(0, (size<MINIMUM_SIZE) ? MINIMUM_SIZE : size) {}

   T& operator[](size_t i)
      { return _data[i]; }

   const T& operator[](size_t i) const
      { return _data[i]; }

   void add(const T& item)
   //: does not check size of array, use when know array doesn't need to grow
      { _data.append(item); }

   void growadd(const T& item)
   //: appends to array, growing if necessary
      { if (_data.allocated()==_data.length()) grow();
        add(item); }

   void insarr(size_t i, const T& item)
   //: inserts item into array, growing if necessary
      { if (_data.length()==_data.allocated()) grow();
        _data.insert(item, i); }

   void remarr(size_t i)
   //: removes item from array, counterpart to insarr()
      { _data.remove(i); }

   void grow();
   //: doubles size of array

   int find(const T& item, size_t& pos) const;
   //: returns TRUE if item in set
   //. pos is set to location of item or location where item should be put
};


//**************************************************************************
// JAM_OrdSet functions                                                      
//**************************************************************************


//**************************************************************************
// JAM_OrdSetComp inline member functions
//**************************************************************************

/* un-inlined and moved to .hc because of BC++ 3.0 bug */


//**************************************************************************
// JAM_OrdSet definition                                            
//**************************************************************************

template<class T>
class JAM_OrdSet : public JAM_OrdSetComp<T, JAM_Comparator<T> > {
   typedef JAM_OrdSetComp<T, JAM_Comparator<T> > inherit;
public:
   JAM_OrdSet() : JAM_OrdSetComp<T, JAM_Comparator<T> >() {}
   JAM_OrdSet(const JAM_OrdSetComp<T, JAM_Comparator<T> >& other)
      : JAM_OrdSetComp<T, JAM_Comparator<T> >(other) {}
   void operator=(const JAM_OrdSetComp<T, JAM_Comparator<T> >& other)
      { inherit::operator=(other); }

   //## BC++ 3.1 bug might require the following
   JAM_OrdSet(const JAM_OrdSet& other)
      : JAM_OrdSetComp<T, JAM_Comparator<T> >(other) {}
   void operator=(const JAM_OrdSet& other)
      { inherit::operator=(other); }
};


//**************************************************************************
// Non-inline functions
//**************************************************************************

#ifndef DONT_INCLUDE_CPP
#include <OrdSet.cpp>
#endif

// backward compatibility hack
#define JAM_OrderedSet JAM_OrdSet
//#define JAM_OrderedSetIterator 

#endif // JAM_OrdSet_H

