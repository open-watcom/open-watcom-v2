//
// Copyright (C) 1992 General Electric Company.
//
// Permission is granted to any individual or institution to use, copy, modify,
// and distribute this software, provided that this complete copyright and
// permission notice is maintained, intact, in all copies and supporting
// documentation.
//
// General Electric Company provides this software "as is" without
// express or implied warranty.
//
// Created: VDN 03/25/92 -- Initial design
// Updated: JAM 08/19/92 -- modernized template syntax, remove macro hacks
// Updated: JAM 08/19/92 -- removed Type##H(andle),P(ointer),R(eference)
//                          typedefs -- not possible with modern templates
//
// Handle<Type> works with Shared mixin on the object to provide 
// a very simple scheme of memory management with reference count.
// Handles to objects are stored in containers or other data structures,
// instead of pointers to objects, if these objects are shared.
// If your design of the object hierarchy has layers, from the root O1
// to the leaves On, parent node Oi should store Oj, or a handle to Oj,
// for automatic deletion of all the nodes in the tree when
// the root O1 is deleted. In essence, you design an acyclic graph
// for your hierarchy. Nodes can be shared with reference count.
// Backward pointers that create cycles in your network should be broken
// up by storing pointers instead of handles. Note that pointers stored in 
// COOL containers are not deleted, while objects are.
//
// Be aware: Empty handles, just as NULL pointers, can create segmentation 
// faults.
//
// Reference:
// 1. Stroupstrup(1991) C++ programming language, 2nd ed, section 13.9.
// 2. Coplien(1992) Advanced C++ programming techniques and idioms.


#ifndef HANDLEH
#define HANDLEH

#ifndef MISCH
#include <cool/misc.h>                          // for Boolean
#endif

#ifndef SHAREDH
#include <cool/Shared.h>
#endif

template <class Type> 
class CoolHandle {
public:
  /*inline##*/ CoolHandle();                    // Empty constructor
  /*inline##*/ CoolHandle(Type* optr);          // construct hdl from ptr
  /*inline##*/ CoolHandle(Type& obj);           // construct hdl from obj
  /*inline##*/ CoolHandle(const CoolHandle<Type>& h); // copy constructor
  
  /*inline##*/ ~CoolHandle();                   // destructor
  
  /*inline##*/ Type* operator-> ();                     // dereference
  
  /*inline##*/ CoolHandle<Type>& operator= (Type* optr); // assign to another ptr
  /*inline##*/ CoolHandle<Type>& operator= (Type& obj);  // assign to obj reference
  /*inline##*/ CoolHandle<Type>& operator= (const CoolHandle<Type>& h); // to hdl
  
//##  /*inline##*/ friend Type* ptr (CoolHandle<Type>&);        // convert to ptr
//##  /*inline##*/ friend Type& ref (CoolHandle<Type>&);        // convert to reference
//##  /*inline##*/ friend CoolHandle<Type> hdl (Type* optr=NULL); // convert to hdl
  
  /*inline##*/ operator Type*();                        // automatic conversion to ptr
  /*inline##*/ operator Type&();                        // automatic conversion to ref

#define DECLARE_HANDLE_OPERATOR(op)                                           \
  /*inline##*/ friend Boolean operator op (const CoolHandle<Type>&,                   \
                                     const CoolHandle<Type>&);
//##  DECLARE_HANDLE_OPERATOR(==)
//##  DECLARE_HANDLE_OPERATOR(!=)
//##  DECLARE_HANDLE_OPERATOR(<=)
#undef DECLARE_HANDLE_OPERATOR

//##  /*inline##*/ friend ostream& operator<< (ostream&, const CoolHandle<Type>&);
//##  /*inline##*/ friend ostream& operator<< (ostream&, const CoolHandle<Type>*);

//##private:  BC++ 3.1 doesn't like those friend functions declared in class
public:
  Type* rep;                                    // pointer to shared obj

  /*inline##*/ void null_pointer_error (const char* fcn, const char* type);
};

// Handle() -- Empty constructor
// Input:             ptr
// Output:            None

template<class Type>
inline CoolHandle<Type>::CoolHandle()
: rep(NULL) {}                                  // pointer is NULL

// Handle() -- Default constructor from a pointer to object, inc count.
// Input:             ptr
// Output:            None

template<class Type>
inline CoolHandle<Type>::CoolHandle(Type* optr) 
: rep(optr) {
  CoolShared::reference(this->rep);             // inc count on shared obj
}

// Handle() -- Constructor from an object, inc count.
// Input:             reference
// Output:            None

template<class Type>
inline CoolHandle<Type>::CoolHandle(Type& obj) 
: rep(&obj) {
  obj.reference();                              // inc count on shared obj
}

// Handle (handle&) -- Copy constructor from another handle
// Input:             handle to be copied
// Output:            None

template<class Type>
inline CoolHandle<Type>::CoolHandle(const CoolHandle<Type>& h) 
: rep(h.rep) {
  CoolShared::reference(this->rep);             // inc count on shared obj
}

// ~Handle  -- Destructor for Handle class, dec count and free object.
// Input:              None
// Output:             None

template<class Type>
inline CoolHandle<Type>::~CoolHandle() {
  if (CoolShared::dereference(this->rep) == 0)  
    delete rep;                                 // dec count on obj & delete
}

// operator-> -- Dereference overloaded, to make handle look like pointer.
// Input:              None
// Output:             pointer to object.

template<class Type>
inline Type* CoolHandle<Type>::operator->() {
  return this->rep;                             
}


// operator= -- Assignment to another pointer, check for aliasing.
// Input:              pointer to object
// Output:             handle on lhs of =

template<class Type>
inline CoolHandle<Type>& CoolHandle<Type>::operator=(Type* optr) {
  if (this->rep != optr) {                      // aliasing -> premature delete
    if (CoolShared::dereference(this->rep)==0)
      delete this->rep;                         // dec count on old obj & delete
    this->rep = optr;
    CoolShared::reference(this->rep);           // inc count on shared obj
  }
  return *this;
}


// operator=(Type&) -- Assignment to another object reference
// Input:              reference to object
// Output:             handle on lhs of =

template<class Type>
inline CoolHandle<Type>& CoolHandle<Type>::operator=(Type& obj) {
  if (this->rep != &obj) {                      // aliasing cause premature delete
    if (CoolShared::dereference(this->rep) == 0)
      delete this->rep;                         // dec count on old obj & delete
    this->rep = &obj;
    obj.reference();                            // inc count on shared obj
  }
  return *this;
}

// operator= -- Assignment to another handle
// Input:              handle on rhs of =
// Output:             handle on lhs of =

template<class Type>
inline CoolHandle<Type>& CoolHandle<Type>::operator=(const CoolHandle<Type>& h) {
  if (this->rep != h.rep) {                     // aliasing cause premature delete
    if (CoolShared::dereference(this->rep) == 0)
      delete this->rep;                         // dec count on old obj & delete
    this->rep = h.rep;
    CoolShared::reference(this->rep);           // inc count on shared obj
  }
  return *this;
}

// ptr  -- Manual conversion to pointer
// Input:              handle to object
// Output:             pointer to object

template<class Type>
inline Type* ptr (CoolHandle<Type>& h) {
  return h.rep;
}

// ref  -- Manual conversion to reference
// Input:              handle to object
// Output:             ref to object

template<class Type>
inline Type& ref (CoolHandle<Type>& h) {
#if ERROR_CHECKING
    if (!h.rep)
      h.null_pointer_error("ref", "Type");
#endif
  return *(h.rep);
}

// Handle hdl (Type*) -- Manual conversion to handle, returned by value.
//                     A temporary handle is created and deleted by compiler.
// Input:              pointer to object
// Output:             handle to object

template<class Type>
inline CoolHandle<Type> hdl(Type* optr) {
  CoolHandle<Type> h(optr);                     // handle with given optr
  return h;                                     // copy on return by value
}

// operator Type*  -- Automatic conversion to pointer
//                     Be careful not to store and delete ptrs outside of handles.
// Input:              handle to object
// Output:             pointer to object

template<class Type> 
inline CoolHandle<Type>::operator Type* () {
  return this->rep;
}


// operator Type&  -- Automatic conversion to reference
// Input:              handle to object
// Output:             reference to object

template<class Type> 
inline CoolHandle<Type>::operator Type& () {
#if ERROR_CHECKING
    if (!this->rep)
      this->null_pointer_error("operator Type&", #Type);
#endif
  return *(this->rep);
}

// Operator== -- Overload comparison operators for handles. Compare pointers.
// Input:             handles 
// Ouput:             Boolean

#define IMPLEMENT_HANDLE_OPERATOR(op)                                         \
template<class Type>                                          \
inline Boolean operator op (const CoolHandle<Type>& h1,                       \
                            const CoolHandle<Type>& h2) {                     \
  return h1.rep op h2.rep;                                                    \
}

IMPLEMENT_HANDLE_OPERATOR(==)
IMPLEMENT_HANDLE_OPERATOR(!=)
IMPLEMENT_HANDLE_OPERATOR(<=)


#undef IMPLEMENT_HANDLE_OPERATOR
  

// operator<< -- print handle object to ostream.
// Input:              ostream and handle to object
// Output:             ostream

template<class Type>
inline ostream& operator<< (ostream& os, const CoolHandle<Type>& h) {
  Type* r = h.rep;
  if (r) 
    return os << *r;
  else
    return os << "Empty";
}


// operator<< -- print pointer to a handle to ostream.
// Input:              ostream and pointer to handle 
// Output:             ostream

template<class Type>
inline ostream& operator<< (ostream& os, const CoolHandle<Type>* h) {
  return os << *h;
}

template<class Type>
inline void CoolHandle<Type>::null_pointer_error (const char* fcn, 
                                                  const char* type) {
  //RAISE (Error, SYM(CoolHandle), SYM(NullPointer))
  printf ("CoolHandle<%s>::%s: Pointer inside handle is NULL.\n",
          type, fcn);
  abort ();
}

#endif                                          // HANDLEH

