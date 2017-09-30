template <class T>
    class X2;
    
class X4 {
    friend class X2;
    int f;
    friend class X2;
};

void foo()
{
    class LOCAL {
	friend class X2;
	int f;
    };
}

struct QQ;
struct PP;

template <class T>
    struct LL {
	void a( QQ *p )
	{
	    p->ok = 1;
	};
	void bad( PP *p )
	{
	    p->bad = 1;
	};
    };

class PP {
    int bad;
};

class QQ {
    friend class LL;	// all instantiations of LL are my friend
    int ok;
};

LL<int> x;

void bar()
{
    QQ z;
    x.a( &z );
}

template class LL<int>;
