// #pragma on (dump_parse)
// #pragma on ( dump_tokens );

struct X
{   X( X& );
    X( int );
//  ~X();
};

struct Y
{
    X x;
    Y( int );
    Y();
    int foo();
    operator int();
};


Y __far yf;
Y __far yf3[3];

Y __near yn;
Y __near yn3[3];


void fun( Y __far & y_far, Y __near & y_near )
{
    Y zn( y_far  );
    Y zf( y_near );

    zn = y_far;
    zn = y_near;
    y_far = y_near;

    delete &y_far;
    delete &y_near;

    y_far.foo();
    y_near.foo();
}
