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

struct Z
{
    short a[32];
};


Y __far yf;
Y __far yf3[3];

Y __near yn;
Y __near yn3[3];

Z __huge HugeArr[3];
Z __huge *HugeMem;

Z __far FarArr[3];
Z __far *FarMem;

void fun( Y __far & y_far, Y __near & y_near )
{
    Y zn( y_far  );
    Y zf( y_near );
    Z localvar;
    int i = 1;

    zn = y_far;
    zn = y_near;
    y_far = y_near;

    delete &y_far;
    delete &y_near;

    y_far.foo();
    y_near.foo();

    HugeArr[i]=localvar;
    HugeMem[i]=localvar;
    FarArr[i]=localvar;
    FarMem[i]=localvar;
    localvar=HugeArr[i];
    localvar=HugeMem[i];
    localvar=FarArr[i];
    localvar=FarMem[i];
}
