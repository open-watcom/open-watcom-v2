template <class Q>
    struct S {
	int a;
    };

template <class T>
    void foo( S<T> x )
    {
	++x;
    }

void ack( const S<int> &r )
{
    foo( r );
}

template <class Key_T>
class abc {
    class def {
	static int g;
    };
};
   
//template <class T>
static int abc<T>::def::g = 0;
