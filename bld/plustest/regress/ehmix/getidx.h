void *getidx( void const *p )
{
    static unsigned long count;
    struct X {
        X*left;
        X*right;
        void const *p;
        void *i;
    };
    static X *root;
    X *c;
    X **h;

    h = &root;
    c = root;
    for(;;) {
        if( c == NULL ) break;
        if( c->p < p ) {
            h = &(c->left);
        } else if( c->p > p ) {
            h = &(c->right);
        } else {
            return c->i;
        }
        c = *h;
    }
    c = new X;
    c->left = NULL;
    c->right = NULL;
    c->p = p;
    c->i = (void*) ++count;
    *h = c;
    return c->i;
}
