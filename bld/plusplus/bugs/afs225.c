// ARM p. 104

class X {
public:
  int  f();
};

void k(X* p)
{
  int i = p->f();  // now X::f() has external linkage
}

inline int X::f() // error: called before defined
                  // as inline
{
  // ...
  return 0;
}
