// test for throw of an abstract

struct X1B {
  virtual void pure() = 0;
  virtual void ident() {}
};

struct X1D : public X1B {
  void pure() {}
  void ident() {}
};

void x11()
{
  X1D d;
  X1B &f(d);
  throw f;              // cannot throw abstract class!
}
