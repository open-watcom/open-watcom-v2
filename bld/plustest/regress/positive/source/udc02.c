#include "fail.h"

class A {
  public:
    virtual void working() = 0;
};

class C :
  virtual public A
{
  public:
    virtual void working();
};

template <class T>
class SP_var {
  private:
    T* _ptr;

  public:
    SP_var(T* p) { _ptr = p; }
    operator T*() const { return _ptr; }
};
typedef SP_var<C> C_var;


void foo(A*);

int ok;

void C::working() {
    ++ok;
}

void foo(A* a) {
  a->working();
}

int main(void) {
  C_var c = new C;

  foo((C*)c);

  foo(c);
  if( ok != 2 ) _fail;
  _PASS;
}
