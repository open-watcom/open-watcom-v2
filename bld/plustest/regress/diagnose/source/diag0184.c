// test bad operands for .*, ->*

int i;
char* p;

struct S {
    int mi;
    char* mp;
    void foo();
};


void goop()
{
    i .* i;
    i .* p;
    p .* i;
    p .* p;
    i ->* i;
    i ->* p;
    p ->* i;
    p ->* p;
}
