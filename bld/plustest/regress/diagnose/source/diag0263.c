
struct X {
    char data[16];
};

char foo( X *p, char str[16] ) {
    if( str != 0 ) {            // this is ok
        return str[1];
    }
    if( p->data != 0 ) {        // this is not ok
        return p->data[2];
    }
    return 'a';
}

#error This should be the only error
