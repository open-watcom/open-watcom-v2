struct B {
};
typedef void (B::* mpf)();

struct D : B {
    void foo( void * );
};

void ack( mpf );

void use()
{
    // warn if member-pointer cast changes both basis class
    // and object type at the same time (user can break cast
    // up into two casts but this catches some bad errors)
    ack( (mpf) &D::foo );
}
