// Brad Bisco found this one
struct QQ;
struct PP;

template <class T>
    struct LL {
	void a( QQ *p )
	{
	    p->no_no = 1;
	};
	void bad( PP *p )
	{
	    p->mega_no = 1;
	};
    };

class PP {
    int mega_no;
};

class QQ {
    friend class LL;	// all instantiations of LL are my friend
    int no_no;
};

LL<int> x;

void main()
{
    QQ z;
    x.a( &z );
}
