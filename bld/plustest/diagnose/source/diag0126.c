double fn( double );
char fn( char );

void bad_map( const double (*f)( double ));
typedef double math_fn_t( double );
void ok_map( math_fn_t const *f );

void foo()
{
    bad_map( fn );
    ok_map( fn );
}

// linkage lasts as long as the definition!
extern "C" void x15()
{
    extern void x17();
    extern int x18[];
}
extern "C" void x17()
{
}
extern "C" int x18[] = { 1, 2 };

extern "C" void x25();
void x25()
{
    extern void x28();
    extern int x29[];
}
extern void x31();
extern "C++" void x31()
{
}
extern "C" void x28()
{
}
extern "C" int x29[] = { 1, 2 };

struct S {
    ~S();
    S();
};

S::~S() {
}

S::S() {
}

struct T {
    ~T();
    T();
};

inline T::~T() {
}

inline T::T() {
}

struct B {
    ~B();
    B();
};
struct D : B {
    ~D(){}
    D(){}
};
inline B::~B() {
}
inline B::B() {
}

struct DA {
    DA(int=0);
};
DA::DA(int)
{
}

struct X56 {
    X56();
};

X56 (*x60())()
{
   return ( X56() ) 0;
}
