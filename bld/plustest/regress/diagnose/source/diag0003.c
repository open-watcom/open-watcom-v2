/*
  base class checks
*/
union A {
};

struct B : A {
};

struct C {
};

union D : C {
};

struct E : E {
};
