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


#include <cool/Quaternion.h>
#include <cool/test.h>
#include <cool/Quaternion.c>

#define M_PI 3.1415926535

Boolean float_fuzz_equal (const float& f1, const float& f2) {
  return ((fabs(f1 - f2) < 1.0e-7) ? TRUE : FALSE);
}

void test_quaternion () {
  CoolM_Vector<float> k(3, 0.0);
  k.z() = 1.0;
  k.set_compare(&float_fuzz_equal);

  CoolQuaternion q0;
  TEST ("q0()", TRUE, TRUE);
  TEST ("q0.magnitude", q0.magnitude()==1.0, TRUE);
  TEST ("q0.axis", q0.axis() == k, TRUE);
  TEST ("q0.angle", q0.angle() == 0.0, TRUE);
  TEST ("q0.x", q0.x() == 0, TRUE);
  TEST ("q0.y", q0.y() == 0, TRUE);
  TEST ("q0.z", q0.z() == 0, TRUE);
  TEST ("q0.r", q0.r() == 1, TRUE);
  TEST ("q0==q0", q0==q0, TRUE);

  CoolQuaternion q1(k, M_PI);
  TEST ("q1(axis,angle)", TRUE, TRUE);
  TEST ("q1.magnitude", q1.magnitude()==1.0, TRUE);
  TEST ("q1.axis", q1.axis()==k, TRUE);
  TEST ("q1.angle", fabs(q1.angle()-M_PI) < 1.0e-6, TRUE);
  TEST ("q1.x", q1.x() == 0, TRUE);
  TEST ("q1.y", q1.y() == 0, TRUE);
  TEST ("q1.z", q1.z() == 1, TRUE);
  TEST ("q1.r", fabs(q1.r()) < 1.0e-6, TRUE);
  TEST ("q1!=q0", q1!=q0, TRUE);
  
  CoolQuaternion q2((CoolM_Vector<float>&) q0);
  TEST ("q2(Vector)", TRUE, TRUE);
  TEST ("q0==q2", q0==q2, TRUE);
  TEST ("(Vector)q", (CoolM_Vector<float>&)q0 == (CoolM_Vector<float>&)q2, TRUE);
  TEST ("(Vector)q", (CoolM_Vector<float>&)q0 != (CoolM_Vector<float>&)q1, TRUE);
  TEST ("q2=q1", (q2=q1, q2==q1 && q2!=q0), TRUE);

  CoolQuaternion q3(k, M_PI);
  TEST ("q3.axis", q3.axis()==k, TRUE);
  double d = fabs(q3.angle()-M_PI);  //## Borland bug
  TEST ("q3.angle", d < 1.0e-6, TRUE);
  TEST ("q3.angle", fabs(q3.angle()-M_PI) < 1.0e-6, TRUE);
}

void test_product () {
  CoolM_Vector<float> i(3,0), j(3,0), k(3,0);
  i.x() = j.y() = k.z() = 1.0;
  CoolQuaternion q0(k, 0), q1(i, M_PI/2), q2(j, M_PI/2), q3(k, M_PI/2);
  
  TEST ("1*q1==q1*1", (q0*q1)==(q1*q0), TRUE);
  TEST ("(q1*q2)!=(q2*q1)", q1*q2 != q2*q1, TRUE);
  TEST ("(q1*q2)*q3==q1*(q2*q3)", (q1*q2)*q3==q1*(q2*q3), TRUE);
  TEST ("q1.inverse", q1.inverse() == q1.conjugate(), TRUE);
  TEST ("q1*q1.inverse==q1.inverse*q1", q1*q1.inverse()==q1.inverse()*q1, TRUE);
  CoolQuaternion q;
  TEST ("q1*q1.conjugate==1", 
        (q = q1*q1.conjugate(), q==q0), TRUE);
  TEST ("(q1*q2).(q1*q3)==(q1.q1)*(q2.q3)",
        fabs(dot_product((q1*q2),(q1*q3)) -
             dot_product(q1,q1)*dot_product(q2,q3)) < 1.0e-6, TRUE);
  TEST ("(q1*q2).(q1*q2)==(q1.q1)*(q2*q2)",
        fabs(dot_product((q1*q2),(q1*q2)) -
             dot_product(q1,q1)*dot_product(q2,q2)) < 1.0e-6, TRUE);
  TEST ("(q1*q2).q3==q1.(q3*q2.conjugate)",
        fabs(dot_product((q1*q2),q3) -
             dot_product(q1,(q3*q2.conjugate()))) < 1.0e-6, TRUE);
}


void test_transform () {
  CoolM_Vector<float> i(3,0.0), j(3,0.0), k(3,0.0);
  i.x() = j.y() = k.z() = 1.0;
  CoolQuaternion q0(k, 0), q1(i, M_PI/2), q2(j, M_PI/2), q3(k, M_PI/2);

  CoolQuaternion q(q0.rotation_transform());
  TEST ("q(q0.rotation_transform)", q==q0, TRUE);
  q = q1.rotation_transform();
  TEST ("q=q1.rotation_transform", q==q1, TRUE);
  CoolMatrix<float> rotz = q3.rotation_transform(2);
  TEST ("rotz=q3.rotation_transform(2)", 
        (fabs(rotz(0,0)) < 1.0e-6 && fabs(rotz(1,1)) < 1.0e-6 &&
         fabs(rotz(0,1)-1) < 1.0e-6 && fabs(rotz(1,0)+1) < 1.0e-6), TRUE);
  q = rotz;
  TEST ("q(rotz)==q3", q==q3, TRUE);
  CoolM_Vector<float> ii;
  TEST ("q0.rotate(ii)", 
        (ii = i, q0.rotate(ii), ii==i), TRUE);
  TEST ("q1.rotate(ii)", 
        (ii = i, q1.rotate(ii), ii==i), TRUE);
  TEST ("q2.rotate(ii)", 
        (ii = i, q2.rotate(ii), ii==-k), TRUE);
  TEST ("q3.rotate(ii)", 
        (ii = i, q3.rotate(ii), ii==j), TRUE);
}

void test_leak () {
  for (;;) {
    test_quaternion();
    test_product();
    test_transform();
  }
}

int main () {
  START("CoolQuaternion");
  test_quaternion();
  test_product();
  test_transform();
  
#if LEAK
  test_leak();
#endif
  SUMMARY();
  return 0;
}
