struct S {
    int s;
    S & operator =( S & );
};

struct C {
    S a[2][3][4];
    int b[2][3][4];
    int c;
};

void foo( C *a, C *b )
{
    *a = *b;
}
