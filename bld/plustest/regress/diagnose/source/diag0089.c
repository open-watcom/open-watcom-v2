template <class G> class S {
    G x;
};

struct Q {
    struct S<int> {
	int x;
    };
};

void foo()
{
    struct S<double> {
	double x;
    };
}

class S<char>;
class S<unsigned char> friend;
class S<signed char> *w;

template class S<unsigned char>;
template class S<signed char>;
