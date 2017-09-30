extern "C" { static int i = 1; };
extern "C" { static void foo() { ++i; } };

extern "C++" {
    static int i;
    static void foo();
};

void main()
{
    foo();
}

/* syntax errors */
struct S {
    S() : 1 {};
    S(int) : 1U {};
    S(char) : 1.0d1 {};
    S(float) : 1.0f1 {};
}
