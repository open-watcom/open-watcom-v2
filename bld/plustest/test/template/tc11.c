template <class T,int size,void (*err)()> class C;
template <class _T,int _size,void (*_err)()> class C;
void ok();

C<int,10,ok> *p;
C<int,10,ok> *q;

template <class __T,int __size,void (*__err)()> struct C {
    C(){foo();}
    ~C(){foo();}
    __T x[__size];
    void foo()
    {
	ok();
    }
};

C<int,10,ok> x;
