extern int test;

class A1 { A1(); };
class B1 { B1(); };
extern A1 a1;
extern B1 b1;
int i1 = ( a1 == b1 );
int t1 = test ? a1 : b1;

class A2 { A2(); };
class B2 : public A2 { B2(); };
class C2 : public A2 { C2(); };
class D2 : public B2, public C2 { D2(); };
extern A2 a2;
extern D2 d2;
int i2 = ( a2 == d2 );
int t2 = test ? a2 : d2;

class A3 { A3(); };
class B3 : private A3 { B3(); };
extern A3 a3;
extern B3 b3;
int i3 = ( a3 == b3 );
int t3 = test ? a3 : b3;

class A4 { A4(); };
class B4 : protected A4 { B4(); };
extern A4 a4;
extern B4 b4;
int i4 = ( a4 == b4 );
int t4 = test ? a4 : b4;
