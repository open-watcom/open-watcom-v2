/****************************************************************************
File: Vector.cpp

Description:   class JAM_ControlledVector<T,Mem>

Usage:

Notes:
   
History:
1992 Dec 1  Jam      fixed operator=(), _length was not always getting changed
   
****************************************************************************/     
#ifndef JAM_Vector_CPP
#define JAM_Vector_CPP

#include <Vector.h>

//************************************************************************
// JAM_ControlledVector member functions
//************************************************************************

template<class T, class Mem>
JAM_ControlledVector<T,Mem>::JAM_ControlledVector(const JAM_ControlledVector<T,Mem>& array)
{
//###   if (JAM_NewLog::activate()) JAM_NewLog::log() << "constructing vector " << (void*)this << "\n"; //###
   _allocated = array._length;
   _length = array._length;
   if (_allocated) {
      _arr = (T*)Mem::alloc(_allocated*sizeof(T));
      JAM_assert(_arr!=0);
      for (size_t i=0; i<_length; ++i)
         JAM_construct(_arr[i], array._arr[i]);
   } else {
      _arr = 0;
   }
}

template<class T, class Mem>
JAM_ControlledVector<T,Mem>::JAM_ControlledVector(size_t length)
{
//###   if (JAM_NewLog::activate()) JAM_NewLog::log() << "constructing vector " << (void*)this << "\n"; //###
   _allocated = length;
   _length = length;
   if (_allocated) {
      _arr = (T*)Mem::alloc(_allocated*sizeof(T));
      JAM_assert(_arr!=0);
      // construct each element with default ctor
      for (size_t i=0; i<_length; ++i)
         JAM_construct(_arr[i]);
   } else {
      _arr = 0;
   }
}

template<class T, class Mem>
JAM_ControlledVector<T,Mem>::JAM_ControlledVector(size_t length, size_t allocated)
{
//###   if (JAM_NewLog::activate()) JAM_NewLog::log() << "constructing vector " << (void*)this << "\n"; //###
   JAM_assert(allocated>=length);
   _allocated = allocated;
   _length = length;
   if (_allocated) {
      _arr = (T*)Mem::alloc(_allocated*sizeof(T));
      JAM_assert(_arr!=0);
      // construct each element with default ctor
      for (size_t i=0; i<_length; ++i)
         JAM_construct(_arr[i]);
   } else {
      _arr = 0;
      }
}

template<class T, class Mem>
JAM_ControlledVector<T,Mem>::JAM_ControlledVector(size_t length, size_t allocated, const T& init)
{
//###   if (JAM_NewLog::activate()) JAM_NewLog::log() << "constructing vector " << (void*)this << "\n"; //###
   JAM_assert(allocated>=length);
   _allocated = allocated;
   _length = length;
   if (_allocated) {
      _arr = (T*)Mem::alloc(_allocated*sizeof(T));
      JAM_assert(_arr!=0);
      // construct each element with copy ctor
      for (size_t i=0; i<_length; ++i)
         JAM_construct(_arr[i], init);
   } else {
      _arr = 0;
      }
}

template<class T, class Mem>
JAM_ControlledVector<T,Mem>::~JAM_ControlledVector()
{
   JAM_assert(_allocated!=size_t(-1));  //###
//###   if (JAM_NewLog::activate()) JAM_NewLog::log() << "deleting vector " << (void*)this << "\n"; //###
   if (_allocated) {
      size_t i=_length;
      while (i>0)
         JAM_destroy(_arr[--i]);
      Mem::free(_arr);
   }
   _allocated = size_t(-1);   //###
}

template<class T, class Mem>
void JAM_ControlledVector<T,Mem>::operator=(const JAM_ControlledVector<T,Mem>& array)
{
   JAM_assert(_allocated!=size_t(-1));  //###
   JAM_assert(array._allocated!=size_t(-1));  //###
   if (this==&array) return;
   if (_allocated<array._length) {        // have to reallocate a new block
      size_t i=_length;
      while (i>0)
         JAM_destroy(_arr[--i]);    // destroy each element
      Mem::free(_arr);

      _allocated = array._length;
      _length = array._length;
      if (_allocated==0) {
         _arr = 0;
      } else {
         _arr = (T*)Mem::alloc(_allocated*sizeof(T));
         JAM_assert(_arr!=0);
         // construct each element with default ctor
         for (size_t i=0; i<_length; ++i)
            JAM_construct(_arr[i]);
      }
   } else if (_length<array._length) {
      // construct elements at the end to make arrays the same size
      while (_length<array._length)
         JAM_construct(_arr[_length++]);
   } else if (_length>array._length) {
      // destroy elements at the end to make arrays the same size
      while (_length>array._length)
         JAM_destroy(_arr[--_length]);
   }
   for (size_t i=0; i<_length; ++i)
      _arr[i] = array._arr[i];
}

template<class T, class Mem>
void JAM_ControlledVector<T,Mem>::clear()
{
   JAM_assert(_allocated!=size_t(-1));  //###
   size_t i=_length;
   while (i>0)
      JAM_destroy(_arr[--i]);
   _length = 0;
}

template<class T, class Mem>
void JAM_ControlledVector<T,Mem>::resize(size_t new_length)
{
//###   dump(cout); cout << "resize(" << new_length << ")\n";
   JAM_assert(_allocated!=size_t(-1));  //###
   JAM_assert(new_length<=max());
   if (_allocated>=new_length) {    // don't have to allocate new space
      if (_length<new_length)
         while (_length<new_length)
            JAM_construct(_arr[_length++]);
      else  // _length>=new_length
         while (_length>new_length)
            JAM_destroy(_arr[--_length]);
   } else {
      size_t min = (new_length<_length) ? new_length : _length;

      // destroy each old extra element
      size_t i=_length;
      while (i>min)
         JAM_destroy(_arr[--i]);

      T* new_arr = (new_length>0) ? (T*)Mem::alloc(new_length*sizeof(T)) : 0;
      JAM_assert(new_arr!=0);

      // copy construct each new element then destroy the original
      if (min) move(new_arr, _arr, min);

      // construct each new extra element
      for (i=min; i<new_length; ++i)
         JAM_construct(new_arr[i]);

      // free space used by old array
      if (_arr) Mem::free(_arr);
      
      _arr = new_arr;
      _length = new_length;
      _allocated = new_length;
      }
}

template<class T, class Mem>
void JAM_ControlledVector<T,Mem>::resize(size_t new_length, const T& init)
{
//###   dump(cout); cout << "resize(" << new_length << ", " << "init" << ")\n";
   JAM_assert(_allocated!=size_t(-1));  //###
   JAM_assert(new_length<=max());
   if (_allocated>=new_length) {    // don't have to allocate new space
      if (_length<new_length)
         while (_length<new_length)
            JAM_construct(_arr[_length++], init);
      else  // _length>=new_length
         while (_length>new_length)
            JAM_destroy(_arr[--_length]);
   } else {
      size_t min = (new_length<_length) ? new_length : _length;

      // destroy each old extra element
      size_t i=_length;
      while (i>min)
         JAM_destroy(_arr[--i]);

      T* new_arr = (new_length>0) ? (T*)Mem::alloc(new_length*sizeof(T)) : 0;
      JAM_assert(new_arr!=0);

      // copy construct each new element then destroy the original
      if (min) move(new_arr, _arr, min);

      // construct each new extra element with initial value
      for (i=min; i<new_length; ++i)
         JAM_construct(new_arr[i], init);

      // free space used by old array
      if (_arr) Mem::free(_arr);
      
      _arr = new_arr;
      _length = new_length;
      _allocated = new_length;
      }
}

template<class T, class Mem>
void JAM_ControlledVector<T,Mem>::allocate(size_t num_elements)
{
//###   dump(cout); cout << "allocate(" << num_elements << ")\n";
   JAM_assert(_allocated!=size_t(-1));  //###
   JAM_assert(num_elements<=max());
   if (_allocated!=num_elements) {
      while (_length>num_elements)
         JAM_destroy(_arr[--_length]);
      _allocated = num_elements;
      T* new_arr = (_allocated>0) ? (T*)Mem::alloc(_allocated*sizeof(T)) : 0;
      if (_length) move(new_arr, _arr, _length);
      if (_arr) Mem::free(_arr);
      _arr = new_arr;
      }
}

template<class T, class Mem>
void JAM_ControlledVector<T,Mem>::insert(const T& item, size_t index)
{
//###   dump(cout); cout << "insert(" << "item" << ", " << index << ")\n";
   JAM_assert(_allocated!=size_t(-1));  //###
   JAM_assert(index<=_length);
   if (_allocated>_length) {
      move(&_arr[index+1], &_arr[index], _length-index, 1); // overlapping move //###here ???
      JAM_construct(_arr[index], item);
      ++_length;
   } else {    // need to allocate more space
      ++_allocated;
      T* new_arr = (T*)Mem::alloc(_allocated*sizeof(T));
      if (index>0) move(new_arr, _arr, index);   // copy 0..index-1
      JAM_construct(new_arr[index], item);
         // copy index..length
      if (_length>index) move(&new_arr[index+1], &_arr[index], _length-index);

      // free space used by old array
      if (_arr) Mem::free(_arr);
      _arr = new_arr;
      ++_length;
      }
}

template<class T, class Mem>
void JAM_ControlledVector<T,Mem>::insert(
      const JAM_ControlledVector<T,Mem>& array,
      size_t a_start, size_t a_end, size_t index )
{
//###   dump(cout); cout << "insert(" << "array" << ", " << index << ")\n";
   JAM_assert(_allocated!=size_t(-1));  //###
   JAM_assert(array._allocated!=size_t(-1));  //###
   JAM_assert(index<=_length);
   JAM_assert(a_start<=a_end);
   size_t n = a_end-a_start+1;
   if (_allocated>=_length+n) {
      move(&_arr[index+n], &_arr[index], _length-index, 1); // overlapping
      move(&_arr[index], &array._arr[a_start], n);
      _length += n;
   } else {    // need to allocate more space
      _allocated = _length+n;
      T* new_arr = (T*)Mem::alloc(_allocated*sizeof(T));
      if (index>0) move(new_arr, _arr, index);   // copy 0..index-1
      move(&new_arr[index], &array._arr[a_start], n);
         // copy index..length
      if (_length>index) move(&new_arr[index+1], &_arr[index], _length-index);

      // free space used by old array
      if (_arr) Mem::free(_arr);
      _arr = new_arr;
      _length += n;
      }
}

template<class T, class Mem>
void JAM_ControlledVector<T,Mem>::remove(size_t index)
{
//###   dump(cout); cout << "remove(" << index << ")\n";
   JAM_assert(_allocated!=size_t(-1));  //###
   JAM_assert(index<_length);
   JAM_destroy(_arr[index]);
   move(&_arr[index], &_arr[index+1], _length-index-1, 1);  // overlapping
   --_length;
}

template<class T, class Mem>
void JAM_ControlledVector<T,Mem>::remove(size_t start, size_t end)
{
//###   dump(cout); cout << "remove(" << start << ", " << end << ")\n";
   JAM_assert(_allocated!=size_t(-1));  //###
   JAM_assert(start<=end && end<_length);
   size_t n = end-start+1;
   size_t i = end+1;
   while (i>start)
      JAM_destroy(_arr[--i]);
   move(&_arr[start], &_arr[start+n], _length-start-n);  // overlapping
   _length -= n;
}

template<class T, class Mem>
void JAM_ControlledVector<T,Mem>::move(T* dst, T* src, size_t n, int overlap)
{
//###   cout << "move(" << (void*)dst << ", " << (void*)src << ", " << n << ", " << overlap << ") " << "sizeof(T)=" << sizeof(T> ) << "\n";
   // copy construct each new element and immediately destroy old
   if (overlap && dst>src) {     // then go in reverse direction
      for (size_t i=n; i>0; ) {
         --i;
         JAM_construct(dst[i], src[i]);
         JAM_destroy(src[i]);
         }
   } else {
      JAM_assert(dst!=src); //###
      for (size_t i=0; i<n; ++i) {
         JAM_construct(dst[i], src[i]);
         JAM_destroy(src[i]);
         }
   }
}

//************************************************************************
// JAM_ControlledVector functions
//************************************************************************

template<class T, class Mem>
ostream& operator<<(ostream& os, const JAM_ControlledVector<T, Mem>& array)
{
   JAM_assert(array._allocated!=size_t(-1));  //###
   size_t n = array.length();
   os << '[' << n << ']';
   for (size_t i=0; i<n; i++)
      os << ' ' << array[i];
   return os;
}

template<class T, class Mem>
istream& operator>>(istream& is, JAM_ControlledVector<T, Mem>& array)
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


#endif // JAM_Vector_CPP

