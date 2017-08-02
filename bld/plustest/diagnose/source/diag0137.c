struct S {
    static int a;
};
int S::a = 2;
#error one error for DIAGNOSE test stream
