// if Defd_Later error happens before 'bad' error; we know
// implementation defines all pending undefined classes
template <class T>
    class Defd_Later;

typedef Defd_Later<int> Not_Yet;

template <class Z>
    class Defd_Later {
	Z x;
	int x;	// error!
    };		// all existing implementations define Defd_Later<int> here
    		// i.e., Borland, MetaWare, and CFRONT

struct S;

S bad;

Not_Yet ok;
