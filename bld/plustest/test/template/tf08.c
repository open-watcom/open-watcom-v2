// 14-4 p.317 draft
template <class T>
    T max( T a, T b )
    {
	return ( a > b ) ? a : b;
    }

//int max(int,int);

void f( int a, int b, char c, char d )
{
    int m1 = max(a,b);
    char m2 = max(c,d);
    int m3 = max(a,c);
}
