// excrt_10.cpp -- test throw when none allowed

void foo() throw()
{
    throw 10;
}

int main()
{
    foo();
    return 0;
}
