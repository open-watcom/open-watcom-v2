struct S {
    operator = ( S const & ) const;
};
struct T : public S {
};
/*
    It is possible to attribute meaning to both 'const' and 'volatile'
    this qualifiers.  In the PC world, 'near' and 'far' become very
    important to propagate.
*/
void g( T a, T const b ) {
    /* should this work? */
    b = a;
}
