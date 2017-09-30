/****************************************************************************
File: Shared.h

Description:  class JAM_Shared<T>
              class JAM_SharedRep<T>

   A Shared<T> is a reference-like object to a T that copies-on-write
   instead of copying the T each time one is assigned to or contructed
   from another T.  Operators '*' and '->' give a reference to a
   const T.  To modify the T, use operator '()' which returns a non-const
   reference to a T.  It makes its own copy if others are also using it.

Usage:
   class String {
      JAM_Shared< Array<char> > _s;
   public:
      String() : _s() { _s.resize(1); _s[0] = '\0'; }
      String(const String& str) : _s(str._s) {}
      String& operator=(const String& str) { _s = str._s; }
      String& operator=(const char* s) { _s().resize(strlen(s)+1); ... }
      char operator[](int i) const { return (*_s)[i]; }
      char& operator[](int i) { return _s()[i]; }
   };

Notes:
   You can derive privately from Shared and implement the member
   functions of T so that you do not have to use the pointer-like
   syntax.

History:
30 Sep 1992 Jam      created from JAM_SmartPtr
   
****************************************************************************/     

#ifndef JAM_Shared_H
#define JAM_Shared_H

#include <assert.hpp>

//***************************************************************************
// JAM_Shared definition
//***************************************************************************

template<class T> struct JAM_SharedRep {
   T data;
   unsigned int refs;
   JAM_SharedRep() {}
   JAM_SharedRep(const T& init) : data(init) {}
};


template<class T> class JAM_Shared {
public:
   JAM_Shared()
      { _ptr = new JAM_SharedRep<T>; _ptr->refs = 1; }

   JAM_Shared(const T& init)
      { _ptr = new JAM_SharedRep<T>(init); _ptr->refs = 1; }

   JAM_Shared(const JAM_Shared<T>& other)
      { _ptr = other._ptr; _ptr->refs++; }

   ~JAM_Shared()
      { if (--_ptr->refs == 0) delete _ptr; }

   void operator=(const JAM_Shared<T>& other) {
      other._ptr->refs++;
      if (--_ptr->refs == 0) delete _ptr;
      _ptr = other._ptr;
      }

   void operator=(const T& data); 

   const T& operator->() const
      { return _ptr->data; }
   const T& operator*() const
      { return _ptr->data; }

   operator const T&() const
      { return _ptr->data; }

   T& operator()()
      { if (_ptr->refs>1) make_own(); return _ptr->data; }

protected:
   inline void make_own();

   JAM_SharedRep<T>* _ptr;
};


//***************************************************************************
// JAM_Shared inlines
//***************************************************************************

template<class T>
inline void JAM_Shared<T>::make_own()
{
   --_ptr->refs;
   _ptr = new JAM_SharedRep<T>(_ptr->data);
   _ptr->refs = 1;
}

template<class T>
inline void JAM_Shared<T>::operator=(const T& data)
{
   if (_ptr->refs==1) {
      _ptr->data = data;
   } else {
      JAM_SharedRep<T>* tmp = new JAM_SharedRep<T>(data);
      tmp->refs = 1;
      if (--_ptr->refs == 0) delete _ptr;
      _ptr = tmp;
      }
}

#endif // JAM_Shared_H

