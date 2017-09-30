typedef void __near *npv;
struct X {
    npv data;
    npv foo();
    npv bar();
};
npv X::foo() {
    return( this ==  0  ?  0  : data );
}
npv X::bar() {
    if( this == 0 ) {
        return 0;
    }
    return( data );
}

#error This should be only message
