// developed by:
//	Erwin Unruh	erwin.unruh@mch.sni.de
//	Tom Pennello	tom@metaware.com
//
template <int i, int j, int k>
    struct Acker_is {
    };

template <int m, int n>
    struct Acker {
	static const int val = m == 0 ? n+1
            : n == 0 ? Acker<m?m-1:0,m?1:0>::val
            : Acker<m?m-1:0,m?Acker<n?m:0,n?n-1:0>::val : 0>::val;
	static void f() {
	    int y = Acker_is< m, n, val >();
	}
    };

template <>
struct Acker<0,0> {
    static const int val = 1;
    static void f() {
    }
};

void foo() {
    Acker<1,2>::f();
};
