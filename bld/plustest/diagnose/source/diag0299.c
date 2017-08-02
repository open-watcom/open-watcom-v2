enum anEnum { one, two, three };
enum Enum2 { A };

struct Test
{
    void foo (anEnum &arg);
    void foo (Enum2 &arg);
    void foo (Enum2 ,Enum2 );
    void bar (anEnum &arg);
    void bar (Enum2 &arg);
    void bar (Enum2 ,Enum2 );
};

void Test::foo (anEnum &arg)
{
}
void Test::foo (Enum2 &arg)
{
}
void Test::foo (Enum2 ,Enum2 )
{
}
void Test::bar (anEnum arg)
{
}
void Test::bar (Enum2 arg)
{
}
void Test::bar (Enum2 &,Enum2 &)
{
}
