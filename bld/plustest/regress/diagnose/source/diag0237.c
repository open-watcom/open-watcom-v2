struct S;
typedef int (S::* mp0)();
typedef int (S::* mp1)(int);
typedef int (S::* mp2)(int,int);
typedef int (S::* mpc)(char);

struct S {
    int one();
    int two();
    int two(int);
    int three();
    int three(int);
    int three(int,int);
    int mixed();
    int mixed(int);
    int mixed(int,int);
    static int mixed(char);
    static mp1 a[];
};

mp1 S::a[] = { two, three };

mp0 x0 = &S::one;
mp1 x1 = &S::two;
mp2 x2 = &S::three;

void foo( mp0 );
void foo( mp1 );
void foo( mp2 );

void foo() {
    foo( &S::one );
    foo( &S::two );
    foo( &S::three );
}
