// from Greg Bentz
// bad error message
// Status: unfixed
struct X {
    void bar( int );
    void foo( void (X::*)( int ) );
    void g( void );
};

void X::g( void ) {
    foo( &bar );
}
// wpp a.cpp
// a.cpp(8): Error! E473: col(8) function argument(s) do not match those in prototype
// a.cpp(8): Note! N392: col(8) 'void X::foo( void (X::* )( int ))' defined in: a.cpp(3) (col 10)
// a.cpp(8): Note! N643: col(8) cannot convert argument 1 defined in: a.cpp(8) (col 10)
// a.cpp(8): Note! N630: col(8) source conversion type is "void (X::* )( int )"
// a.cpp(8): Note! N631: col(8) target conversion type is "void (X::* )( int )"
