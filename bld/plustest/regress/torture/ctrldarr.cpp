/****************************************************************************
File: CtrldArray.cpp

Description:   class JAM_ControlledArray<T,Mem>

Usage:

Notes:
   
History:
   
****************************************************************************/     
#ifndef JAM_ControlledArray_CPP
#define JAM_ControlledArray_CPP

#include <CtrldArr.h> //## ay

//************************************************************************
// JAM_ControlledArray member functions
//************************************************************************

template<class T, class Mem>
JAM_ControlledArray<T,Mem>::JAM_ControlledArray(const JAM_ControlledArray<T,Mem>& array)
{
   _length = array._length;
   if (_length>0) {
      _arr = (T*)Mem::alloc(_length*sizeof(T));
      JAM_assert(_arr!=0);
      for (size_t i=0; i<_length; ++i)
         JAM_construct(_arr[i], array._arr[i]);
   } else {
      _arr = 0;
   }
}

template<class T, class Mem>
JAM_ControlledArray<T,Mem>::JAM_ControlledArray(size_t length)
{
   _length = length;
   if (_length) {
      _arr = (T*)Mem::alloc(_length*sizeof(T));
      JAM_assert(_arr!=0);
      // construct each element with default ctor
      for (size_t i=0; i<_length; ++i)
         JAM_construct(_arr[i]);
   } else {
      _arr = 0;
      }
}

template<class T, class Mem>
JAM_ControlledArray<T,Mem>::JAM_ControlledArray(size_t length, const T& init)
{
   _length = length;
   if (_length) {
      _arr = (T*)Mem::alloc(_length*sizeof(T));
      JAM_assert(_arr!=0);
      // construct each element with copy ctor
      for (size_t i=0; i<_length; ++i)
         JAM_construct(_arr[i], init);
   } else {
      _arr = 0;
      }
}

template<class T, class Mem>
JAM_ControlledArray<T,Mem>::~JAM_ControlledArray()
{
   size_t i=_length;
   while (i>0)
      JAM_destroy(_arr[--i]);
   Mem::free(_arr);
}

template<class T, class Mem>
void JAM_ControlledArray<T,Mem>::operator=(const JAM_ControlledArray<T,Mem>& array)
{
   if (this==&array) return;
   if (_length!=array._length) {        // have to reallocate a new block
      size_t i=_length;
      while (i>0)
         JAM_destroy(_arr[--i]);    // destroy each element
      Mem::free(_arr);

      _length = array._length;
      if (_length==0) {
         _arr = 0;
      } else {
         _arr = (T*)Mem::alloc(_length*sizeof(T));
         JAM_assert(_arr!=0);
         // construct each element with default ctor
         for (size_t i=0; i<_length; ++i)
            JAM_construct(_arr[i]);
      }
   }
   for (size_t i=0; i<_length; ++i)
      _arr[i] = array._arr[i];
}

template<class T, class Mem>
void JAM_ControlledArray<T,Mem>::clear()
{
   size_t i=_length;
   while (i>0)
      JAM_destroy(_arr[--i]);
   Mem::free(_arr);
   _arr = 0; _length = 0;
}

template<class T, class Mem>
void JAM_ControlledArray<T,Mem>::resize(size_t new_length)
{
   JAM_assert(new_length<=max());
   if (new_length==0) {
      clear();
   } else {
      size_t min = (new_length<_length) ? new_length : _length;

      // destroy each old extra element
      size_t i=_length;
      while (i>min)
         JAM_destroy(_arr[--i]);

      T* new_arr = (T*)Mem::alloc(new_length*sizeof(T));
      JAM_assert(new_arr!=0);

      // copy construct each new element then destroy the original
      move(new_arr, _arr, min);

      // construct each new extra element
      for (i=min; i<new_length; ++i)
         JAM_construct(new_arr[i]);

      // free space used by old array
      Mem::free(_arr);
      
      _arr = new_arr;
      _length = new_length;
      }
}

template<class T, class Mem>
void JAM_ControlledArray<T,Mem>::resize(size_t new_length, const T& init)
{
   JAM_assert(new_length<=max());
   if (new_length==0) {
      clear();
   } else {
      size_t min = (new_length<_length) ? new_length : _length;

      // destroy each old extra element
      size_t i=_length;
      while (i>min)
         JAM_destroy(_arr[--i]);

      T* new_arr = (T*)Mem::alloc(new_length*sizeof(T));
      JAM_assert(new_arr!=0);

      // copy construct each new element then destroy the original
      move(new_arr, _arr, min);

      // construct each new extra element
      for (i=min; i<new_length; ++i)
         JAM_construct(new_arr[i], init);

      // free space used by old array
      Mem::free(_arr);
      
      _arr = new_arr;
      _length = new_length;
      }
}

template<class T, class Mem>
void JAM_ControlledArray<T,Mem>::move(T* dst, T* src, size_t n)
{
   // copy construct each new element and immediately destroy old
   for (size_t i=0; i<n; ++i) {
      JAM_construct(dst[i], src[i]);
      JAM_destroy(src[i]);
      }
}


//************************************************************************
// JAM_ControlledArray functions
//************************************************************************

template<class T, class Mem>
ostream& operator<<(ostream& os, const JAM_ControlledArray<T, Mem>& array)
{
   size_t n = array.length();
   os << '[' << n << ']';
   for (size_t i=0; i<n; i++)
      os << ' ' << array[i];
   return os;
}

template<class T, class Mem>
istream& operator>>(istream& is, JAM_ControlledArray<T, Mem>& array)
{
   is >> ws;
   if (is.peek()=='[') {
      is.get();
      size_t n;
      char c = 0;
      is >> n >> c;
      if (c==']') {
         array.clear();
         array.resize(n);
         for (size_t i=0; i<n; i++)
            is >> array[i];
      } else {     // array length not specified properly
         is.clear(ios::badbit);
      }
   } else {    // length not specified
      is.clear(ios::badbit);
   }
   return is;
}


#endif // JAM_ControlledArray_CPP

