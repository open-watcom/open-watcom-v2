struct Q {
    enum qq { a = 1, b = 2 };
};
int foo( Q::qq value ) {
    return( (int)value );
}
int bar( enum Q::qq value ) {
    return( (int)value );
}
// wpp386 c.cpp
// c.cpp(7): Error! E251: col(21) enum references must have an identifier
// c.cpp(7): Error! E498: col(21) syntax error before 'value'; probable cause: incorrectly spelled type name
