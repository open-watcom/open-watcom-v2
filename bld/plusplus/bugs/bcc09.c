// borland C++ doesn't get all the required ctors called
// - GRB
class A {
    public:
        char a;
    A();
    A( int );
};

A a[3][2] = { 1 };
/******************
storage allocated:
{ 1,2,3,4,5,6 }
ctors called:
{ 1,2, , , ,  }
******************/

A b[3][2] = { { 1 }, { 3 } };
/******************
storage allocated:
{ 1,2,3,4,5,6 }
ctors called:
{ 1,2,3,4, ,  }
******************/
