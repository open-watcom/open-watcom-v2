/*
  more base class checks
*/
struct A { };

struct B : virtual A { };

struct C { };

struct D : virtual C { };

struct X1 : A, A { };

struct X2 : B, A { };

struct X3 : B, virtual B { };

struct X3 : C, B { };

struct Z : Z { };
