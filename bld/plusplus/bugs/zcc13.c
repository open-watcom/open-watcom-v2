typedef signed char bool;

class X
{
  public:
        X& operator<<(short i);
        X& operator<<(unsigned long l);
        X& operator<<(char);
        X& operator<<(int);
};

void foo()
{
    X x;
    bool aBool = 0;
    x << aBool;
}
/*
Zortech Error: ambiguous reference to function
Had: X::operator <<(<ulong>)
and: X::operator <<(<short>)
resolution: explicit cast char(aBool), or add operator<<(signed char)
*/

