/****************************************************************************
File: CtrldArray.h

Description:   class JAM_ControlledArray<T,Mem>

Usage:

Notes:
move() can be redefined for built-in and "moveable" types as just 
   memmove(dst, src, n*sizeof(T));

Output/input is of the form "[N] t1 t2 t3 ... tN".

History:
28 Aug 1992 Jam      created from JAM_Array
   
****************************************************************************/     
#ifndef JAM_ControlledArray_H
#define JAM_ControlledArray_H

#include <iostream.h>
#include <stddef.h>     // for size_t
#include <assert.hpp>
#include <PlaceNew.h>
#include <Iter.h>

//************************************************************************
// JAM_ControlledArray definition
//************************************************************************

template<class T, class Mem> class JAM_ControlledArray {
public:
   JAM_ControlledArray();
   //: constructs zero element array

   JAM_ControlledArray(const JAM_ControlledArray<T,Mem>& array);
   //: copy construct each element with array

   JAM_ControlledArray(size_t length);
   //: constructs "length" element array using T's default constructor

   JAM_ControlledArray(size_t length, const T& init); 
   //: copy constructs each element with an initial value

   ~JAM_ControlledArray(); 
   //: destroy each element and free memory used

   void operator=(const JAM_ControlledArray<T,Mem>& array); 
   //: assign each element, resizing if necessary

   void resize(size_t new_length);
   //: grow or shrink to new_size elements
   //. elements 0..min(length,new_length)-1 are copy constructed
   //. elements length-1..new_length-1 are destroyed _or_
   //. elements new_length-1..length-1 are default constructed

   void resize(size_t new_length, const T& init);
   //: like resize except initialize any new elements with 'init'

   void clear(); 
   //: destroy each element and free memory used; makes length()==0

   void enter(size_t index, const T& item); 
   //: assigns item to index position (ie, array[index]=item)

   T& operator[](size_t index); 
   //: returns non-const reference to index'th element

   const T& elem(size_t index) const;
   //: returns const reference to index'th element

   const T& operator[](size_t index) const; 
   //: returns const reference to index'th element

   size_t length() const; 
   //: returns current number of elements in array

   static size_t max();
   //: returns maximum length (due to size_t limit) of array


// JAM_Iter<Container> functionality //

   typedef size_t IterState;
   //: position indicator used by JAM_Iter<Container>

   typedef const T& IterItemType;
   //: type JAM_Iter<Container> can convert to

   typedef const T* IterItemTypeP;
   //: type JAM_Iter<Container> operator->() returns

   typedef T& MuterItemType;
   //: type JAM_Muter<Container> can convert to

   typedef T* MuterItemTypeP;
   //: type JAM_Muter<Container> operator->() returns

   static int valid(const IterState& i)
   //: returns whether 'i' is offEnd
      { return i != size_t(-1); }

   void gofirst(IterState& i) const
   //: puts 'i' at first item or offEnd, returns false if length is 0
      { if (length()) i = 0; else i = size_t(-1); }
   void golast(IterState& i) const
   //: puts 'i' at last item or offEnd, returns false if length is 0
      { if (length()) i = length()-1; else i = size_t(-1); }

   void next(IterState& i) const
   //: puts 'i' at next item or offEnd if already at last item
   //: if 'i' already offEnd, puts at first; returns false if offEnd
      { if (length()==0 || i==length()-1) i = size_t(-1);
        else if (i==size_t(-1)) i = 0;
        else ++i; }
   void prev(IterState& i) const
   //: puts 'i' at prev item or offEnd if already at first item
   //: if 'i' already offEnd, puts at first; returns false if offEnd
      { if (length()==0 || i==0) i = size_t(-1);
        else if (i==size_t(-1)) i = length()-1;
        else --i; }

   const T& val(const IterState& i) const
   //: returns IterItemType indicated by 'i'
      { return elem(i); }

   T& ref(const IterState& i)
   //: returns IterItemType indicated by 'i'
      { return (*this)[i]; }

   T* refp(const IterState& i)
   //: returns MuterItemTypeP indicated by 'i'
      { return &(*this)[i]; }

   void remiter(const IterState& i);
   //: removes item at i

   void insiter(IterState& i, const T& item);
   //: inserts item at i

protected:
   T* _arr;       // each element constructed and destroyed explicitly
   size_t _length;  // current number of elements

   static void move(T* dst, T* src, size_t n); 
   //: copy src to raw memory dst then destroy src elements
};


//**************************************************************************
// JAM_ControlledArray functions
//**************************************************************************

template<class T, class Mem>
ostream& operator<<(ostream& os, const JAM_ControlledArray<T, Mem>& array);

template<class T, class Mem>
istream& operator>>(istream& is, JAM_ControlledArray<T, Mem>& array);


//**************************************************************************
// JAM_ControlledArray inline member functions
//**************************************************************************

template<class T, class Mem>
inline JAM_ControlledArray<T,Mem>::JAM_ControlledArray()
   : _length(0), _arr(0)
{
}

template<class T, class Mem>
inline const T& JAM_ControlledArray<T,Mem>::elem(size_t index) const
{ JAM_assert(index<_length); return _arr[index];
}

template<class T, class Mem>
inline void JAM_ControlledArray<T,Mem>::enter(size_t index, const T& item)
{ JAM_assert(index<_length); _arr[index] = item;
}

template<class T, class Mem>
inline const T& JAM_ControlledArray<T,Mem>::operator[](size_t index) const
{ JAM_assert(index<_length); return _arr[index];
}

template<class T, class Mem>
inline T& JAM_ControlledArray<T,Mem>::operator[](size_t index)
{ JAM_assert(index<_length); return _arr[index];
}

template<class T, class Mem>
inline size_t JAM_ControlledArray<T,Mem>::length() const
{ return _length;
}

template<class T, class Mem>
inline size_t JAM_ControlledArray<T,Mem>::max()
{ return size_t(-1) / sizeof(T);
}


//**************************************************************************
// Non-inlined functions
//**************************************************************************

#ifndef DONT_INCLUDE_CPP
#include <CtrldArr.cpp> //## ay
#endif

#endif // JAM_ControlledArray_H
