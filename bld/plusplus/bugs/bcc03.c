// Borland produces incorrect error message on line 9

// 5.2.2

int     i;

void f5_2_2( int *const icp )
{
    *icp++;                     // OK - this is perfectly legal.
                                // changing icp itself would be illegal.
    i = *icp;                   // This is to avoid any unreferenced warnings.
}

void main( void )
{
    int * const     icp=&i;

    f5_2_2( icp );
}
