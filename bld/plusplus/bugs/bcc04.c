
void main( void )
{
    float           b1;
    signed int      b3;
    float *         p1;

    b1=5;b3=5;p1=&b1;       // avoid possible use warnings

    b3 = b1 && p1;          // Borland produces an incorrect error on this line

    b1=b1;b3=b3;p1=p1;      // avoid unref'd warnings
}
