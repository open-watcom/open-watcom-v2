void a( int );
void a( int = 1, int = 2 );

void b( int = 1 );
void b( int = 1, int = 2 );

void c( int = 1, int = 2 );
void c( int = 1 );

void d( int = 1, int = 2 );
void d( int );

struct S {
    static const int a;
};

extern "C" {
    extern void e( int, int = 0 );
};

extern "C" f( int );
extern "C" f( int, int );

extern int g;
extern int h();

extern "C" {
    int g;
}

extern "C" int h()
{
    return g++;
}

extern "C" int va1;
extern int va1;

extern int va2;
extern "C" int va2;

extern "C" {
    extern int va3;
};
extern int va3;

extern int va4;
extern "C" {
    extern int va4;
};

extern "C" int va5;
extern "C++" int va5;

extern "C++" int va6;
extern "C" int va6;

extern "C" {
    extern int va7;
};
extern "C++" int va7;

extern "C++" int va8;
extern "C" {
    extern int va8;
};

extern "C" int va9;
extern "C++" int va9;

extern "C++" int va10;
extern "C" int va10;

extern "C" {
    extern int va11;
};
extern "C++" {
    extern int va11;
};

extern "C++" {
    extern int va12;
};
extern "C" {
    extern int va12;
};

extern "C" int dva1;
int dva1 = 1;

extern int dva2;
extern "C" int dva2 = 1;

extern "C" {
    extern int dva3;
};
extern int dva3 = 1;

extern int dva4;
extern "C" {
    extern int dva4 = 1;
};

extern "C" int dva5;
extern "C++" int dva5 = 1;

extern "C++" int dva6;
extern "C" int dva6 = 1;

extern "C" {
    extern int dva7;
};
extern "C++" int dva7 = 1;

extern "C++" int dva8;
extern "C" {
    extern int dva8 = 1;
};

extern "C" int dva9;
extern "C++" int dva9 = 1;

extern "C++" int dva10;
extern "C" int dva10 = 1;

extern "C" {
    extern int dva11;
};
extern "C++" {
    extern int dva11 = 1;
};

extern "C++" {
    extern int dva12;
};
extern "C" {
    extern int dva12 = 1;
};

extern "C" int a1();
int a1(){ return 1; };

extern int a2();
extern "C" int a2(){ return 1; };

extern "C" {
    extern int a3();
};
extern int a3(){ return 1; };

extern int a4();
extern "C" {
    extern int a4(){ return 1; };
};

extern "C" int a5();
extern "C++" int a5(){ return 1; };

extern "C++" int a6();
extern "C" int a6(){ return 1; };

extern "C" {
    extern int a7();
};
extern "C++" int a7(){ return 1; };

extern "C++" int a8();
extern "C" {
    extern int a8(){ return 1; };
};

extern "C" int a9();
extern "C++" int a9(){ return 1; };

extern "C++" int a10();
extern "C" int a10(){ return 1; };

extern "C" {
    extern int a11();
};
extern "C++" {
    extern int a11(){ return 1; };
};

extern "C++" {
    extern int a12();
};
extern "C" {
    extern int a12(){ return 1; };
};

extern "C" int da1();
int da1();

extern int da2();
extern "C" int da2();

extern "C" {
    extern int da3();
};
extern int da3();

extern int da4();
extern "C" {
    extern int da4();
};

extern "C" int da5();
extern "C++" int da5();

extern "C++" int da6();
extern "C" int da6();

extern "C" {
    extern int da7();
};
extern "C++" int da7();

extern "C++" int da8();
extern "C" {
    extern int da8();
};

extern "C" int da9();
extern "C++" int da9();

extern "C++" int da10();
extern "C" int da10();

extern "C" {
    extern int da11();
};
extern "C++" {
    extern int da11();
};

extern "C++" {
    extern int da12();
};
extern "C" {
    extern int da12();
};
