typedef void (*genFnPtr)();
typedef void (*argFnPtr)(int,const char*);

class Y
{
  public:
    Y( genFnPtr m );
};

void foo( argFnPtr method)
{
  new Y( genFnPtr(method) );    // function-like cast should call proper ctor
}
