#if !defined( ARCH ) || ( ARCH != 386 )
    #error system dependent test for 386
#else

#error begin 10.0 specific diagnostics
void one_cpp( int, int );
#pragma aux one_cpp parm reverse;

void two_cpps( int, int );
void two_cpps( char, char );
#pragma aux two_cpps parm reverse;

void one_cpp_one_c( int, int );
extern "C" void one_cpp_one_c( char, char );
#pragma aux one_cpp_one_c parm reverse;

void two_cpps_one_c( int, int );
void two_cpps_one_c( char, char );
extern "C" void two_cpps_one_c( char, int );
#pragma aux two_cpps_one_c parm reverse;

template <class T>
    void many_cpp( T, T );
#pragma aux many_cpp parm reverse;

extern "C" void template_one_c( char, char );
template <class T>
    void template_one_c( T, T );
#pragma aux template_one_c parm reverse;
#error end 10.0 specific diagnostics

typedef struct S {
    int a;
    char b[];
} S;

struct T : S {
    int a;
};
struct B {
    int b;
};
struct D : B {
    int d;
    char a[];
};
struct X1 {
    char a[];
    char b[];
};
struct X2 {
    char a[];
    char b;
};
struct X3 {
    char a[];
    char b : 1;
};
struct X4 {
    char a[];
    virtual void foo();
    void ack();
};
struct X5 {
    char a[];
    static int x;
};
struct X6 {
    char a[];
    static int x[];
    static int y[];
};
struct X7 {
    char a[];
    static int foo();
};

#endif
