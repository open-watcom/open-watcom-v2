struct S {
    operator int ();
    operator double ();
    int s;
};

int (S::* p)() = &(S::operator int);
double (S::* q)() = &(S::operator double);

void foo( S *z )
{
    (z->*p)();
    (z->*q)();
}
