/****************************************************************************
File: Vector.h

Description:   class JAM_ControlledVector<T,Mem>
   Like Array except reduces memory allocations by keeping an
   "allocation size".

Usage:

Notes:
   'move()' can be redefined for built-in and "moveable" types as just 
   'memmove(dst, src, n*sizeof(T));'.

Output/input is of the form "[N] t1 t2 t3 ... tN".

History:
1992 Aug 28 Jam      created from JAM_ControlledArray
1992 Dec  1 Jam      added ctor/op=() to JAM_Vector in case of BC++ bug
1993 Mar 23 Jam      corrected remiter() so that removing last element puts
                     iterator in an invalid state
   
****************************************************************************/     
#ifndef JAM_Vector_H
#define JAM_Vector_H

#include <iostream.h>
#include <stddef.h>     // for size_t
#include <assert.hpp>
#include <PlaceNew.h>
#include <Iter.h>
#include <MemDefau.h> //## lt

//************************************************************************
// JAM_ControlledVector definition
//************************************************************************

template<class T, class Mem> class JAM_ControlledVector {
public:
   JAM_ControlledVector();
   //: constructs zero element array

   JAM_ControlledVector(const JAM_ControlledVector<T,Mem>& array);
   //: copy construct each element with array

   JAM_ControlledVector(size_t length);
   //: constructs "length" element array using T's default constructor

   JAM_ControlledVector(size_t length, size_t allocated); 
   //: constructs "length" element array using T's default constructor
   //. but sets aside space for up to "allocated" elements

   JAM_ControlledVector(size_t length, size_t allocated, const T& init); 
   //: copy constructs each element with an initial value

   ~JAM_ControlledVector(); 
   //: destroy each element and free memory used

   void operator=(const JAM_ControlledVector<T,Mem>& array); 
   //: assign each element, resizing if necessary

   void resize(size_t new_length);
   //: grow or shrink to new_size elements
   //. elements 0..min(length,new_length)-1 are copy constructed
   //. elements length-1..new_length-1 are destroyed _or_
   //. elements new_length-1..length-1 are default constructed

   void resize(size_t new_length, const T& init);
   //: like resize except initialize any new elements with 'init'

   void allocate(size_t num_elements);
   //: set number of elements to set aside space for

   void clear(); 
   //: destroys each element, making length()==0; does *not* free memory

   void enter(size_t index, const T& item); 
   //: assigns item to index position (ie, array[index]=item)

   T& operator[](size_t index); 
   //: returns non-const reference to index'th element

   const T& elem(size_t index) const;
   //: returns const reference to index'th element

   const T& operator[](size_t index) const;
   //: returns const reference to index'th element

   void append(const T& item);
   //: resizes array to length()+1 and puts item at last length()-1

   void insert(const T& item, size_t index = 0);
   //: puts item at index, moving elements or resizing vector if necessary

   void insert(const JAM_ControlledVector<T,Mem>& array,
               size_t a_start, size_t a_end, size_t index = 0);
   //: puts item array[a_start]...array[a_end] into array at index,
   //. moving elements and resizing vector if necessary

   void remove(size_t index); 
   //: deletes item at index, moving elements if necessary

   void remove(size_t start, size_t end);
   //: deletes item from start to end (inclusive), moving elements
   //. if necessary

   size_t length() const; 
   //: returns current number of elements in array

   size_t allocated() const; 
   //: returns number of elements allocated for

   static size_t max();
   //: returns maximum length (due to size_t limit) of array

   void dump(ostream& os) const
   //: prints information useful for debugging
//      { os << "this=" << (const void*)this << " _arr=" << (void*)_arr << " _length=" << _length << " _allocated=" << _allocat> ed << endl; }
      { os << "this=" << (void*)this;
   os << " _arr=" << (void*)_arr;
   os << " _length=" << _length << " _allocated=" << _allocated;
   os << endl; }


// JAM_Iter<Container> functionality //

   typedef size_t IterState;
   //: position indicator used by JAM_Iter<Container>

   typedef const T& IterItemType;
   //: type JAM_Iter<Container> operator*() returns

   typedef const T* IterItemTypeP;
   //: type JAM_Iter<Container> operator->() returns

   typedef T& MuterItemType;
   //: type JAM_Muter<Container> operator*() returns

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

   const T* valp(const IterState& i) const
   //: returns IterItemTypeP indicated by 'i'
      { return &elem(i); }

   T& ref(const IterState& i)
   //: returns MuterItemType indicated by 'i'
      { return (*this)[i]; }

   T* refp(const IterState& i)
   //: returns MuterItemTypeP indicated by 'i'
      { return &(*this)[i]; }

   void remiter(IterState& i)
   //: removes item at i
      { remove(i);
        if (i==_length) i=size_t(-1);  // put offEnd if removing last item
      }

   void insiter(IterState& i, const T& item)
   //: inserts item at i
      { insert(item, i); }

protected:
   T* _arr;       // each element constructed and destroyed explicitly
   size_t _length;  // current number of elements
   size_t _allocated;   // number of elements allocated for

   static void move(T* dst, T* src, size_t n, int overlap=0);
   //: copy src to raw memory dst then destroy src elements
   //. overlap should be 1 iff dst and src are pointers into same array
};


//**************************************************************************
// JAM_ControlledVector functions
//**************************************************************************

template<class T, class Mem>
ostream& operator<<(ostream& os, const JAM_ControlledVector<T, Mem>& array);

template<class T, class Mem>
istream& operator>>(istream& is, JAM_ControlledVector<T, Mem>& array);


//**************************************************************************
// JAM_ControlledVector inline member functions
//**************************************************************************

template<class T, class Mem>
inline JAM_ControlledVector<T,Mem>::JAM_ControlledVector()
   : _allocated(0), _length(0), _arr(0)
{
//###   if (JAM_NewLog::activate()) JAM_NewLog::log() << "constructing vector " << (void*)this << "\n"; //###
}

template<class T, class Mem>
inline const T& JAM_ControlledVector<T,Mem>::elem(size_t index) const
{
   JAM_assert(index<_length); return _arr[index];
}

template<class T, class Mem>
inline void JAM_ControlledVector<T,Mem>::enter(size_t index, const T& item)
{
   JAM_assert(index<_length); _arr[index] = item;
}

template<class T, class Mem>
inline const T& JAM_ControlledVector<T,Mem>::operator[](size_t index) const
{
   JAM_assert(index<_length); return _arr[index];
}

template<class T, class Mem>
inline T& JAM_ControlledVector<T,Mem>::operator[](size_t index)
{
   JAM_assert(index<_length); return _arr[index];
}

template<class T, class Mem>
inline size_t JAM_ControlledVector<T,Mem>::length() const
{
   return _length;
}

template<class T, class Mem>
inline size_t JAM_ControlledVector<T,Mem>::allocated() const
{
   return _allocated;
}

template<class T, class Mem>
size_t JAM_ControlledVector<T,Mem>::max()
{
   return size_t(-1) / sizeof(T);
}

template<class T, class Mem>
void JAM_ControlledVector<T,Mem>::append(const T& item)
{
   if (_allocated==_length) resize(_length+1, item);
   else JAM_construct(_arr[_length++], item);
}

//**************************************************************************
// Non-inlined functions
//**************************************************************************

#ifndef DONT_INCLUDE_CPP
#include <Vector.cpp>
#endif


//************************************************************************
// JAM_Vector definition
//************************************************************************

template<class T>
class JAM_Vector : public JAM_ControlledVector<T,JAM_MemDefault> {
public:
   JAM_Vector()
   //: constructs zero element array
      : JAM_ControlledVector<T,JAM_MemDefault>() {}

   JAM_Vector(size_t length)
   //: constructs "length" element array using T's default constructor
      : JAM_ControlledVector<T,JAM_MemDefault>(length) {}

   JAM_Vector(size_t length, size_t allocated)
   //: copy constructs each element with an initial value
      : JAM_ControlledVector<T,JAM_MemDefault>(length, allocated) {}

   JAM_Vector(size_t length, size_t allocated, const T& init)
   //: copy constructs each element with an initial value
      : JAM_ControlledVector<T,JAM_MemDefault>(length, allocated, init) {}

   JAM_Vector(const JAM_ControlledVector<T,JAM_MemDefault>& other)
      : JAM_ControlledVector<T,JAM_MemDefault>(other) {}
   void operator=(const JAM_ControlledVector<T,JAM_MemDefault>& other)
      { JAM_ControlledVector<T,JAM_MemDefault>::operator=(other); }

   //### bug might require the following two be defined
   JAM_Vector(const JAM_Vector& other)
      : JAM_ControlledVector<T,JAM_MemDefault>(other) {}
   void operator=(const JAM_Vector& other)
      { JAM_ControlledVector<T,JAM_MemDefault>::operator=(other); }

   // inherits all public members of JAM_ControlledVector
};

#endif // JAM_Vector_H


