// No preference is given to user-defined conversion by constructor
//      or conversion function or vice versa
class B;
class A {
    public:
        A( B );
};
class B {
    public:
        operator A();
};
void f( A );
void g( B b )
{
    f( b );     // ambiguous
                // f( A( b ) ) - constructor
                // f( A( b ) ) - conversion function
}

#error at least one error
