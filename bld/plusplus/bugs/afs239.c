class CI_B {
    public:
    CI_B( int i ) { _i = i; };
    virtual ~CI_B() { _i = 0; };
    int _i;
};

class CI_D : public CI_B
{
    public:
    CI_D( int i ) : CI_B(i) { _ii = i; };
    virtual ~CI_D() { _ii=0; };
    int _ii;
};

class DI_B {
    public:
    DI_B( int i ) { _i = i; };
    virtual ~DI_B() { _i = 0; };
    int _i;
};

class DI_D : public DI_B
{
    public:
    DI_D( int i ) : DI_B(i) { _ii = i; };
    virtual ~DI_D() { _ii=0; };
    int _ii;
};

// (1) copy ctor for CI_D and CI_B are left in the .obj file even
//     though they are not req'd in this module
void copy_initialization() {
    CI_D v = 12;
}

void direct_initialization() {
    DI_D v( 12 );
}
