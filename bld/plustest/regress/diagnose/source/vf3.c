struct A {
    int a1,a2;
};

struct W {
    int w1,w2;
};

struct V {
    int v1,v2;
};

struct B : W, virtual V {
    int b1,b2;
    B();
};

struct Q {
    int q1,q2;
};

struct P : Q, virtual B {
    int p1,p2;
    P();
};

struct R {
    int r1,r2;
};

struct S : virtual A, Q {
    int s1,s2;
    S();
};

struct C : R, P, virtual A, virtual B, S {
    int c1,c2;
    C();
};

//#pragma on(dump_ptree);
B::B(){}
P::P(){}
S::S(){}
C::C(){}
