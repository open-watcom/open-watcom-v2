// diagnose const_cast

struct B {
    int i;
    long l;
};

struct D : public B {
    float f;
};

int v;

long& l1 = const_cast<long&>( v );
int D::* mpd1 = const_cast< int D::* >( &B::i );
int const volatile D::* mpd2 = const_cast< int const D::* >( &B::i );
int const volatile D::* mpd3 = const_cast< int volatile D::* >( &B::i );
int const volatile B::* mpb4 = const_cast< int B::* >( &B::i ); // ok
int const volatile B::* mpb5 = const_cast< int B::* >( &B::l );


