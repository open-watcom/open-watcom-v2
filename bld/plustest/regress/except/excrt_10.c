// excrt_10.c -- test throw when none allowed

void foo() throw()
{
    throw 10;
}

int main()
{
    foo();
    return 0;
}
