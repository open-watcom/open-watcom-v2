/****************************************************************************
File: List.h

Description:  class JAM_List<T>


Usage:

Notes:
  T must define a copy-ctor and assignment operator.
  A list should not be changed while an iterator is active over it!


History:
1991 Dec 20 Jam      created from my <generic.h> macros
1992 Feb 20 Jam      name changes, performance enhancements
1992 Feb 24 Jam      made to use JAM_assert() instead of <assert.h>
1992 Mar 20 Jam      remove stream ops as friends so not instantiated
1992 Apr 28 Jam      moved inlines inside class because of BC++ 3.0 bug
1992 Jun 16 Jam      redid stream ops as friends for parse stuff
1992 Oct 07 Jam      put in jam subdirectory
1992 Oct 07 Jam      made to use Vector instead of Array
1992 Oct 07 Jam      made to use Iter instead of specialized OrderedSetIterator
1993 Mar 30 Jam      added remove_all(item) member function

****************************************************************************/
#ifndef JAM_List_H
#define JAM_List_H

#include <iostream.h>
#include <assert.hpp>
#include <Vector.h>
#include <Iter.h>


//**************************************************************************
// JAM_List definition
//**************************************************************************

template<class T> class JAM_List {
public:
   JAM_List();

   JAM_List(const JAM_List<T>& l);  //###
   void operator=(const JAM_List<T>& l);  //###
   ~JAM_List();  //###

   void clear()
      { _data.clear(); }

// Inquiry member functions //

   template<class TT>
   friend ostream& operator<<(ostream& os, const JAM_List<TT>& list);
   template<class TT>
   friend istream& operator>>(istream& is, JAM_List<TT>& list);

   template<class TT>
   friend int operator==(const JAM_List<TT>& list1, const JAM_List<TT>& list2);
   template<class TT>
   friend int operator!=(const JAM_List<TT>& list1, const JAM_List<TT>& list2);

   int empty() const
      { return _data.length()==0; }

   size_t length() const
      { return _data.length(); }

   int contains(const T& item) const;

   const T& first() const
      { return _data.elem(0); }
   const T& last() const
      { return _data.elem(_data.length()-1); }

   JAM_List<T> rest() const;

   static JAM_List<T> of(const T& item); 
   static JAM_List<T> of(const T& item1,const T& item2); 
   static JAM_List<T> of(const T& item1,const T& item2,const T& item3); 

// Modifier member functions //

   void insert(const T& item)
   //: inserts item at beginning of list
      { insarr(0, item); }

   JAM_List& append(const T& item)
   //: appends item to end of list
      { insarr(_data.length(), item); return *this; }

   void remove_first()
      { _data.remove(0); }

   void remove_last()
      { _data.remove(length()-1); }

   JAM_List<T>& append(const JAM_List<T>& other);
   //: appends other to this list

   JAM_List<T>& operator+=(const T& item)
      { return append(item); }

   JAM_List<T>& operator+=(const JAM_List<T>& other)
      { return append(other); }

   void remove_all(const T& item);
   //: removes all occurances of item

   template<class TT>
   friend JAM_List<TT> concat(const JAM_List<TT>& list1,
                              const JAM_List<TT>& list2);
   //: returns new list made by concatenating list1 and list2

   template<class TT>
   friend JAM_List<TT> operator+(const JAM_List<TT>& list1,
                                 const JAM_List<TT>& list2);
   //: calls concat()


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

   void remiter(const IterState& i)
   //: removes item at i
      { _data.remove(i); }

   void insiter(IterState& i, const T& item)
   //: inserts item at i
      { insarr(i, item); }

// JAM_Iter functions //

   //## should return JAM_Iter but can't use recursive templates in BC++ 3.1
   IterState search(const T& item) const;
   //: returns an iterator positioned at item or offEnd() if item not in list

protected:
   JAM_Vector<T> _data;

   void insarr(size_t pos, const T& item)
   //: inserts item into list, growing if necessary
      { if (_data.allocated()==_data.length())
           _data.allocate( _data.length()<_data.max()/2 ?
                              _data.length()*2 : _data.max() );
        _data.insert(item, pos); }

   enum { ALIVE=1225, DEAD=666 } state;  //###
};


//**************************************************************************
// JAM_List functions
//**************************************************************************

template<class T> inline
int operator!=(const JAM_List<T>& list1, const JAM_List<T>& list2)
   { return !(list1==list2); }

template<class T> inline
JAM_List<T> operator+(const JAM_List<T>& list1, const JAM_List<T>& list2)
   { return concat(list1, list2); }


//**************************************************************************
// JAM_List inline member functions
//**************************************************************************

/* un-inlined and moved to .hc because of BC++ 3.0 bug */


//**************************************************************************
// JAM_Iterator inline member functions
//**************************************************************************


//**************************************************************************
// Non-inlined functions
//**************************************************************************

#ifndef DONT_INCLUDE_CPP
#include <List.cpp>
#endif


#endif // JAM_List_H
