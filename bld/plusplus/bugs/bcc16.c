// we correctly diganose this as ambiguous (see ARM, pg275)
// every other compiler chooses the second option
// GRB
struct A {
    operator int ();
    A operator + ( A & );
};
void f( A a, A b )
{
    a + b;      // A::operator int( a ) + A::operator int( a )
                // or a.A::operator + ( b );
}
