/**** I shouldn't have redesigned this because the original syntax is
      legal C++ (BC++ 3.1 and gcc don't support it yet, though).
      --Jam, 11-19-92
****/
//
// Copyright (C) 1991 Texas Instruments Incorporated.
//
// Permission is granted to any individual or institution to use, copy, modify,
// and distribute this software, provided that this complete copyright and
// permission notice is maintained, intact, in all copies and supporting
// documentation.
//
// Texas Instruments Incorporated provides this software "as is" without
// express or implied warranty.
//
//
// Created: MBN 08/21/89 -- Initial design and implementation
// Updated: LGO 10/05/89 -- Make destructor inline
// Updated: LGO 12/04/89 -- Efficiency re-write
// Updated: DLS 03/27/91 -- New lite version
// Updated: JAM 08/21/92 -- completely redesigned because modern template
//                          do not support previous style
//
// The  parameterized  Range<Type,lbound,hbound> class is publicly derived from
// the Range class  and supports arbitrary  user-defined ranges  for a type  of
// object or built-in data type. This allows other higher level data structures
// such as  the Vector and  List container classes  to  be parameterized over a
// range of values for some  type so  that the programmer does  not have to add
// bounds checking code to the application. A Vector  of positive integers, for
// example, would  be easy  to declare, facilitating bounds checking restricted
// to the code that implements the type, not the Vector.
//
// The  inclusive  upper and  lower   bounds for   the  range are  specified as
// arguments  to the parameterized  type  declaration and  implementation macro
// calls.   They are declared  as C++ constants  of  the  appropriate type.  No
// storage is  allocated and all  references are compiled out by  the compiler.
// Once  declared, a Range<Type,lbound,hbound> object cannot  have its upper or
// lower bounds   changed,   since maintenance of all instances   would require
// significant and unwarranted overhead.  Each Range<Type,lbound,hbound> object
// has  a single private  data  slot that  holds ths instance value.  There are
// three constructors.   The first is  a simple  empty  constructor. The second
// accepts and sets an initial value.   The third  takes a reference to another
// Range<Type,lbound,hbound> object and duplicates its value.
//
// All  Range<Type,lbound,hbound>  methods  are  implemented   as small  inline
// functions to provide efficient encapsulation of objects,  including built-in
// types such as int.  Methods are provided to get  the  lower and upper bounds
// and set the value of the instance data  slot.  Assignment of  a value or one
// object to another is supported by the overloaded operator= method.  Finally,
// an implicit conversion  from a   Range<Type,lbound,hbound> object to a  Type
// value is provided to allow mixed expressions.
//

#ifndef RANGEH                                  // If no Range class
#define RANGEH

#include <iostream.h>

#include <cool/misc.h>
#include <cool/Comparator.h>

#define COOL_DefineBoundsType(TYPE) \
template <TYPE LOW, TYPE HIGH> \
struct CoolBounds_##TYPE { \
  typedef TYPE Type; \
  inline static Type low()    /* Get low value range */ \
    { return LOW; } \
  inline static Type high()   /* Get high value range */ \
    { return HIGH; } \
}

COOL_DefineBoundsType(char);     // defines CoolBounds_char
COOL_DefineBoundsType(int);      // defines CoolBounds_int

#define CoolRangeM(TYPE,LOW,HIGH) CoolRange< CoolBounds_##TYPE<LOW,HIGH> >

#define COOL_DefineRange(TYPE, LOW, HIGH, NAME) \
struct CoolBounds_##NAME { \
  typedef TYPE Type; \
  static Type low() { return LOW; } \
  static Type high() { return HIGH; } \
}; \
typedef CoolRange< CoolBounds_##NAME > NAME

template <class Bounds>
class CoolRange : public Bounds {
public:
  CoolRange();     // Default ctor, initialize to lower bound
//##  CoolRange(const Type& value); // Construct with value
  // uses default copy-ctor and assignment operator
  void operator=(const CoolRange& r) { this->data = r.data; } //## should not be necessary

//##  inline void set (const Type&);            // Set instance value

//##  inline operator Type () const;            // Implicit type conversion

//##   inline friend ostream& operator<<(ostream& os, const CoolRange<Bounds>& r);
//##   inline friend istream& operator>>(istream& is, CoolRange<Bounds>& r);
#ifndef COMPILERFIXED //##
  CoolRange(const Bounds::Type& value) { this->set(value); }

  void set (const Bounds::Type& value) {
     if (lessthan(value, low()) || greaterthan(value, high()))
        this->report_set_error ();                      // Raise exception
     this->data = value;
     }

   operator Bounds::Type() const {return this->data;}

   friend ostream& operator<<(ostream& os, const CoolRange<Bounds>& r)
      { return os << r.data; }
   friend istream& operator>>(istream& is, CoolRange<Bounds>& r) {
      Type value;
      if (is>>value)                // if input succeeds
         if (CoolRange<Bounds>::lessthan(value, r.low())
               || CoolRange<Bounds>::greaterthan(value, r.high()))
            r.data = value;         // value is good
         else
            is.clear(ios::badbit);     // input value out of range
      return is;
      }
#endif
private:
  Type data;                                    // Storage for instance value

  void report_set_error () const;

  // convenient functions to test values
  static int lessthan(const Type& a, const Type& b)
    { return CoolComparator<Type>::lessthan(a,b); }
  static int greaterthan(const Type& a, const Type& b)
    { return !CoolComparator<Type>::lessthan(a,b); }
};

// CoolRange<Bounds> -- Default constructor (no parameters)
// Input:         None
// Output:        None
template<class Bounds>
inline CoolRange<Bounds>::CoolRange() : data(low()) {}


#ifdef COMPILERFIXED //##

// CoolRange<Bounds> -- Constructor with initial value
// Input:         Reference to value
// Output:        None

template<class Bounds>
inline CoolRange<Bounds>::CoolRange(const Bounds::Type& value) {
  this->set(value);
}


// set -- Set the value of the object
// Input:  Value to set
// Output: None

template<class Bounds>
inline void CoolRange<Bounds>::set (const Bounds::Type& value) {
  if (lessthan(value, low()) || greaterthan(value, high()))
     this->report_set_error ();                 // Raise exception
  this->data = value;
}

// operator Type() -- Provide implicit conversion to what ever type this 
//                    class is parameterized over to allow mixed expressions
// Input:             None
// Output:            Value of object

template<class Bounds>
inline CoolRange<Bounds>::operator Bounds::Type() const {return this->data;}


// operator<< -- Overload output operator for CoolRange objects
// Input:        CoolRange reference
// Output:       Formatted output and stream descriptor

template<class Bounds>
inline ostream& operator<<(ostream& os, const CoolRange<Bounds>& r) {
   return os << r.data;
}


// operator>> -- Overload input operator for CoolRange objects
// Input:        CoolRange reference
// Output:       Formatted input and stream descriptor

template<class Bounds>
inline istream& operator>>(istream& is, CoolRange<Bounds>& r) {
   Type value;
   if (is>>value)                // if input succeeds
      if (lessthan(value, low()) || greaterthan(value, high()))
         r.data = value;         // value is good
      else
         is.clear(ios::bad);     // input value out of range
   return is;
}

#endif   // FIXEDCOMPILER

#endif                                          // End #ifdef of RANGEH
