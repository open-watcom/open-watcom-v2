struct V1 {
    virtual void foo( int, int );
    virtual void bar( int, int );
    int v1,v2;
};
struct V2 {
    virtual void foo( int, int );
    virtual void bar( int, int );
    int v1,v2;
};
struct D : virtual V1, virtual V2 {
};

void foo( D *p )
{
    for( int i = 0; i < 10; ++i ) {
        V1 *q = p;                      // assignment to 'q' should be brought
                                        // outside the loop
        q->bar( p->V1::v1 + p->V2::v1, p->V1::v2 + p->V2::v2 );
    }
}
