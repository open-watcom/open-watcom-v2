typedef void * P;
typedef void const* Pc;
typedef void volatile * Pv;
typedef void const volatile * Pvc;

#define cast( t, v, s ) t v = const_cast<t>( s );

void poo
    ( P p1
    , Pc p2
    , Pv p3
    , Pvc p4 )
{
    cast( P,   v01, p1 );
    cast( Pc,  v02, p1 );
    cast( Pv,  v03, p1 );
    cast( Pvc, v04, p1 );
    cast( P,   v05, p2 );
    cast( Pc,  v06, p2 );
    cast( Pv,  v07, p2 );
    cast( Pvc, v08, p2 );
    cast( P,   v09, p3 );
    cast( Pc,  v00, p3 );
    cast( Pv,  v11, p3 );
    cast( Pvc, v12, p3 );
    cast( P,   v13, p4 );
    cast( Pc,  v14, p4 );
    cast( Pv,  v15, p4 );
    cast( Pvc, v16, p4 );

    this_is_the_only_error;
}
