
class X
{
  public:
    static int init();
  private:
    static int _init;
};

int X::_init= init();
// Zortech error: "function 'initialize' has no prototype"
//resolution: "int X::_init= X::init();"

int X::init()
{
    return 1;
}



