// default arguments anyone?
template <class G>
    void f2( G *p = 0 )
    {
    }
extern void f2(int,int);
extern void f2(char,int);
extern void f2(char,char);
template <class G>
    void f2( register void (*p)( G, G ) = f2, G )// which 'f2'? shall we generate one?
    {
    }

void foo()
{
    f2();	// what will 'G' bind to?
    int *x;
    f2(x);
}
