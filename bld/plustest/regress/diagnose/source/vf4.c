struct D { int d1, d2; };
struct F { int f1, f2; virtual void foo( void ); };
struct V : D, F { int v1, v2; };
struct X : virtual V { int d1, d2; virtual void foo( void ); };
struct E { int e1, e2; };
struct H : E, X { int h1, h2; };
struct O { int o1, o2; };
struct I : O, virtual H { int i1, i2; virtual void bar(void); };
struct P { int p1, p2; virtual void sam( void ); };
struct R : P, I { int r1, r2; };

R x;
