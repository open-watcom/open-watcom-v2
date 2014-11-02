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


#include <cool/Value.h>

static char* CoolValue_Type_Names_s[] = 
{
  "Unknown",                                    // formatted output
  "char",
  "int",
  "unsigned",
  "long",
  "float",
  "double",
  "void*"
};

static int CoolValue_Type_Sizes_s[] = 
{
  0,                                            // sizes in bytes for copying
  sizeof(char),
  sizeof(int),
  sizeof(unsigned),
  sizeof(long),
  sizeof(float),
  sizeof(double),
  sizeof(void*)
};

// type_error()  -- Raise exception for unmatched type at runtime.
// Input:    function name, and 2 types.
// Output:   exception or abort.

void CoolValue::type_error(const char* fcn, Type t1, Type t2) const {
  //RAISE (Error, SYM(CoolValue), SYM(Invalid_Type),
  printf ("CoolValue::%s(): Type %s and %s do not match.\n", 
          fcn, CoolValue_Type_Names_s[t1], CoolValue_Type_Names_s[t2]);
  abort ();
}

// CoolValue()  -- Empty constructor.
// Input:   none
// Output:  Value reference

CoolValue::CoolValue () 
: type(UNKNOWN) {               
  for (int i = 0; i < sizeof(this->data); i++)  // zero out all bytes
    this->data[i] = 0;
}


// CoolValue()  -- Copy constructor.
// Input:   Value to be copied
// Output:  Value reference

CoolValue::CoolValue (const CoolValue& v) 
: type(v.type) {
  for (int i = 0; i < CoolValue_Type_Sizes_s[v.type]; i++) // copy all bytes
    this->data[i] = v.data[i];
}

// operator =   -- Assignment right to left values, no type-checking.
// Input:   left and right values
// Output:  right value reference

CoolValue& CoolValue::operator= (const CoolValue& v) {
  this->type = v.type;
  for (int i = 0; i < CoolValue_Type_Sizes_s[v.type]; i++) // copy all bytes
    this->data[i] = v.data[i];
  return *this;
}

// operator ==  -- Test for equality of two values.
// Input:   this and second value
// Output:  TRUE/FALSE for equality or not

Boolean CoolValue::operator== (const CoolValue& v) const {
  if (this->type != v.type)                     // check stored type
    return FALSE;
  switch (this->type) {                         // base on stored type
  case UNKNOWN:                                 // do equality test
    return TRUE;
  case CHAR:                            
    return *((char*) this->data) == *((char*) v.data);
  case INT:
    return *((int*) this->data) == *((int*) v.data);
  case UNSIGNED:
    return *((unsigned*) this->data) == *((unsigned*) v.data);
  case LONG:
    return *((long*) this->data) == *((long*) v.data);
  case FLOAT:
    return *((float*) this->data) == *((float*) v.data); // == with machine fuzz
  case DOUBLE:
    return *((double*) this->data) == *((double*) v.data); 
  case VOIDP:
    return *((void**) this->data) == *((void**) v.data);
  default:
    return FALSE;
  }
}

// operator<<  -- Output value to ostream
// Input:    ostream, reference to value
// Output:   ostream reference

ostream& operator<< (ostream& os, const CoolValue& v) {
  os << '(' << CoolValue_Type_Names_s[v.type] << ')';
  switch (v.type) {                             // base on stored type
  case CoolValue::CHAR:                         // do correct printout 
    os << *((char*) v.data);                    // of the bits.
    break;
  case CoolValue::INT:
    os << *((int*) v.data);
    break;
  case CoolValue::UNSIGNED:
    os << *((unsigned*) v.data);
    break;
  case CoolValue::LONG:
    os << *((long*) v.data);
    break;
  case CoolValue::FLOAT:
    os << *((float*) v.data);
    break;
  case CoolValue::DOUBLE:
    os << *((double*) v.data);
    break;
  case CoolValue::VOIDP:
    os << *((void**) v.data);
    break;
  case CoolValue::UNKNOWN:    
  default:
    os << "Undefined";
    break;
  }
  return os;
}

