#include <stdio.h>
#include <math.h>
#include <process.h>

#define R       (10000)
#define N       (19)
#define M       ((int)(3.322*N*4))

SBIG a[3340];

int main( void )
{
    int i;
    int k;
    SBIG q;

    for (i = 0; i <= M; i++) {
        a[i] = 2;
    }
    a[M] = 4;

    for (i = 1; i <= N; i++) {
        q = 0;
        for (k = M; k > 0; k--) {
            a[k] = a[k]*R+q;
            q = a[k]/(2*k+1);
            a[k] -= (2*k+1)*q;
            q *= k;
        }
        a[0] = a[0]*R+q;
        q = a[0]/R;
        a[0] -= q*R;
        printf("%04ld", q );
        fflush( stdout );
    }
    putchar( '\n' );
    return( 0 );
}
