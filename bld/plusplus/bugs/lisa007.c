// Status: no work to do
// this should give an error on 3rd declaration: that's how the standard reads
int f(int i,int j,int k) { return i+j+k; } ;
int f(int i,int j,int k=3);

void foo()
{

    f(3,2);
    f(3,4,5);
    extern int f(int i, int j = 2, int k=3);  // two default parameters
    f(3,2);
    f(3);
    f(3,4,2);
}



void main()
{
    foo();
}
