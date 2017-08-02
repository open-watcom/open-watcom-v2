
// can only overload member functions where there is no static member function
// with the overloaded name

struct A {
//    void foo( int ) const;
    void foo( int ) const volatile;
    void foo( int );
    void foo( int ) volatile;
    void foo( int ) const volatile;
    void bar1( auto void (A::*mp )( int ) const );
};


void A::bar1( auto void (A::*mp)( int ) const );

void main()
{
    A a;
    a.bar1( &A::foo );
}
