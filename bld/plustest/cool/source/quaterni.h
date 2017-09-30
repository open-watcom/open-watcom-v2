//
// Copyright (C) 1992 General Electric Company.
//
// Permission is granted to any individual or institution to use, copy, modify,
// and distribute this software, provided that this complete copyright and
// permission notice is maintained, intact, in all copies and supporting
// documentation.
//
// General Electric Company,
// provides this software "as is" without express or implied warranty.
//
// Created: VDN 06/23/92 -- design and implementation
// Updated: JAM 09/26/92 -- modernized template syntax, remove macro hacks
// Updated: JAM 09/26/92 -- made envelope use modern templates
// Updated: JAM 09/30/92 -- added operator*=(Quaternion,Quaternion) because
//                          BC++ 3.1 says Envelope needed it
//
// Quaternion is a vector with 1 real and 3 imaginary parts:
//       q = cos(theta/2) + sin(theta/2) (x i + y j + z k)
// Quaternion is represented by a 4-elmt vector, with imaginary parts
// being the first 3 elements, so that they are aligned with x, y, z, and
// the real part, the fourth element.
//
// References:
// 1. Horn, B.K.P. (1987) Closed-form solution of absolute orientation using 
//       unit quaternions. J. Opt. Soc. Am. Vol 4, No 4, April.
// 2. Horn, B.K.P. (1987) Robot Vision. MIT Press. pp. 437-551.

#ifndef QUATERNIONH
#define QUATERNIONH

#ifndef MATRIXH
#include <cool/Matrix.h>
#endif

#ifndef M_VECTORH
#include <cool/M_Vector.h>
#endif

//## hack to workaround BC++ 3.1 Envelope bug
#undef CoolEnvelope_H
#define CoolEnvelope CoolEnvelope_Quaternion

template<class CoolLetter> class CoolEnvelope;

class CoolQuaternion : public CoolM_Vector<float> {
 public:
  CoolQuaternion (float x = 0, float y = 0, float z = 0, // default is null quat.
                  float r = 1.0); 
  CoolQuaternion (const CoolM_Vector<float>& axis, // from axis&angle
                  float angle);                 
  CoolQuaternion (const CoolMatrix<float>& transform); // from 2-4 square row-major
  inline CoolQuaternion (const CoolM_Vector<float>& vec); // from 2-4D vector
  inline CoolQuaternion (const CoolQuaternion& q);        // copy constructor
  inline ~CoolQuaternion();                               // free internal array
  
  inline float& x ();                           // imaginary component
  inline float& y ();                           // parallel to axis of rotation
  inline float& z ();
  inline float& r ();                           // real component
  inline float real () const;                           
  inline CoolEnvelope_M_Vector/*##*/< CoolM_Vector<float> > imaginary () const; // imaginary vector part
  
  CoolEnvelope_M_Vector/*##*/< CoolM_Vector<float> > axis () const;             // Axis of rotation
  float angle () const;                         // Angle of rotation
  
  inline CoolQuaternion& operator= (const CoolQuaternion& rhs); // q1 = q2
  inline CoolQuaternion& operator= (CoolEnvelope<CoolQuaternion>& env); // q1 = env
  
  Boolean operator== (const CoolQuaternion& rhs) const; // cmp with fuzz = 1.0e-6
  inline Boolean operator!= (const CoolQuaternion& rhs) const; // instead of 1.0e-8
  
  //inline operator CoolM_Vector<float>& ();      // cast to 4-elmt vector
  CoolEnvelope_Matrix/*##*/< CoolMatrix<float> > rotation_transform (int dim = 4) const; // to 2-4 rot matrix
  
  CoolEnvelope<CoolQuaternion> conjugate () const;              // same real, opposite img part
  CoolEnvelope<CoolQuaternion> inverse () const;                // inverse for nonzero quat
  
  friend CoolEnvelope<CoolQuaternion> operator* (const CoolQuaternion& q1, // q = q1 * q2 * q3
                                    const CoolQuaternion& q2);
  void rotate (CoolM_Vector<float>& v) const;   // rotate 3D v, store result in v.
  
  /*inline##*/ friend ostream& operator<< (ostream& os, const CoolQuaternion& q);
  /*inline##*/ friend ostream& operator<< (ostream& os, const CoolQuaternion* q);
};

//## BC++ 3.1 bug
void hack(CoolQuaternion);

#include <cool/Envelope.h>    //## BC++ 3.1 bug prevents from moving to top


// Quaternion -- Construct Quaternion from 2-, 3- or 4-elmt vector.
//             If nD-vector, construct imaginary Quaternion.
// Input:      2D, 3D location vector, or 4D Quaternion vector.

inline CoolQuaternion::CoolQuaternion (const CoolM_Vector<float>& vec)
: CoolM_Vector<float>(vec) {                    // 1-1 layout between vector&quat
}

// Quaternion -- Copy constructor
// Input:      Quaternion

inline CoolQuaternion::CoolQuaternion (const CoolQuaternion& q) 
  : CoolM_Vector<float>(q) {                    // 1-1 layout between vector&quat
}

// ~Quaternion -- Nothing, since Quaternion has same data as Vector.

inline CoolQuaternion::~CoolQuaternion () {}    // Vector will free data

// x -- Access first imaginary component, along x axis.

inline float& CoolQuaternion::x () {
  return this->data[0];
}

// y -- Access second imaginary component, along y axis.

inline float& CoolQuaternion::y () {
  return this->data[1];
}

// z -- Access third imaginary component, along z axis.

inline float& CoolQuaternion::z () {
  return this->data[2];
}

// r -- Access real component

inline float& CoolQuaternion::r () {
  return this->data[3];
}

// real -- Get real part

inline float CoolQuaternion::real () const {
  return this->data[3];
}

// imaginary -- Get imaginary part

inline CoolEnvelope_M_Vector/*##*/< CoolM_Vector<float> > CoolQuaternion::imaginary () const {
  return (CoolEnvelope_M_Vector< CoolM_Vector<float> > &) this->extract(3,0);
}

  // operator=  -- Assignment q1 = q2;
// Input:    Quaternion on rhs
// Output:   Quaternion on lhs, with contents of Quaternion on rhs copied over.

inline CoolQuaternion& CoolQuaternion::operator= (const CoolQuaternion& rhs) {
  CoolM_Vector<float>::operator=(rhs);          // same as copy vector part
  return *this;
}

// operator=  -- Assignment from an envelope back to real Quaternion
//            Swap the contents over, rhs envelope will be deleted by compiler.

inline CoolQuaternion& CoolQuaternion::operator= (CoolEnvelope<CoolQuaternion>& env) {
  env.shallow_swap((CoolEnvelope<CoolQuaternion>*)this, &env); // same physical layout
  return *this;  
}

// operator!= -- Components of Quaternion are compared with fuzz = 1.0e-6

inline Boolean CoolQuaternion::operator!= (const CoolQuaternion& rhs) const {
  return (!operator==(rhs));
}

// operator M_Vector  --  Automatic conversion to a 4-elmt vector.
// Input:    *this, a Quaternion.
// Output:   a reference to the 4-elmt vector, through a type-cast.
//don't do this, vector is a base class
//inline CoolQuaternion::operator CoolM_Vector<float>& () {
//  return *((CoolM_Vector<float>*) this);        // same physical space
//}

// operator<<  -- Print the components of Quaternion.

  inline ostream& operator<< (ostream& os, const CoolQuaternion& q) {
  return os << *((CoolM_Vector<float>*) &q);
}

inline ostream& operator<< (ostream& os, const CoolQuaternion* q) {
  return os << *((CoolM_Vector<float>*) q);
}

#undef CoolEnvelope

#endif                                          // QuaternionH
