// incorrect errors detected by :
//TopSpeed Release 3.02

class A
{
  public:
    virtual operator long();
};

class C : public A
{
};

A::operator long()
{
    return 10;
}

main()
{
    C c;
    long i= c;
//TopSpeed Error: Incompatible type of initializer expression
    i= long(c);
//TopSpeed Error: Operand must have scalar type
    i= c.operator long();

    return 0;
}
