static union {
    int a;
    int b;
};

union {
    char *p;
    double d;
} static;

void foo( void )
{
    a = 1;
    d = a + 1;
    b += *p;
}
