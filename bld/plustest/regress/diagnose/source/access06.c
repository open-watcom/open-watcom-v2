// page 205 ARM
struct A1 {
    void f( void );
};

struct A2 : virtual A1 {
    void f( void );
};

struct A3 : A2 {
};

struct A4 : A3, virtual A1 {
};

void foo( A2 *pA2, A4 *pA4 )
{
    pA2->f();		// OK; call A2::f
    pA4->f();		// OK; call A2::f
}
