class B {
public:
    int f(int);
};

class D : public B {
public:
    int f(char*);
};



void h(D* pd)
{
    pd->f(1);       // error:
                    // D::f(char*) hides B::f(int)
    pd->B::f(1);    // ok
    pd->f("Ben");   // ok, calls D::f
}



int f(char*);
void g()
{
    extern f(int);
    f("asdf");  // error: f(int) hides f(char*)
                // so there is no f(char*) in this scope
}

void caller ()
{
  void callee (int, int);
  {
      void callee (int);  // hides callee (int, int)
      callee (88, 99);    // error: only callee (int) in scope
  }
)
