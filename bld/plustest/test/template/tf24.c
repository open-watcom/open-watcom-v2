// will probably disallowed
template <class T>
    void foo( T * ), bar( T * );

void main()
{
    void *p;

    foo(p);
    bar(p);
}
