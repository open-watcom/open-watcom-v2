// developed by:
//	Erwin Unruh	erwin.unruh@mch.sni.de
//	Tom Pennello	tom@metaware.com
//
template <int i, int j, int k>
    struct Acker_is {
    };

template <int m, int n>
    struct Acker {
	enum {
	    val = m == 0 ? n+1
	    		 : n == 0 ? Acker<m?m-1:0,m?1:0>::val
			          : Acker<m?m-1:0,m?Acker<n?m:0,n?n-1:0>::val : 0>::val
	};
	Acker_is< m, n, (int) val > x;
	void f() {
	    int y = x;
	}
    };

struct Acker<0,0> {
    enum {
	val = 1
    };
};

void foo() {
    Acker<1,2> x;
};
