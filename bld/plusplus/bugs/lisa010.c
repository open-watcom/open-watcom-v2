void foo( short & );
void bar( void ) {
    short a;
    foo( (short)a ); // cast yields an rvalue because short is not a reference type
}
// wpp386 b.cpp
// b.cpp(4): Error! E333: col(16) cannot convert argument to type specified in function prototype
// b.cpp(4): Note! N630: col(16) source conversion type is "short"
// b.cpp(4): Note! N631: col(16) target conversion type is "short (lvalue)"
