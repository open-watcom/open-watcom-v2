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
// Changed: VDN 04/15/92 -- Lice version
//
// Value is a general slot capable of taking void* and all primitive types.
// It is needed to store any value in a property list, and to do conversion.
// If your application has a base class, then you can add pointer to this
// base class as a type of Value, instead of using void*.
//

#ifndef VALUEH
#define VALUEH

#ifndef STREAMH
#include <iostream.h>                           // for iostream ops
#define STREAMH
#endif

#ifndef MISCELANEOUSH           // If we have not included this file,
#include <cool/misc.h>          // include miscelaneous useful definitions.
#endif

class CoolValue {
public:
  CoolValue ();                                 // empty constructor
  CoolValue (const CoolValue&);                 // copy constructor
  inline ~CoolValue();                          // destructor
  
  CoolValue& operator= (const CoolValue&);      // assignment
  Boolean operator== (const CoolValue&) const;  // equality test
  
  inline CoolValue (char);                      // char <-> Value
  inline CoolValue& operator= (char);
  inline operator char () const;        
  
  inline CoolValue (int);                       // int <-> Value
  inline CoolValue& operator= (int);
  inline operator int () const;
  
  inline CoolValue (unsigned);                  // unsigned <-> Value
  inline CoolValue& operator= (unsigned);
  inline operator unsigned () const;
  
  inline CoolValue (long);                      // long <-> Value
  inline CoolValue& operator= (long);
  inline operator long () const;
  
  inline CoolValue (float);                     // float <-> Value
  inline CoolValue& operator= (float);
  inline operator float () const;
  
  inline CoolValue (double);                    // double <-> Value
  inline CoolValue& operator= (double);
  inline operator double () const;
  
  inline CoolValue (void*);                     // void* <-> Value
  inline CoolValue& operator= (void*);
  inline operator void* () const;

  friend ostream& operator<< (ostream&, const CoolValue&); // output
  /*inline##*/ friend ostream& operator<< (ostream&, const CoolValue*);

protected:
  enum Type {                                   // for correct print format
    UNKNOWN = 0,                                // and some run time type checking
    CHAR,
    INT,
    UNSIGNED,
    LONG,
    FLOAT,
    DOUBLE,
    VOIDP
    };
  Type type;                            
  char data [8];                                // enough to store anything

  void type_error (const char* fcn, Type t1, Type t2) const; // Raise exception
};

// ~CoolValue()  -- Destructor
// Input:   none
// Output:  none

inline CoolValue::~CoolValue()          {}      // nothing needed

// CoolValue(Type)  -- Constructor from fundamental types: char, int, 
//                 unsigned, long, float, double, void*, Generic*
// Input:   char, number, or pointer
// Ouput:   Value reference

// operator Type() -- Conversion to fundamental types.
// Input:   Value reference
// Output:  data of Value in fundamental type.

inline CoolValue::CoolValue (char c) 
: type(CHAR) {
  *((char *)this->data) = c;                    // align with start of data
}               

inline CoolValue& CoolValue::operator= (char c) {
  if ((this->type == CHAR) || (this->type == UNKNOWN)) {
    this->type = CHAR;
    *((char *)this->data) = c;
  } else 
    this->type_error("operator=", this->type, CHAR);
  return *this;
}

inline CoolValue::operator char () const {
  char c = 0;
  switch (this->type) {
  case CHAR:
    c = *((char *)this->data); 
    break;
  default:
    this->type_error("operator char",this->type, CHAR);
    break;
  }
  return c;
}       

inline CoolValue::CoolValue (unsigned u) 
: type(UNSIGNED) { 
  *((unsigned *) this->data) = u;
}

inline CoolValue& CoolValue::operator= (unsigned u) {
  if ((this->type == UNSIGNED) || (this->type == UNKNOWN)) {
    this->type = UNSIGNED;
    *((unsigned *)this->data) = u;
  } else 
    this->type_error("operator=", this->type, UNSIGNED);
  return *this;
}

inline CoolValue::operator unsigned () const {
  unsigned u = 0;
  switch (this->type) {
  case CHAR:
    u = *((char *) this->data); 
    break;
  case UNSIGNED:
    u = *((unsigned *) this->data); 
    break;
  default:
    this->type_error("operator unsigned", this->type, UNSIGNED);
    break;
  }
  return u;
}

inline CoolValue::CoolValue (int i) 
: type(INT) {
  *((int *) this->data) = i;
}       

inline CoolValue& CoolValue::operator= (int i) {
  if ((this->type == INT) || (this->type == UNKNOWN)) {
    this->type = INT;
    *((int *)this->data) = i;
  } else 
    this->type_error("operator=", this->type, INT);
  return *this;
}

inline CoolValue::operator int () const {
  int i = 0;
  switch (this->type) {
  case CHAR:
    i = *((char *) data); 
    break;
  case UNSIGNED:
    i = *((unsigned *) data); 
    break;
  case INT:
    i = *((int *) data); 
    break;
  default:
    this->type_error("operator int",this->type, INT);
    break;
  }
  return i;
}

inline CoolValue::CoolValue (long l) 
: type(LONG) {
  *((long *) this->data) = l;
}

inline CoolValue& CoolValue::operator= (long l) {
  if ((this->type == LONG) || (this->type == UNKNOWN)) {
    this->type = LONG;
    *((long *)this->data) = l;
  } else 
    this->type_error("operator=", this->type, LONG);
  return *this;
}

inline CoolValue::operator long () const {
  long l = 0;
  switch (this->type) {
  case CHAR:
    l = *((char *) data); 
    break;
  case UNSIGNED:
    l = *((unsigned *) data); 
    break;
  case INT:
    l = *((int *) data); 
    break;
  case LONG:
    l = *((long *) data); 
    break;
  default:
    this->type_error("operator long",this->type, LONG);
    break;
  }
  return l;
}

inline CoolValue::CoolValue (float f) 
: type(FLOAT) {
  *((float *) this->data) = f; 
} 

inline CoolValue& CoolValue::operator= (float f) {
  if ((this->type == FLOAT) || (this->type == UNKNOWN)) {
    this->type = FLOAT;
    *((float *)this->data) = f;
  } else 
    this->type_error("operator=", this->type, FLOAT);
  return *this;
}

inline CoolValue::operator float () const { 
  float f = 0;
  switch (this->type) {
  case CHAR:
    f = *((char *) data); 
    break;
  case UNSIGNED:
    f = *((unsigned *) data); 
    break;
  case INT:
    f = *((int *) data);                        // precision may be lost
    break;
  case LONG:
    f = *((long *) data);                       // precision may be lost
    break;
  case FLOAT:
    f = *((float *) data); 
    break;
  case DOUBLE:
    f = *((double *) data);                     // precision may be lost
    break;
  case VOIDP:                                   // avoid warning from CC
  case UNKNOWN:
  default:
    this->type_error("operator float",this->type, FLOAT);
    break;
  }
  return f;
}

inline CoolValue::CoolValue (double d) 
: type(DOUBLE) {
  *((double *) this->data) = d; 
} 

inline CoolValue& CoolValue::operator= (double d) {
  if ((this->type == DOUBLE) || (this->type == UNKNOWN)) {
    this->type = DOUBLE;
    *((double *)this->data) = d;
  } else 
    this->type_error("operator=", this->type, DOUBLE);
  return *this;
}

inline CoolValue::operator double () const { 
  double d = 0;
  switch (this->type) {
  case CHAR:
    d = *((char *) data); 
    break;
  case UNSIGNED:
    d = *((unsigned *) data); 
    break;
  case INT:
    d = *((int *) data);                        // precision may be lost
    break;
  case LONG:
    d = *((long *) data);                       // precision may be lost
    break;
  case FLOAT:
    d = *((float *) data); 
    break;
  case DOUBLE:
    d = *((double *) data);             
    break;
  case VOIDP:                                   // avoid warning from CC
  case UNKNOWN:
  default:
    this->type_error("operator double",this->type, DOUBLE);
    break;
  }
  return d;
}

inline CoolValue::CoolValue (void* v) 
: type(VOIDP) { 
  *((void **) this->data) = v; 
}

inline CoolValue& CoolValue::operator= (void* d) {
  if ((this->type == VOIDP) || (this->type == UNKNOWN)) {
    this->type = VOIDP;
    *((void* *)this->data) = d;
  } else 
    this->type_error("operator=", this->type, VOIDP);
  return *this;
}

inline CoolValue::operator void* () const {
  void* v = NULL;
  switch (this->type) {
  case VOIDP:
    v = *((void**) this->data); 
    break;
  default:
    this->type_error("operator void*",this->type, VOIDP);
    break;
  }
  return v;
}


// operator<<  -- Output value* to ostream
// Input:    ostream, pointer to value
// Output:   ostream reference

inline ostream& operator<< (ostream& os, const CoolValue* v) {
  os << *v;
  return os;
}

#endif
