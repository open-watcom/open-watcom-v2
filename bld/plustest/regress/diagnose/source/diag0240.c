struct X1 {
    X1( X1 );
};

void x5( X1 x ) {
    0,X1( x );
}
struct x8 {
    struct B {
	virtual void p() = 0;
    protected:
	B();
	B( B const & );
    };
    struct D : B {
	void p() {
	}
    };
    void x() {
	throw D();	// OK
    }
};

template <class T>
    struct X26 {
    };

template <class T>
    void x29( X26<T> const *** );

template <class T>
    void x29( X26<T> const * );

struct X38 : X26<int> {
};

void x37() {
    X38 *y;
    x29( y );	// OK
    X38 ***x;
    x29( x );	// error
    X38 volatile *q;
    x29( q );	// error
}
struct CPoint {
    };

template<class TYPE>
    void DumpElements(const TYPE*) {}

template<class TYPE>
    struct CArray {
	    TYPE* m_pData;
	    void Dump() const;
    };

template<class TYPE>
    void CArray<TYPE>::Dump() const {
	DumpElements(m_pData);
    }

//extern void DumpElements(const CPoint*);
typedef CArray<CPoint> CArrayCpoint;

struct X67 {
    X67(int);
};
void x70( double a )
{
    X67 x( int(a) );	// warn no 'extern' on function prototype
    X67 y( int() );	// warn no 'extern' on function prototype
    static X67 z( int(a) );	// warn no 'static' allowed
    static X67 w( int() );	// warn no 'static' allowed

    extern X67 xok( int(a) );	// OK
    extern X67 yok( int() );	// OK
}

#include <stddef.h>

struct X83;
struct X84 {
    struct B {
	int m1();
	static int m2();
	static int m3;
	int m4;
	typedef int m5;
	enum { m6 };
	int m7 : 1;
	typedef int T1[];
	typedef int T2;
    
	void test() {
	    int x;
	    x = offsetof( B, m1 );
	    x = offsetof( B, m2 );
	    x = offsetof( B, m3 );
	    x = offsetof( B, m4 );
	    x = offsetof( B, m5 );
	    x = offsetof( B, m6 );
	    x = offsetof( B, m7 );
	    x = offsetof( T1, m1 );
	    x = offsetof( T2, m1 );
	    x = offsetof( X83, m1 );
	    x = offsetof( B, m4.m1 );
	    x = offsetof( B, m4[3] );
	}
    };
};

struct x114 {
    struct C {
	C *next;
	char name[];
    };
    
    struct X {
	C q;
    };
    
    C a[10];
};
// crashes compiler
class qEventQ {
   static char *Obj_Menu;				 // ** 1 **
};
short KbdCharIn()
{
   static shortlastshift = bioskey(2);			// ** 2 **
}
char  *qEventQ::Obj_Menu = 0L;				 // ** 3 **
#if 0
#illegal directive (no error should be reported)
#endif
struct X138 {
    struct S {
	~S();
    };
    S fn_retS();
    void useS( const S );
    void afs( void )
    {
	useS( fn_retS() );
    }
};
#include <string.h>
struct x150 {
    struct VF {
	int virtual foo( int );
    };
    struct V {
    };
    struct VB : virtual V {
    };
    struct IVF : VF {
    };
    struct IVB : VB {
    };
    struct IV : V {
    };
    struct EVF {
	VF x;
    };
    struct EVB {
	VB x;
    };
    struct EVF2 {
	EVF x;
    };
    struct EVB2 {
	EVB x;
    };
    
    void foo( VF *p_vf, VB *p_vb, V *p_v, IVF *p_ivf, IVB *p_ivb, IV *p_iv ) {
	memset( p_v, 0, sizeof( *p_v ) );
	memset( p_iv, 0, sizeof( *p_iv ) );
    
	memset( p_vf, 0, sizeof( *p_vf ) );
	memset( p_vb, 0, sizeof( *p_vb ) );
	memset( p_ivf, 0, sizeof( *p_ivf ) );
	memset( p_ivb, 0, sizeof( *p_ivb ) );
    }
    void bar( EVF *p_evf, EVB *p_evb, EVF2 *p_evf2, EVB2 *p_evb2 ) {
	memset( p_evf, 0, sizeof( *p_evf ) );
	memset( p_evb, 0, sizeof( *p_evb ) );
	memset( p_evf2, 0, sizeof( *p_evf2 ) );
	memset( p_evb2, 0, sizeof( *p_evb2 ) );
    }
};
struct x193 {
    x193();
    int a;
};
struct x197 {
    ~x197();
    int a;
};

x193 __declspec(thread) x202;
x197 __declspec(thread) x203;
int x204();
int __declspec(thread) x205 = x204();
int __declspec(thread) x206 = 5;


void x209() {
    static x193 __declspec(thread) x210;
    x210.a = 1;
    x203.a = 3;
}
#define g214(a,b)	a##b
int x214= g214(0x,1f);
enum x216 { A, B, C };
typedef unsigned x217;

class x219 {
    x219( x216 const );
    x219( x217 const );
};

x219::x219( x217 handle )	// missing a const!
{
}

x219::x219( x216 handle )	// missing a const!
{
}
void x231() {
    float ok_float[] = {
	.117549435e-37f,
	1.17549435e-38f,
	11.7549435e-39f,
	-.117549435e-37f,
	-1.17549435e-38f,
	-11.7549435e-39f,
	3.40282347e+38f,
	.340282347e+39f,
	.0340282347e+40f,
	-3.40282347e+38f,
	-.340282347e+39f,
	-.0340282347e+40f,
    };
    float bad_float[] = {
	.117549434e-38f,
	.117549433e-38f,
	.117549434999e-38f,
	.117549435e-38f,
	1.17549435e-39f,
	11.7549435e-40f,
	-.117549435e-38f,
	-1.17549435e-39f,
	-11.7549435e-40f,
	-1.17549434e-39f,
	-1.17549433e-39f,
	-1.17549434999e-39f,
	3.40282348e+38f,
	.340282349e+39f,
	.0340282347e+41f,
	-3.40282348e+38f,
	-.340282349e+39f,
	-.0340282347e+41f,
    };
    double ok_double[] = {
	22.250738585072014e-309,
	2.2250738585072014e-308,
	.22250738585072014e-307,
	.022250738585072014e-306,
	1.7976931348623157e308,
	.17976931348623157e309,
	.017976931348623157e310,
	.0017976931348623157e311,
	.00017976931348623157e312,
	.000017976931348623157e313,
	.0000017976931348623157e314,
	.00000017976931348623157e315,
    };
    double bad_double[] = {
	1.7976931348623158e308,
	1.7976931348623159e308,
	.0000017976931348623159e315,
	-1.7976931348623158e308,
	-1.7976931348623159e308,
	-.0000017976931348623159e315,
	2.2250738585072013e-308,
	22.250738585072013e-309,
	-2.2250738585072013e-308,
	-22.250738585072013e-309,
    };
}
