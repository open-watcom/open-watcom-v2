#include "fail.h"

#pragma inline_depth(0)
void foo() {
    int w;
    {
	struct S {
	    int x( int v ) {
		{{
		static int q;
		int t;

		t = q;
		q = v;
		return( t );
	       }}
	    }
	};
	S x;
	w = x.x( 1 );
	if( w != 0 ) fail(__LINE__);
	w = x.x( 2 );
	if( w != 1 ) fail(__LINE__);
	w = x.x( 3 );
	if( w != 2 ) fail(__LINE__);
    }
    #if __WATCOM_REVISION__ >= 8
    {
	struct S {
	    int x( int v ) {
		{{
		static int q;
		int t;

		t = q;
		q = v;
		return( t );
	       }}
	    }
	};
	S x;
	w = x.x( 4 );
	if( w != 0 ) fail(__LINE__);
    }
    {
	struct W {
	    int x( int v ) {
		int t;
		{
		    static int q;	// 0, -99		; -9, 45
		    t = q;
		    q = v;
		    v = t + 1;		// 1			; -98, -8
		}
		{
		    static int q=42;	// 42, 1		; -98, -8
		    t = q;
		    q = v;
		    v = t + 1;		// 43			; 2, -97
		}
		{
		    struct W {
			int x( int v ) {
			    int t;
			    {
				static int q;	// 0, 43, 1	; 2, -97
				t = q;
				q = v;
				v = t + 1;	// 1, 44	; 2, 3
			    }
			    {
				static int q;	// 0, 1, 44	; 2, 3
				t = q;
				q = v;
				v = t + 1;	// 1, 2		; 45, 3
			    }
			    return v;
			}
		    };
		    W x;
		    v = x.x( v );	// 1			; 45
		    v += x.x( v );	// 3			; 48
		}
		return v;
	    }
	};
	W x;
	w = x.x( -99 );		// 3
	if( w != 3 ) fail(__LINE__);
	w += x.x( -9 );		// 51
	if( w != 51 ) fail(__LINE__);
    }
    #endif
}

int main() {
    foo();
    _PASS;
}
