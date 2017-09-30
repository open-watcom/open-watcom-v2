namespace x {
    namespace x2 {
          namespace { int i; }       // unique::i
          void f() { i++; }          // unique::i++

          namespace A {
                  namespace {
                          int i;     // A::unique::i
                          int j;     // A::unique::j
                  }
                  void g() { i++; }  // A::unique::i++
          }

          using namespace A;
          void h() {
                  i++;               // error: unique::i or A::unique::i
                  A::i++;            // A::unique::i
                  j++;               // A::unique::j
          }
    }
    namespace x21 {
          namespace N {
                  int i;
                  int g(int a) { return a; }
                  void k();
                  void q();
          }

          namespace { int k=1; }

          namespace N {
                  int g(char a)         // overloads N::g(int)
                  {
                          return k+a;   // error: k is a fn
                  }

                  int i;                // error: duplicate definition

                  void k();             // ok: duplicate function declaration

                  void k()              // ok: definition of N::k()
                  {
                          return g(a);  // calls N::g(int)
                  }

                  int q();              // error: different return type
          }
    }
      namespace x49 {
          namespace Q {
                  namespace V {
                          void f();
                  }
                  void V::f() { /* ... */ }  // fine
                  void V::g() { /* ... */ }  // error: g() is not yet a member of V
                  namespace V {
                          void g();
                  }
		  struct S {
		      void f();
		  };
          }

          namespace R {
                  void Q::V::g() { /* ... */ } // error: R doesn't enclose Q
		  void Q::S::f() {
		  }
          }
      }
      namespace x70 {
	    namespace M {
		int i;
	    }
	    
	    namespace N {
		int i;
		using namespace M;
	    }
	    
	    void f() {
		N::i = 7; // well-formed: M::i is not a member of N     
		using namespace N;                                      
		i = 7;    // error: both M::i and N::i are accessible   
	    }
      }
      namespace x86 {
	    namespace A { class X {}; }
	    namespace B { void X(int); }
	    using namespace A;
	    using namespace B;
	    void g() {
		X(1);
	    }
	    namespace D {
		int d1;
		void f(char);                                           
	    }
	    using namespace D;
	    
	    int d1;            // ok: no conflict with D::d1
	    
	    namespace E {
		int e;
		void f(int);
	    }
	    
	    namespace D {       // namespace extension
		int d2;
		using namespace E;
		void f(int);
	    }
	    
	    void f() {
		d1++;      // error: ambiguous ::d1 or D::d1?
		::d1++;    // ok
		D::d1++;   // ok
		d2++;      // ok: D::d2
		e++;       // ok: E::e
		f(1);      // error: ambiguous: D::f(int) or E::f(int)?
		f('a');    // ok: D::f(char)
	    }
      }
};
