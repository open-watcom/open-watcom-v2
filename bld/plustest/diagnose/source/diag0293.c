//#include <iostream.h>
typedef void (*void_func)();

class A
{
  public:
     static void f();
     static void g();
};

void_func func(bool b)
{
    return b ? A::f : A::g;
}

#error this should be only error
