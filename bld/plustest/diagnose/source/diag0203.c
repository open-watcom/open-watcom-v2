// developed by:
//	Erwin Unruh	erwin.unruh@mch.sni.de
//	Tom Pennello	tom@metaware.com
//
// modified by:
//      Christof Meerwald
//
template <int i>
    struct Prime {
      private:
        Prime();
    };

template <>
    struct Prime<0> {
        Prime();
    };

template <int p, int i>
    struct is_prime {
	enum {
	    value = (p%i) && is_prime< (i>2?p:0), i-1 >::value
	};
    };

template <int i>
    struct Prime_print {
	static void f() {
            Prime< is_prime< i, i-1 >::value ? i : 0 >();
            Prime_print< i-1 >::f();
	}
    };

template <>
struct is_prime<0,0> {
    enum {
	value = 1
    };
};
template <>
struct is_prime<0,1> {
    enum {
	value = 1
    };
};
template <>
struct Prime_print<2> {
    static void f() {
        Prime< 2 >();
    }
};

void gen_primes()
{
    Prime_print<30>::f();
}
