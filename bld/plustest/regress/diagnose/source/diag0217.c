// 10.1 did not catch assignment to const bit field


struct S {
    int i;
    int j : 1;
};

void foo( S const *p )
{
    p->j = 0;   // no error reported
    p->i = 1;   // error reported
}
