;;;;;;;;;;
;;;;;;;;;;
;;;;;;;;;;
;	@;;;;;;;;
;;	@;;;;;;;;
;;;	@;;;;;;;;
;;;;	@;;;;;;;;
;;;;;	@;;;;;;;;
;;;;;;	@;;;;;;;;
;;;;;;;	@;;;;;;;;
;;;;;;;;	@;;;;;;;;
;;;;;;;;;	@;;;;;;;;
;;;;;;;;;;	@;;;;;;;;
;;;;;;;;;;;	@;;;;;;;;
;;;;;;;;;;;;	@;;;;;;;;
;;;;;;;;;;;;;	@;;;;;;;;
;;;;;;;;;;;;;;	@;;;;;;;;
;;;;;;;;;;;;;;;	@;;;;;;;;
;;;;;;;;;;;;;;;;	@;;;;;;;;
;;;;;;;;;;;;;;;;;	@;;;;;;;;
;;;;;;;;;;;;;;;;;;	@;;;;;;;;
;;;;;;;;;;;;;;;;;;;	@;;;;;;;;
;;;;;;;;;;;;;;;;;;;;	@;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;	@;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;	@;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;	@;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;	@;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;	@;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;	@;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;	@;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;	@;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;	@;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;	@;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;	@;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;	@;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
namespace x1 {
    struct S {
	S( int, int );
    };
    
    void bb( int x, int y ) {
	(void)S( x, y );	// should be OK!
    }
};
namespace x46 {
    class Base {
	int	_private;
    };
    
    class Derived : public Base {
	int	Foo() {
	    _private
    += 3;
	    return 0;
	}
    };
};
void x59( int c ) {
    switch( c ) {
    case 0:
	static int y = 3;
	break;
    default:
	break;
    }
}
namespace x68 {
    namespace n1 { };
    namespace n2 { };
    namespace n3 { };
    namespace n4 { };
    namespace n5 { };
    namespace n6 { };
    namespace n7 { };
    namespace n1 {
	using namespace n2;
	using namespace n3;
	int x1;
	int v1;
    };
    namespace n2 {
	using namespace n4;
	using namespace n5;
	int x2;
	int v2;
    };
    namespace n3 {
	using namespace n6;
	using namespace n7;
	using namespace n1;
	int x2;
    };
    namespace n4 {
	int x3;
    };
    namespace n5 {
	int x3;
	int v3;
    };
    namespace n6 {
	int x3;
    };
    namespace n7 {
	int x3;
    };
    void foo() {
	n1::v1++;
	n1::v2++;
	n1::v3++;
	n1::x1++;
	n1::x2++;
	n1::x3++;
    }
}
namespace x116 {
    namespace M {
	int var_var;
	int tdef_var;
	int var_fn();
	int fn_fn( int );
    }
    namespace N {
	int var_var;
	typedef int tdef_var;
	int var_fn;
	int fn_fn();
	using namespace M;
    }
    namespace X {
	using namespace N;
	using namespace M;
    }
    void f() {
	X::var_var++;
	++X::tdef_var;
	X::var_fn();
	X::fn_fn();
    }
    void g() {
	N::var_var++;
	++N::tdef_var;
	N::var_fn();
	N::fn_fn();
    }
}
namespace x147 {
    namespace M {
	int var_var;
	int tdef_var;
	int var_fn();
	int fn_fn( int );
    }
    namespace N {
	int var_var;
	typedef int tdef_var;
	int var_fn;
	int fn_fn();
	using namespace M;
    }
    void f() {
	using namespace N;
	var_var++;
	++tdef_var;
	var_fn();
	fn_fn();
    }
}
namespace x169 {
    namespace A {
	struct S {
	    void ok();
	    void bad();
	};
	void ok();
	void bad();
    };
    void A::S::ok() {
    }
    void A::ok() {
    }
    namespace B {
	void A::S::bad() {
	    // error!
	}
	void A::bad() {
	    // error!
	}
    };
    namespace n1 {
	int i;
	void foo( int );
    };
    namespace n2 { using namespace n1; };
    namespace n3 { using namespace n2; };
    namespace n4 { using namespace n3; };
    namespace n5 {
	using namespace n4;
	void foo( char );
	int i;
    };
    int i;
    
    void foo() {
	using namespace n::n5;
	++i;
	foo( 0 );
	foo( 'a' );
    }
};

#error last line
