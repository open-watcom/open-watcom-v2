#include <process.h>
void foo() {
    _beginthread(  0, 0, 0 );
    _beginthreadex( 0, 0, 0, 0, 0, 0 );
}
namespace x1 {
    int x;
    struct B {
	int x;
	typedef int T;
    };
    struct BB : B {
    };
    class E1 : B {
	::x;
	::B;
	BB::T;
	B::T;
    };
    struct E2 : BB {
	BB::T;
    };
    struct P {
	typedef int PU;
    private:
	typedef int PI;
    protected:
	typedef int PO;
	friend class DPI;
	friend class DPO;
	friend class DPU;
    };
    struct DPI : private P {
    private:
	P::PU;
	P::PI;
	P::PO;
    };
    struct DPO : private P {
    protected:
	P::PU;
	P::PI;
	P::PO;
    };
    struct DPU : private P {
    public:
	P::PU;
	P::PI;
	P::PO;
    };
    struct XX {
	enum T {
	    A,B,C
	};
    };
    struct YY : private XX {
	void foo() {
	    enum T x;
	    x = A;
	    x = B;
	    x = C;
	}
    };
    struct ZZ : private XX {
	XX::T;
	void foo() {
	    enum T x;
	    x = A;
	    x = B;
	    x = C;
	}
    };
};
namespace x69 {
          int x;
          namespace Y {
                  void f(float);
                  void h(int);
          }
          namespace Z {
                  void h(double);
          }
          namespace A {
                  using namespace Y;
                  void f(int);
                  void g(int);
                  int i;
          }
          namespace B {
                  using namespace Z;
                  void f(char);
                  int i;
          }
          namespace AB {
                  using namespace A;
                  using namespace B;
                  void g();
          }

          void h()
          {
                  AB::g();     // g is declared directly in AB,
                               // therefore S is { AB::g() } and AB::g() is chosen
                  AB::f(1);    // f is not declared directly in AB so the rules are
                               // applied recursively to A and B;
                               // namespace Y is not searched and Y::f(float)
                               // is not considered;
                               // S is { A::f(int), B::f(char) } and overload
                               // resolution chooses A::f(int)
                  AB::f('c');  // as above but resolution chooses B::f(char)

                  //AB::x++;     // x is not declared directly in AB, and
                               // is not declared in A or B, so the rules are
                               // applied recursively to Y and Z,
                               // S is { } so the program is ill-formed
                  //AB::i++;     // i is not declared directly in AB so the rules are
                               // applied recursively to A and B,
                               // S is { A::i, B::i } so the use is ambiguous
                               // and the program is ill-formed
                  AB::h(16.8); // h is not declared directly in AB and
                               // not declared directly in A or B so the rules are
                               // applied recursively to Y and Z,
                               // S is { Y::h(int), Z::h(float) } and overload
                               // resolution chooses Z::h(float)
          }
};
namespace x122 {
          namespace A {
                  namespace B {
                          void f1(int);
                  }
                  using namespace B;
          }
          void A::f1(int){}  // ill-formed, f1 is not a member of A
};
namespace x131 {
          namespace A {
                  struct x { };
                  int x;
                  int y;
          }
          namespace B {
                  struct y {};
          }
          namespace C {
                  using namespace A;
                  using namespace B;
                  int i = C::x; // ok, A::x (of type 'int')
                  int j = C::y; // ambiguous, A::y or B::y
          }
          namespace N {
                  int i;
                  int g(int a) { return a; }
                  int k();
                  void q();
          }
          namespace { int l=1; }
          // the potential scope of l is from its point of declaration
          // to the end of the translation unit
          namespace N {
                  int g(char a)         // overloads N::g(int)
                  {
                          return l+a;   // l is from unnamed namespace
                  }
                  int i;                // error: duplicate definition
                  int k();              // ok: duplicate function declaration
                  int k()               // ok: definition of N::k()
                  {
                          return g(i);  // calls N::g(int)
                  }
                  int q();              // error: different return type
          }
	  namespace x168 {
          namespace A {
                  namespace N {
                          void f();
                  }
          }
          void A::N::f() {
                  i = 5;
                  // The following scopes are searched for a declaration of i:
                  // 1) function scope of A::N::f, before the use of i
                  // 2) scope of namespace N
                  // 3) scope of namespace A
                  // 4) global scope, before the definition of A::N::f
          }
	  }
	  namespace x183 {
          namespace M {
                  class B { };
          }
          namespace N {
                  class Y : public M::B {
                          class X {
                                  int a[i];
                          };
                  };
          }
          // The following scopes are searched for a declaration of i:
          // 1) scope of class N::Y::X, before the use of i
          // 2) scope of class N::Y, before the definition of N::Y::X
          // 3) scope of N::Y's base class M::B
          // 4) scope of namespace N, before the definition of N::Y
          // 5) global scope, before the definition of N
	  }
	  namespace x201 {
          namespace A {
                  namespace B {
                          void f1(int);
                  }
                  using namespace B;
          }
          void A::f1(int){}  // ill-formed, f1 is not a member of A
          class B { };
          namespace M {
                  namespace N {
                          class X : public B {
                                  void f();
                          };
                  }
          }
          void M::N::X::f() {
                  i = 16;
          }
          // The following scopes are searched for a declaration of i:
          // 1) function scope of M::N::X::f, before the use of i
          // 2) scope of class M::N::X
          // 3) scope of M::N::X's base class B
          // 4) scope of namespace M::N
          // 5) scope of namespace M
          // 6) global scope, before the definition of M::N::X::f
	  }
};
namespace x229 {
    int q;
    namespace x {
	template <class T>
	    struct S {
		T x[10];
	    };
	template <class T>
	    void foo( T x );
	namespace N {
	    int i;
	};
	typedef int T;
	using ::q;
	using ::operator +;
    };
    
    using x::S;
    using x::operator int;
    using x::T;
    using x::N;
    using x::foo;
    using x::operator +;
    
    namespace x25 {
	struct B {
	    using ::q;
	    int m;
	};
	using B::m;
	namespace x {
	    namespace B {
		int x;
	    };
	    struct D {
		using B::x;
	    };
	}
	namespace y {
	    struct Q {
		int q;
	    };
	    struct B {
		int i;
		int h;
	    };
	    struct BB : B {
		using Q::q;
		int k;
	    };
	    struct D : BB {
		using Q::q;
		using BB::h;
		using B::i;
	    };
	    int i;
	};
	void foo() {
	    using y::i;
	    using y::BB::k;
	}
    }
    namespace x291 {
	struct S {
	    operator unsigned char();
	    operator unsigned short();
	};
	
	int test( S & h )
	{
	    return h != 0;
	}
    }
    namespace x302 {
	// Assume f and g have not yet been defined.
	namespace A {
	    class X {
		friend void f(X*);  // declaration of f
		int v;
		class Y {
		    friend void g();
		};
	    };
	    
	    void f(X*p) { p->v++; }     // definition of f declared above
	    X x;
	    void g() { f(&x); }         // f and g are members of A
	}
	
	using A::x;
	
	void h() {
	    A::f(x);
	    A::X::f(x);    // error: f is not a member of A::X
	    A::X::Y::g();  // error: g is not a member of A::X::Y
	}
    };
    namespace x326 {
	struct X {
	    int i;
	    static int s;
	};
	
	void f() {
	    using X::i; // error: X::i is a class member
			// and this is not a member declaration.
	    using X::s; // error: X::s is a class member
			// and this is not a member declaration.
	}
    };
    namespace x339 {
	namespace B {
	    int i;
	    void f(int);
	    void f(double);
	}
	
	void g() {
	    int i;
	    using B::i;     // error: i declared twice
	    void f(char);
	    using B::f;     // fine: each f is a function
	}
	namespace C {
	    void f(int);
	    void f(double);
	    void f(char);
	}
	
	void h()
	{
	    using B::f;   // B::f(int) and B::f(double)
	    using C::f;   // C::f(int), C::f(double), and C::f(char)
	    f('h');       // calls C::f(char)
	    f(1);         // error: ambiguous: B::f(int) or C::f(int) ?
	    void f(int);  // error: f(int) conflicts with C::f(int)
	}
    };
    namespace x367 {
	namespace C {
	    void f(int);
	    void f(double);
	    void f(char);
	}
	
	void h()
	{
	    using B::f;   // B::f(int) and B::f(double)
	    using C::f;   // C::f(int), C::f(double), and C::f(char)
	    f('h');       // calls C::f(char)
	    f(1);         // error: ambiguous: B::f(int) or C::f(int) ?
	    void f(int);  // error: f(int) conflicts with C::f(int)
	}
    };
    namespace x383 {
	namespace A {
	    void foo( int );
	};
	namespace B {
	    void foo( int );
	};
	void foo() {
	    using A::foo;
	    extern void foo( int );
	}
	void bar() {
	    extern void foo( int );
	    using A::foo;
	}
	void sam() {
	    using A::foo;
	    using B::foo;
	}
	void ack() {
	    extern void foo( int );
	    extern void foo( int );
	}
	namespace a {
	    using A::foo;
	    extern void foo( int );
	}
	namespace b {
	    extern void foo( int );
	    using A::foo;
	}
	namespace c {
	    using A::foo;
	    using B::foo;
	}
	namespace d {
	    extern void foo( int );
	    extern void foo( int );
	}
	namespace x422 {
	    namespace A {
		int foo;
	    };
	    namespace B {
		int foo;
	    };
	    void qfoo() {
		using A::foo;
		extern int foo;
	    }
	    void bar() {
		extern int foo;
		using A::foo;
	    }
	    void sam() {
		using A::foo;
		using B::foo;
	    }
	    void ack() {
		extern int foo;
		extern int foo;
	    }
	    namespace a {
		using A::foo;
		extern int foo;
	    }
	    namespace b {
		extern int foo;
		using A::foo;
	    }
	    namespace c {
		using A::foo;
		using B::foo;
	    }
	    namespace d {
		extern int foo;
		extern int foo;
	    }
	}
    };
    namespace x423 {
	class C {
	    int n;
	    using C::n;
	};
	namespace N {
	    int n;
	    using N::n;
	};
	namespace M {
	    struct n;
	    using N::n;
	};
	namespace A {
	    int a;
	};
	namespace B {
	    using A::a;
	};
	namespace A {
	    using B::a;
	};
    };
    namespace x446 {
	namespace B {
	    int i;
	    void f(int);
	    void f(double);
	}
	
	void g() {
	    int i;
	    using B::i;     // error: i declared twice
	    extern void f(char);
	    using B::f;     // fine: each f is a function
	}
	namespace C {
	    void f(int);
	    void f(double);
	    void f(char);
	}
	
	void h()
	{
	    using B::f;   // B::f(int) and B::f(double)
	    using C::f;   // C::f(int), C::f(double), and C::f(char)
	    f('h');       // calls C::f(char)
	    f(1);         // error: ambiguous: B::f(int) or C::f(int) ?
	    extern void f(int);  // error: f(int) conflicts with C::f(int)
	    f(1);         // error: ambiguous: B::f(int) or C::f(int) or ::f(int) ?
	}
    };
    namespace x520 {
	int __interrupt q;
	int __export x;
	int *__export y;
	__declspec(dllexport) int (*bar)( void );
	namespace X {
	    int foo( int );
	};
	namespace Y {
	    using X::foo;
	};
	int Y::foo( int x ) {
	    return x + 1;
	}
    };
};
#error last line
