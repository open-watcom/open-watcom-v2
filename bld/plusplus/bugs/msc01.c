extern void f1();
extern int f2();

void foo()
{
        0 ? f1() : f2() ;  // one void, one int
}

/*
error : Value of type void is not allowed
all compilers except Borland 2.0 and MS 7.0 catch this
*/
