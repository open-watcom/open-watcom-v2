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
//
// Rep Invariant: 
//   1. norm = 1, for a rotation.
//   2. position vector represented by imaginary quaternion.


#include <cool/Quaternion.h>

#include <cool/Matrix.C>
#include <cool/M_Vector.C>

#define CoolEnvelope CoolEnvelope_Quaternion


// Quaternion -- Construct Quaternion from the 4 components
// Input:      3 imaginary and 1 real components
// Ouput:      none.

CoolQuaternion::CoolQuaternion (float x, float y, float z, float r) 
: CoolM_Vector<float>(4) {
  this->data[0] = x;                            // 3 first elmts are 
  this->data[1] = y;                            // imaginary parts
  this->data[2] = z;
  this->data[3] = r;                            // last element is real part
}  

// Quaternion -- Construct Quaternion from angle and axis of rotation.
// Input:      angle in radians, and 3D normalized vector for axis of rotation.
// Ouput:      none.

CoolQuaternion::CoolQuaternion (const CoolM_Vector<float>& axis, float angle)
: CoolM_Vector<float>(4) {
  double a = angle / 2.0;                       // half angle
  double s = sin(a);
  CoolM_Vector<float>& axis2 = (CoolM_Vector<float>&) axis; // cast away const
  for (int i = 0; i < 3; i++)                   // imaginary vector is sin of
    this->data[i] = s * axis2(i);               // half angle multiplied with axis
  this->data[3] = cos(a);                       // real part is cos of half angle
}


// should be part of transform class.

CoolEnvelope_Matrix/*##*/< CoolMatrix<float> > extract_3d_rotation (const CoolMatrix<float>& transform) {
  if (transform.rows() != transform.columns()) {
    printf("Ambiguous rotation submatrix from a %dx%d transform.\n", 
           transform.rows(), transform.columns());
    abort();
  }
  if (transform.rows() >= 3)
    return (CoolEnvelope_Matrix< CoolMatrix<float> > &) transform.extract(3, 3, 0, 0);       // rotation is top-left most block
  else {
    CoolMatrix<float> rot(3, 3, 0.0);
    CoolMatrix<float>& transform2 = (CoolMatrix<float>&) transform;
    for (int i = 0; i < 2; i++)                 // extract rotation in xy-plane
      for (int j = 0; j < 2; j++)               // from transform.
        rot(i,j) = transform2(i,j);
    rot(2,2) = 1.0;                             // zz component 
    CoolEnvelope_Matrix/*##*/< CoolMatrix<float> >& result = *((CoolEnvelope_Matrix< CoolMatrix<float> >*) &rot);
    return result;                              // avoid deep copy with envelope
  }
}

// Quaternion -- Construct Quaternion from 2-4 square row-major transform.
//              Basis vectors is stored row-wise in submatrix(3,3,0,0).
// Input:       Transformation matrix, with orthonormal basis vectors.
// Output:      none.

CoolQuaternion::CoolQuaternion (const CoolMatrix<float>& transform) 
: CoolM_Vector<float>(4) {
  CoolMatrix<float> rot = extract_3d_rotation(transform);
  double d0 = rot(0,0), d1 = rot(1,1), d2 = rot(2,2);
  double xx = 1.0 + d0 - d1 - d2;               // from the diagonal of rotation
  double yy = 1.0 - d0 + d1 - d2;               // matrix, find the terms in
  double zz = 1.0 - d0 - d1 + d2;               // each Quaternion compoment
  double rr = 1.0 + d0 + d1 + d2;

  double max = rr;                              // find the maximum of all
  if (xx > max) max = xx;                               // diagonal terms.
  if (yy > max) max = yy;
  if (zz > max) max = zz;

  if (rr == max) {
    double r4 = sqrt(rr * 4.0);
    this->x() = (rot(1,2) - rot(2,1)) / r4;     // find other components from
    this->y() = (rot(2,0) - rot(0,2)) / r4;     // off diagonal terms of
    this->z() = (rot(0,1) - rot(1,0)) / r4;     // rotation matrix.
    this->r() = r4 / 4.0;
  } else if (xx == max) {
    double x4 = sqrt(xx * 4.0);
    this->x() = x4 / 4.0;
    this->y() = (rot(0,1) + rot(1,0)) / x4;
    this->z() = (rot(0,2) + rot(2,0)) / x4;
    this->r() = (rot(1,2) - rot(2,1)) / x4;
  } else if (yy == max) {
    double y4 = sqrt(yy * 4.0);
    this->x() = (rot(0,1) + rot(1,0)) / y4;
    this->y() =  y4 / 4.0;
    this->z() = (rot(1,2) + rot(2,1)) / y4;
    this->r() = (rot(2,0) - rot(0,2)) / y4;
  } else {
    double z4 = sqrt(zz * 4.0);
    this->x() = (rot(0,2) + rot(2,0)) / z4;
    this->y() = (rot(1,2) + rot(2,1)) / z4;
    this->z() =  z4 / 4.0;
    this->r() = (rot(0,1) - rot(1,0)) / z4;
  }
}

// angle -- Positive angle of rotation

float CoolQuaternion::angle () const {
  double sin = 0;
  for (int i = 0; i < 3; i++)                   // compute sin of half angle
    sin += this->data[i] * this->data[i];       // from imaginary vector
  sin = sqrt(sin);
  double cos = this->data[3];                   // cos of half angle
  return (2.0 * atan2 (sin, cos));              // angle is always positive
}

// axis -- Normalized direction vector of axis of rotation.
//         Returns (0, 0, 1) if Quaternion is zero, and axis is not well defined.

CoolEnvelope_M_Vector/*##*/< CoolM_Vector<float> > CoolQuaternion::axis () const {
  CoolM_Vector<float> dir = this->imaginary(); // dir parallel to imag. part
  float mag = dir.magnitude();
  if (mag == 0) {
    cout << "Axis is not well defined for zero Quaternion. Use (0,0,1) instead. "
      << endl;
    dir.z() = 1.0;                              // or signal exception here.
  } else 
    dir /= mag;                                 // normalize direction vector
  CoolEnvelope_M_Vector/*##*/< CoolM_Vector<float> >& result = *((CoolEnvelope_M_Vector/*##*/< CoolM_Vector<float> >*) &dir);
  return result;                                // avoid deep copy
}


// operator== -- Components of Quaternion are compared with fuzz = 1.0e-6

Boolean CoolQuaternion::operator== (const CoolQuaternion& rhs) const {
  for (int i = 0; i < 4; i++)
    if (fabs(this->data[i] - rhs.data[i]) > 1.0e-6) // more fuzz because of
      return FALSE;                                 // sqrt, etc...
  return TRUE;
}


// rotation_transform -- Convert to a rotation transform matrix with 
//            specified rows & cols. Assumed normalized Quaternion.
// Input:     number of rows and columns specifying format of transform.
// Output:    matrix for rotation transform equivalent to Quaternion.

CoolEnvelope_Matrix/*##*/< CoolMatrix<float> > CoolQuaternion::rotation_transform (int dim) const {
  CoolMatrix<float> rot(dim, dim, 0.0);
  CoolQuaternion q(*this);                      
  if (dim == 2) {
    q.x() = q.y() = 0.0;                        // find best approx rotation
    q.normalize();                              // along z-axis only.
    double s = q.z(), c = q.r();
    rot(0,0) = rot(1,1) = (c * c) - (s * s);
    rot(0,1) = 2.0 * s * c;
    rot(1,0) = -rot(0,1);
  } 
  if (dim >= 3) {
    double x2 = q.x() * q.x();
    double y2 = q.y() * q.y();
    double z2 = q.z() * q.z();
    double r2 = q.r() * q.r();
    rot(0,0) = r2 + x2 - y2 - z2;               // fill diagonal terms
    rot(1,1) = r2 - x2 + y2 - z2;
    rot(2,2) = r2 - x2 - y2 + z2;
    double xy = q.x() * q.y();
    double yz = q.y() * q.z();
    double zx = q.z() * q.x();
    double rx = q.r() * q.x();
    double ry = q.r() * q.y();
    double rz = q.r() * q.z();
    rot(0,1) = 2 * (xy + rz);                   // fill off diagonal terms
    rot(0,2) = 2 * (zx - ry);
    rot(1,2) = 2 * (yz + rx);
    rot(1,0) = 2 * (xy - rz);
    rot(2,0) = 2 * (zx + ry);
    rot(2,1) = 2 * (yz - rx);
  }
  if (dim == 4) rot(3,3) = 1.0;                 // for homogeneous transform
  CoolEnvelope_Matrix/*##*/< CoolMatrix<float> >& result = *((CoolEnvelope_Matrix/*##*/< CoolMatrix<float> >*) &rot);
  return result;                                // avoid deep copy with envelope
}

// conjugate -- Conjugate of a Quaternion has same real part and 
//            opposite imaginary part.

CoolEnvelope<CoolQuaternion> CoolQuaternion::conjugate () const {
  CoolQuaternion* self = (CoolQuaternion*) this;        // cast away const
  CoolQuaternion conj(-self->x(), -self->y(), -self->z(), self->r());
  CoolEnvelope<CoolQuaternion>& result = *((CoolEnvelope<CoolQuaternion> *) &conj); // avoid deep copy with
  return result;                                  // envelope
}

// inverse -- Inverse Quaternion exists only for nonzero Quaternion.
//            If Quaternion represents rotation, inverse is conjugate.

CoolEnvelope<CoolQuaternion> CoolQuaternion::inverse () const {
  CoolQuaternion inv = this->conjugate() / dot_product(*this, *this);
  CoolEnvelope<CoolQuaternion>& result = *((CoolEnvelope<CoolQuaternion> *) &inv); // avoid deep copy with
  return result;                                  // envelope
}

// operator* -- Multiplication of two Quaternions is not symmetric
//            and has fewer operations than mult of orthonormal matrices 
//            If object is rotated by r1, then by r2, then the composed
//            rotation (r2 o r1) is represented by the Quaternion (q2 * q1),
//            and by the matrix (m1 * m2). 
//            Remember that matrix and vector are represented row-wise,
//            and this reverses the composition order.

CoolEnvelope<CoolQuaternion> operator* (const CoolQuaternion& q1, const CoolQuaternion& q2) {
  float r1 = q1.real();                         // real and img parts of args
  float r2 = q2.real();         
  CoolM_Vector<float> i1 = q1.imaginary();
  CoolM_Vector<float> i2 = q2.imaginary();
  float real = (r1 * r2) - dot_product(i1, i2); // real&img of product
#if 0
  abort();
  CoolM_Vector<float> img = i1;
#else
  CoolM_Vector<float> img = ( r1 * i2 + r2 * i1 ) + cross_3d(i1, i2);
#endif
  CoolQuaternion prod(img.x(), img.y(), img.z(), real); 
  CoolEnvelope<CoolQuaternion>& result = *((CoolEnvelope<CoolQuaternion> *) &prod); // avoid deep copy with
  return result;                                  // envelope
}

// rotate --  Transform 3D vector by rotation Quaternion
//            

void CoolQuaternion::rotate (CoolM_Vector<float>& v) const {
  float r = this->real();
  CoolM_Vector<float> i = this->imaginary();
#if 0
  abort();
#else
  v += float(2 * r) * cross_3d(i, v) -          // fewer number of mults
    float(2) * cross_3d(cross_3d(i, v), i);
#endif
}

//### {Jam}This was not originally in the code, but I had to add it because
// BC++ 3.1 said Envelope needed it.
inline void operator*=(CoolQuaternion& q1, const CoolQuaternion& q2)
 { q1 = q1 * q2; }


#undef CoolEnvelope

