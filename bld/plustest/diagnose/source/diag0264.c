
struct X {};
const X *bar1( X *f, const X *p ) {
    if( f != 0 ) {
        return f;
    }
    return p;
}
const X *bar2( X *f, const X *p ) {
    return (f!=0) ? f : p;
}

#error This should be the only error
