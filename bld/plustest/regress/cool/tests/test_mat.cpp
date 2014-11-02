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

#include <math.h>                               // for fabs
#include <cool/Matrix.h>
#include <cool/Matrix.C>
#include <cool/test.h>

void test_int () {
  int result = 0;
  CoolMatrix<int> m0(2,2);
  TEST ("CoolMatrix<int> m0(2,2)", (m0.rows()==2 && m0.columns()==2), TRUE);
  CoolMatrix<int> m1(3,4);
  TEST ("CoolMatrix<int> m1(3,4)", (m1.rows()==3 && m1.columns()==4), TRUE);
  CoolMatrix<int> m2(2,2,2);
  TEST ("CoolMatrix<int> m2(2,2,2)",
        (m2.get(0,0)==2 && m2.get(0,1)==2 && m2.get(1,0)==2 && m2.get(1,1)==2), TRUE);
  TEST ("m0=2", 
        (m0=2,
         (m0.get(0,0)==2 && m0.get(0,1)==2 && m0.get(1,0)==2 && m0.get(1,1)==2)), TRUE);
  TEST ("m0 == m2", (m0 == m2), TRUE);
  TEST ("(m0 == m2)", (m0 == m2), TRUE);
  TEST ("m2.put(1,1,3)", (m2.put(1,1,3),m2.get(1,1)), 3);
  TEST ("m2.get(1,1)", m2.get(1,1), 3);
  TEST ("m0 == m2", (m0 == m2), FALSE);
  TEST ("m0 != m2", (m0 != m2), TRUE);
  TEST ("(m0 == m2)", (m0 == m2), FALSE);
  TEST ("m1.fill(3)",
        (m1.fill(3),
         (m1.get(0,0)==3 && m1.get(1,1)==3 && m1.get(2,2)==3 && m1.get(2,3)==3)), TRUE);
  TEST ("m2.fill(2)",
        (m2.fill(2),
         (m2.get(0,0)==2 && m2.get(0,1)==2 && m2.get(1,0)==2 && m2.get(1,1)==2)), TRUE);
  CoolMatrix<int> m3(1,3,3,1,2,3);
  TEST ("m3(1,3,3,1,2,3)",(m3.get(0,0)==1 && m3.get(0,1)==2 && m3.get(0,2)==3), TRUE);
  CoolMatrix<int> m4(m3);
  TEST ("CoolMatrix<int> m4(m3)", (m3==m4), TRUE);
  TEST ("m0=m2", (m0=m2, (m0==m2)), TRUE);

  /// test additions and substractions
  TEST ("m0=m2+3",
        ((m0=m2+3),
         (m0.get(0,0)==5 && m0.get(0,1)==5 && m0.get(1,0)==5 && m0.get(1,1)==5)), TRUE);
  TEST ("m0=3+m2",
        ((m0=3+m2),
         (m0.get(0,0)==5 && m0.get(0,1)==5 && m0.get(1,0)==5 && m0.get(1,1)==5)), TRUE);
  TEST ("m0+=(-3)",
        (m0+=(-3),
         (m0.get(0,0)==2 && m0.get(0,1)==2 && m0.get(1,0)==2 && m0.get(1,1)==2)), TRUE);
  TEST ("m0-=(-3)",
        (m0-=(-3),
         (m0.get(0,0)==5 && m0.get(0,1)==5 && m0.get(1,0)==5 && m0.get(1,1)==5)), TRUE);
  TEST ("m0=m2-3",
        ((m0=m2-3),
         (m0.get(0,0)==-1 && m0.get(0,1)==-1 && m0.get(1,0)==-1 && m0.get(1,1)==-1)), TRUE);
  TEST ("m0=3-m2",
        ((m0=3-m2),
         (m0.get(0,0)==1 && m0.get(0,1)==1 && m0.get(1,0)==1 && m0.get(1,1)==1)), TRUE);
  TEST ("m0= -m2",
        (m0= -m2,
         (m0.get(0,0)==-2 && m0.get(0,1)==-2 && m0.get(1,0)==-2 && m0.get(1,1)==-2)), TRUE);

  CoolMatrix<int> m5(2,2);
  m0 = m2;
  TEST ("m5=m0+m2",
        ((m5=m0+m2),
         (m5.get(0,0)==4 && m5.get(0,1)==4 && m5.get(1,0)==4 && m5.get(1,1)==4)), TRUE);
  TEST ("m5=m0-m2",
        ((m5=m0-m2),
         (m5.get(0,0)==0 && m5.get(0,1)==0 && m5.get(1,0)==0 && m5.get(1,1)==0)), TRUE);
  TEST ("m0+=m2", 
        ((m0+=m2),
         (m0.get(0,0)==4 && m0.get(0,1)==4 && m0.get(1,0)==4 && m0.get(1,1)==4)), TRUE);
  TEST ("m0-=m2", 
        ((m0-=m2),
         (m0.get(0,0)==2 && m0.get(0,1)==2 && m0.get(1,0)==2 && m0.get(1,1)==2)), TRUE);

  /// test multiplications and divisions
  TEST ("m4=m3*5",
        ((m4=m3*5),
         (m4.get(0,0)==5 && m4.get(0,1)==10 && m4.get(0,2)==15)), TRUE);
  TEST ("m4=5*m3",
        ((m4=5*m3),
         (m4.get(0,0)==5 && m4.get(0,1)==10 && m4.get(0,2)==15)), TRUE);
  TEST ("m3*=5",((m3*=5), (m3== m4)), TRUE);
  TEST ("m4=m3/5",
        ((m4=m3/5),
         (m4.get(0,0)==1 && m4.get(0,1)==2 && m4.get(0,2)==3)), TRUE);
  TEST ("m3/=5", ((m3/=5), (m3==m4)), TRUE);
  

  CoolMatrix<int> m6(2,2,4,1,2,3,4);
  TEST ("CoolMatrix<int> m6(2,2,4,1,2,3,4)", m6.get(1,1), 4);
  CoolMatrix<int> m7(2,2,4,5,6,7,8);
  TEST ("CoolMatrix<int> m7(2,2,4,5,6,7,8)", m7.get(1,1), 8);
  TEST ("m5=m6*m7", 
        ((m5=m6*m7),
         (m5.get(0,0)==19 && m5.get(0,1)==22 && m5.get(1,0)==43 && m5.get(1,1)==50)), TRUE);
  TEST ("m6*=m7", 
        ((m6*=m7),
         (m6.get(0,0)==19 && m6.get(0,1)==22 && m6.get(1,0)==43 && m6.get(1,1)==50)), TRUE);
  CoolMatrix<int> c0(2,1,2,1,0);
  CoolMatrix<int> c1;
  TEST ("c1=m6*c0",
        ((c1=m6*c0), 
         c1.rows()==c0.rows() && c1.columns()==c0.columns() && 
         c1.get(0,0)==19 && c1.get(1,0)==43), TRUE);
  CoolMatrix<int> r0(1,2,2,1,0);
  CoolMatrix<int> r1;
  TEST ("r1=r0*m6",
        ((r1=r0*m6), 
         r1.rows()==r0.rows() && r1.columns()==r0.columns() && 
         r1.get(0,0)==19 && r1.get(0,1)==22), TRUE);
  TEST ("r0*=m6",
        ((r0*=m6), r0==r1), TRUE);
  TEST ("m6*=c0",
        ((m6*=c0), c1==m6), TRUE);
  

  /// additional tests
  CoolMatrix<int> m(2,2,4,0,-2,2,0);
  m0 = m; m1 = m;
  TEST ("m(i,j)", 
        (m(0,0)==0 && m(0,1)==-2 && m(1,0)==2 && m(1,1)==0), TRUE);
  TEST ("m.transpose()",
        ((m1 = m.transpose()), 
         (m1(0,0)==0 && m1(0,1)==2 && m1(1,0)==-2 && m1(1,1)==0)), TRUE);
  TEST ("m.abs()",
        ((m1 = m.abs()), 
         (m1(0,0)==0 && m1(0,1)==2 && m1(1,0)==2 && m1(1,1)==0)), TRUE);
  TEST ("m.sign()",
        ((m1 = m.sign()),
         (m1(0,0)==0 && m1(0,1)==-1 && m1(1,0)==1 && m1(1,1)==0)), TRUE);
  TEST ("element_product(m,m)",
        ((m1 = element_product(m,m)),
         (m1(0,0)==0 && m1(0,1)==4 && m1(1,0)==4 && m1(1,1)==0)), TRUE);
  TEST ("element_quotient(m,[2])",
        ((m2 = 2),
         (m1 = element_quotient(m,m2)),
         (m1(0,0)==0 && m1(0,1)==-1 && m1(1,0)==1 && m1(1,1)==0)), TRUE);
  TEST ("m.update(m.abs())",
        ((m1 = m.abs()),
         (m2.update(m1)), 
         (m2==m1)), TRUE);
  TEST ("m.extract(1,1,1,1)",
        ((m1 = m.extract(1,1,1,1)),
         (m1.rows()==1 && m1.columns()==1 && m1(0,0)==m(1,1))), TRUE);
  TEST ("m.update([4],1,1)",
        ((m1=4),
         (m.update(m1,1,1)),
         (m(0,0)==0 && m(0,1)==-2 && m(1,0)==2 && m(1,1)==4)), TRUE);

  CoolMatrix<int> v (4,1,4,1,0,0,0);
  TEST ("v(i)", 
        (v(0)==v(0,0) && v(0)==v.x() && v.x()==1 &&
         v(1)==v(1,0) && v(1)==v.y() && v.y()==0 &&
         v(2)==v(2,0) && v(2)==v.z() && v.z()==0 &&
         v(3)==v(3,0) && v(3)==v.t() && v.t()==0), TRUE);
  CoolMatrix<int> v1(3,1,3,1,0,0);
  CoolMatrix<int> v2(3,1,3,0,1,0);
  CoolMatrix<int> v3(3,1,3,0,0,1);
  TEST ("dot_product(v1,v2)",
        (dot_product(v1,v2)==0 && dot_product(v1,v3)==0 && dot_product(v2,v3)==0), TRUE);
  v = v3;
  TEST ("4d-v=3d-v", (v.rows()==3 && v.columns()==1 && v==v3), TRUE);
  TEST ("cross_3d(v1,v2)", (v=cross_3d(v1,v2), v == v3), TRUE);
  TEST ("cross_3d(v2,v3)", (v=cross_3d(v2,v3), v == v1), TRUE);
  TEST ("cross_3d(v1,v3)", (v=cross_3d(v1,v3), v == -v2), TRUE);
  CoolMatrix<int> vv(2,1,0);
  v1 = vv; v1.x()=1;
  v2 = vv; v2.y()=1;
  TEST ("cross_2d(v1,v2)", cross_2d(v1,v2)==1, TRUE);
}


void test_determinant () {
  CoolMatrix<int> m1(2,2,4,1,2,3,4);
  TEST ("m1.determinant", m1.determinant()==-2, TRUE);
  CoolMatrix<int> m2(2,2,4,5,6,7,8);
  TEST ("m2.determinant", m2.determinant()==-2, TRUE);
  TEST ("(m1*m2).determinant", 
        (m1*m2).determinant()==(m1.determinant()*m2.determinant()), TRUE);
  CoolMatrix<int> d0(4,4,16, 1,1,1,1, 1,2,2,2, 1,2,3,3, 1,2,3,4);
  TEST ("d0.determinant", d0.determinant()==7, TRUE);
  CoolMatrix<int> d1(4,4,16, 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);
  TEST ("d1.determinant", d1.determinant()==1, TRUE);
  CoolMatrix<int> d2(4,4,16, 1,2,3,4, 5,6,7,8, 9,10,11,12, 13,14,15,16);
  TEST ("d2.determinant", d2.determinant()==0, TRUE);
}

Boolean Matrix_double_is_data_equal (const double& d1, const double& d2) {
  return ((fabs(d1 - d2) < 1.0e-12) ? TRUE : FALSE);
}

void test_double () {
  double result = 0.0;
  CoolMatrix<double> d0(2,2);
  TEST ("CoolMatrix<double> d0(2,2)", (d0.rows()==2 && d0.columns()==2), TRUE);
  CoolMatrix<double> d1(3,4);
  TEST ("CoolMatrix<double> d1(3,4)", (d1.rows()==3 && d1.columns()==4), TRUE);
  CoolMatrix<double> d2(2,2,2.0);
  TEST ("CoolMatrix<double> d2(2,2,2.0)",
        (d2.get(0,0)==2.0 && d2.get(0,1)==2.0 && d2.get(1,0)==2.0 && d2.get(1,1)==2.0), TRUE);
  TEST ("d0=2.0", (d0=2.0,
                   (d0.get(0,0)==2.0 && d0.get(0,1)==2.0 && d0.get(1,0)==2.0 && d0.get(1,1)==2.0)), TRUE);
  TEST ("d0.set_compare", 
        (d0.set_compare(&Matrix_double_is_data_equal), TRUE), TRUE);
  TEST ("d0 == d2", (d0 == d2), TRUE);
  TEST ("(d0 == d2)", (d0==d2), TRUE);
  TEST ("d2.put(1,1,3.0)", (d2.put(1,1,3.0),d2.get(1,1)), 3.0);
  TEST ("d2.get(1,1)", d2.get(1,1), 3.0);
  TEST ("d0 == d2", (d0 == d2), FALSE);
  TEST ("d0 != d2", (d0 != d2), TRUE);
  TEST ("(d0 == d2)", (d0==d2), FALSE);
  TEST ("d1.fill(3.0)",
        (d1.fill(3.0),
         (d1.get(0,0)==3.0 && d1.get(1,1)==3.0 && d1.get(2,2)==3.0 && d1.get(2,3)==3.0)), TRUE);
  TEST ("d2.fill(3.0)",
        (d2.fill(2.0),
         (d2.get(0,0)==2.0 && d2.get(0,1)==2.0 && d2.get(1,0)==2.0 && d2.get(1,1)==2.0)), TRUE);
  CoolMatrix<double> d3(1,3,3,1.0,2.0,3.0);
  TEST ("d3(1,3,3,1.0,2.0,3.0)",(d3.get(0,0)==1.0 && d3.get(0,1)==2.0 && d3.get(0,2)==3.0), TRUE);
  CoolMatrix<double> d4(d3);
  TEST ("CoolMatrix<double> d4(d3)", (d3 == d4), TRUE);
  TEST ("d0=d2", (d0=d2,  (d0==d2)), TRUE);
  TEST ("d0=d2+3.0",
        ((d0=d2+3.0),
         (d0.get(0,0)==5.0 && d0.get(0,1)==5.0 && d0.get(1,0)==5.0 && d0.get(1,1)==5.0)), TRUE);
  TEST ("d0+=(-3.0)",
        (d0+=(-3.0),
         (d0.get(0,0)==2.0 && d0.get(0,1)==2.0 && d0.get(1,0)==2.0 && d0.get(1,1)==2.0)), TRUE);
  CoolMatrix<double> d5(2,2);
  TEST ("d5=d0+d2",
        ((d5=d0+d2),
         (d5.get(0,0)==4.0 && d5.get(0,1)==4.0 && d5.get(1,0)==4.0 && d5.get(1,1)==4.0)), TRUE);
  TEST ("d0+=d2", 
        ((d0+=d2),
         (d0.get(0,0)==4.0 && d0.get(0,1)==4.0 && d0.get(1,0)==4.0 && d0.get(1,1)==4.0)), TRUE);
  TEST ("d4=d3*5.0",((d4=d3*5.0),(d4.get(0,0)==5.0 && d4.get(0,1)==10.0 && d4.get(0,2)==15.0)), TRUE);
  TEST ("d3*=5.0",((d3*=5.0),  (d3== d4)), TRUE);
  CoolMatrix<double> d6(2,2,4,1.0,2.0,3.0,4.0);
  TEST ("CoolMatrix<double> d6(2,2,4,1.0,2.0,3.0,4.0)", d6.get(1,1), 4.0);
  CoolMatrix<double> d7(2,2,4,5.0,6.0,7.0,8.0);
  TEST ("CoolMatrix<double> d7(2,2,4,5.0,6.0,7.0,8.0)", d7.get(1,1), 8.0);
  TEST ("d5=d6*d7", ((d5=d6*d7),
                     (d5.get(0,0)==19.0 && d5.get(0,1)==22.0 && d5.get(1,0)==43.0 && d5.get(1,1)==50.0)), TRUE);
  TEST ("d6*=d7", ((d6*=d7),
                   (d6.get(0,0)==19.0 && d6.get(0,1)==22.0 && d6.get(1,0)==43.0 && d6.get(1,1)==50.0)), TRUE); 
  cout << d6 << '=' << &d6 << endl;
  TEST ("<<", TRUE, TRUE);
}

void test_envelope () {
  CoolMatrix<int> m0(2,2,0), m1(m0), m2(m0), m3(m0), m4(m0);
  m1 = 1; m2 = 2; m3 = 3; m4 = 4;
  m0 = m1 + m2 + m3 + m4;
  TEST ("operator+", (m0(0,0)==10 && m0(0,1)==10 && m0(1,0)==10 && m0(1,1)==10),
        TRUE);
  m0 = m1 * m2 * m3 * m4;
  TEST ("operator*", (m0(0,0)==192 && m0(0,1)==192 && m0(1,0)==192 && m0(1,1)==192),
        TRUE);
}

void test_conversion () {
  int i, j, d;
  Boolean check;
  {
    // convert from a CoolMatrix to a block array:
    CoolMatrix<int> m1 (4,3,12, 1,2,3, 4,5,6, 7,8,9, 10,11,12);
    const int* data = m1.data_block();
    {
      check = TRUE;
      for (d = 0; d < 12; d++)
        if (data[d] != d+1)
          check = FALSE;
    }
    TEST ("(const int*) m.data_block", check, TRUE);
    
    typedef int block [4][3];
    block& m2 = *((block*) data);
    {
      check = TRUE;
      for (i = 0; i < 4; i++) 
        for (j = 0; j < 3; j++) 
          if (m1(i,j) != m2[i][j])
            check = FALSE;
    }
    TEST ("matrix(i,j)==block[i][j]", check, TRUE);
    
    // convert from a block array to a CoolMatrix:
    block b1;
    for (i=0; i<4; i++) 
      for (j=0; j<3; j++)
        b1[i][j] = i*3+j;
    data = ((const int*) b1);                   // & in ((const int*) &b1) 
    {                                           // is not needed.
      check = TRUE;
      for (d = 0; d < 12; d++)
        if (data[d] != d)
          check = FALSE;
    }
    TEST ("(const int*) block", check, TRUE);
    CoolMatrix<int> b2(4, 3, data);
    { 
      check = TRUE;
      for (i = 0; i < 4; i++) 
        for (j = 0; j < 3; j++) 
          if (b1[i][j] != b2(i,j))
            check = FALSE;
    }
    TEST ("block[i][j]==matrix(i,j)", check, TRUE);
  }
  {
    // convert from a CoolMatrix to a block array:
    CoolMatrix<double> m1 (4,3,12, 
                           1.0,2.0,3.0, 4.0,5.0,6.0, 
                           7.0,8.0,9.0, 10.0,11.0,12.0);
    const double* data = m1.data_block();
    {
      check = TRUE;
      for (d = 0; d < 12; d++)
        if (data[d] != d+1)
          check = FALSE;
    }
    TEST ("(const double*) m.data_block", check, TRUE);
    
    typedef double block [4][3];
    block& m2 = *((block*) data);
    {
      check = TRUE;
      for (i = 0; i < 4; i++) 
        for (j = 0; j < 3; j++) 
          if (m1(i,j) != m2[i][j])
            check = FALSE;
    }
    TEST ("matrix(i,j)==block[i][j]", check, TRUE);
    
    // convert from a block array to a CoolMatrix:
    block b1;
    for (i=0; i<4; i++) 
      for (j=0; j<3; j++)
        b1[i][j] = i*3+j;
    data = ((const double*) b1);                // & in ((const double*) &b1)
    {                                           // is not needed
      check = TRUE;
      for (d = 0; d < 12; d++)
        if (data[d] != d)
          check = FALSE;
    }
    TEST ("(const double*) block", check, TRUE);
    CoolMatrix<double> b2(4, 3, data);
    { 
      check = TRUE;
    for (i = 0; i < 4; i++) 
      for (j = 0; j < 3; j++) 
        if (b1[i][j] != b2(i,j))
          check = FALSE;
    }
    TEST ("block[i][j]==matrix(i,j)", check, TRUE);
  }
}

void test_leak () {                             // use top4.1 to watch memory usage.
  for (;;) {                                    // remember to kill process.
    test_int ();
    test_double ();
    test_envelope ();
    test_determinant ();
    test_conversion ();
  }
}

int main (void) {
  START("CoolMatrix");
  test_int ();
  test_double ();
  test_envelope ();
  test_determinant ();
  test_conversion ();
#if LEAK
  test_leak ();
#endif
  SUMMARY();
    return 0;
}
