class Test
{
  public:
            Test();
    virtual long pureVirt() = 0;
};

typedef long (Test::*method)();

Test::Test()
{
    method mPtr = method(&Test::pureVirt);
}

//cast of pure virtual to typedef'd method causes GP fault in ZTC2BR.EXE

