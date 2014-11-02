//
// Copyright (C) 1991 Texas Instruments Incorporated.
// Copyright (C) 1992 General Electric Company.
//
// Permission is granted to any individual or institution to use, copy, modify,
// and distribute this software, provided that this complete copyright and
// permission notice is maintained, intact, in all copies and supporting
// documentation.
//
// Texas Instruments Incorporated, General Electric Company,
// provides this software "as is" without express or implied warranty.
//

#include <cool/M_Vector.h>
#include <cool/Matrix.h>
#include <cool/test.h>

#include <cool/M_Vector.C>
#include <cool/Matrix.C>

Boolean Vector_is_data_equal (const int& i1, const int& i2) {
  return ((fabs(double(i1 - i2)) < 1.0e-12) ? TRUE : FALSE);
}

void test_int () {
  //// test constructors, accessors
  CoolM_Vector<int> v0;
  TEST ("CoolM_Vector<int> v0()", (v0.length()==0 || v0.length()==1), TRUE);
  CoolM_Vector<int> v1(2);
  TEST ("CoolM_Vector<int> v1(2)", (v1.length()==2), TRUE);
  CoolM_Vector<int> v2(2,2);
  TEST ("CoolM_Vector<int> v2(2,2)",
        (v2.get(0)==2 && v2.get(1)==2), TRUE);
  TEST ("v0.set_compare", (v0.set_compare(&Vector_is_data_equal), TRUE), TRUE);
  TEST ("v1=2", 
        (v1=2, (v1.get(0)==2 && v1.get(1)==2)), TRUE);
  TEST ("v1 == v2", (v1 == v2), TRUE);
  TEST ("v0 = v2", ((v0 = v2), (v0 == v2)), TRUE);
  TEST ("v2.put(1,3)", (v2.put(1,3),v2.get(1)), 3);
  TEST ("v2.get(1)", v2.get(1), 3);
  TEST ("v0 == v2", (v0 == v2), FALSE);
  TEST ("v0 != v2", (v0 != v2), TRUE);
  TEST ("(v0 == v2)", (v0 == v2), FALSE);
  TEST ("v1.fill(3)",
        (v1.fill(3), (v1.get(0)==3 && v1.get(1)==3)), TRUE);
  TEST ("v2.fill(2)",
        (v2.fill(2), (v2.get(0)==2 && v2.get(1)==2)), TRUE);
  CoolM_Vector<int> v3(3,3,1,2,3);
  TEST ("v3(3,3,1,2,3)",(v3.get(0)==1 && v3.get(1)==2 && v3.get(2)==3), TRUE);
  CoolM_Vector<int> v4(v3);
  TEST ("CoolM_Vector<int> v4(v3)", (v3==v4), TRUE);
  TEST ("v0=v2", (v0=v2, (v0==v2)), TRUE);

  //// test additions and substractions
  TEST ("v0=v2+3",
        ((v0=v2+3), (v0.get(0)==5 && v0.get(1)==5)), TRUE);
  TEST ("v0=3+v2",
        ((v0=3+v2), (v0.get(0)==5 && v0.get(1)==5)), TRUE);
  TEST ("v0+=(-3)",
        (v0+=(-3), (v0.get(0)==2 && v0.get(1)==2)), TRUE);
  TEST ("v0-=(-3)",
        (v0-=(-3), (v0.get(0)==5 && v0.get(1)==5)), TRUE);
  TEST ("v0=v2-3",
        ((v0=v2-3), (v0.get(0)==-1 && v0.get(1)==-1)), TRUE);
  TEST ("v0=3-v2",
        ((v0=3-v2), (v0.get(0)==1 && v0.get(1)==1)), TRUE);
  TEST ("v0= -v2",
        (v0= -v2, (v0.get(0)==-2 && v0.get(1)==-2)), TRUE);

  CoolM_Vector<int> v5(2);
  v0 = v2;
  TEST ("v5=v0+v2",
        ((v5=operator+(v0,v2)), (v5.get(0)==4 && v5.get(1)==4)), TRUE);
  TEST ("v5=v0-v2",
        ((v5=v0-v2), (v5.get(0)==0 && v5.get(1)==0)), TRUE);
  TEST ("v0+=v2", 
        ((v0+=v2), (v0.get(0)==4 && v0.get(1)==4)), TRUE);
  TEST ("v0-=v2", 
        ((v0-=v2), (v0.get(0)==2 && v0.get(1)==2)), TRUE);

  //// test multiplications and divisions
  TEST ("v4=v3*5",
        ((v4=v3*5), (v4.get(0)==5 && v4.get(1)==10 && v4.get(2)==15)), TRUE);
         
  TEST ("v4=5*v3",
        ((v4=5*v3), (v4.get(0)==5 && v4.get(1)==10 && v4.get(2)==15)), TRUE);
  TEST ("v3*=5",((v3*=5), (v3== v4)), TRUE);
  TEST ("v4=v3/5",
        ((v4=v3/5), (v4.get(0)==1 && v4.get(1)==2 && v4.get(2)==3)), TRUE);
  TEST ("v3/=5", ((v3/=5), (v3==v4)), TRUE);

  //// additional tests
  CoolM_Vector<int> v(4,4,0,-2,2,0);
  v0 = v; v1 = v; v2 = v;
  TEST ("v(i)", 
        (v(0)==0 && v(1)==-2 && v(2)==2 && v(3)==0), TRUE);
  TEST ("v.abs()",
        ((v1 = v.abs()), 
         (v1(0)==0 && v1(1)==2 && v1(2)==2 && v1(3)==0)), TRUE);
  TEST ("v.sign()",
        ((v1 = v.sign()),
         (v1(0)==0 && v1(1)==-1 && v1(2)==1 && v1(3)==0)), TRUE);
  TEST ("element_product(v,v)",
        ((v1 = element_product(v,v)),
         (v1(0)==0 && v1(1)==4 && v1(2)==4 && v1(3)==0)), TRUE);
  TEST ("element_quotient(v,[2])",
        ((v2 = 2),
         (v1 = element_quotient(v,v2)),
         (v1(0)==0 && v1(1)==-1 && v1(2)==1 && v1(3)==0)), TRUE);
  TEST ("v.update(v.abs())",
        ((v1 = v.abs()),
         (v2.update(v1)), 
         (v2==v1)), TRUE);
  TEST ("v.extract(1,3)",
        ((v1 = v.extract(1,3)),
         (v1.length()==1 && v1(0)==v(3))), TRUE);
  TEST ("v.update([4],3)",
        ((v1=4),
         (v.update(v1,3)),
         (v(0)==0 && v(1)==-2 && v(2)==2 && v(3)==4)), TRUE);
  
  {                                             // new scope to reuse variables
    CoolM_Vector<int> v (4,4,1,0,0,0);
    TEST ("v(i)", 
          (v(0)==v.x() && v.x()==1 &&
           v(1)==v.y() && v.y()==0 &&
           v(2)==v.z() && v.z()==0 &&
           v(3)==v.t() && v.t()==0), TRUE);
    CoolM_Vector<int> v1(3,3,1,0,0);
    CoolM_Vector<int> v2(3,3,0,1,0);
    CoolM_Vector<int> v3(3,3,0,0,1);
    TEST ("dot_product(v1,v2)",
          (dot_product(v1,v2)==0 && dot_product(v1,v3)==0 && dot_product(v2,v3)==0), TRUE);
    TEST ("4d-v=3d-v", 
          ((v = v3), v.length()==3 && v==v3), TRUE);
    TEST ("cross_3d(v1,v2)", (v=cross_3d(v1,v2), v == v3), TRUE);
    TEST ("cross_3d(v2,v3)", (v=cross_3d(v2,v3), v == v1), TRUE);
    TEST ("cross_3d(v1,v3)", (v=cross_3d(v1,v3), v == -v2), TRUE);
    CoolM_Vector<int> vv(2,0);
    v1 = vv; v1.x()=1;
    v2 = vv; v2.y()=1;
    TEST ("cross_2d(v1,v2)", cross_2d(v1,v2)==1, TRUE);
  }

  {
    CoolM_Vector<int> v (4,4,1,0,0,0);
    TEST ("v.squared_magnitude",
          (v.squared_magnitude()==1), TRUE);
    TEST ("v.magnitude",
          (v.magnitude()==1), TRUE);
    TEST ("v.normalize",
          ((v1 = 3 * v),
           (v1.normalize()), v1 == v), TRUE);
  }

}

void test_matrix() {                            // product with matrices
  CoolMatrix<int> m(2,3,6,
                    1,2,3,
                    4,5,6);
  CoolM_Vector<int> v, v2(2,2,1,0), v3(3,3,1,0,0);
  TEST ("v.pre_multiply(m)",
        ((v = v3), 
         (v.pre_multiply(m)), 
         (v.length()==2 && v(0)==1 && v(1)==4)), TRUE);
  TEST ("v.post_multiply(m)",
        ((v = v2), 
         (v.post_multiply(m)), 
         (v.length()==3 && v(0)==1 && v(1)==2 && v(2)==3)), TRUE);
  TEST ("v*=m",
        ((v = v2),
         (v *= m),
         (v.length()==3 && v(0)==1 && v(1)==2 && v(2)==3)), TRUE);
  TEST ("v2*m",
        ((v = v2 * m),
         (v.length()==3 && v(0)==1 && v(1)==2 && v(2)==3)), TRUE);
  TEST ("m*v3",
        ((v = m * v3),
         (v.length()==2 && v(0)==1 && v(1)==4)), TRUE);
}

void test_leak() {                              // use top4.1 to watch for memory
  for (;;) {                                    // remember to kill process.
    test_int ();
    test_matrix ();
  }
}
   
int main (void) {
  START("CoolM_Vector");
  test_int ();
  test_matrix ();
#if LEAK
  test_leak ();
#endif
  SUMMARY();
  return 0;
}
