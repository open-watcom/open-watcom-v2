#include <math.h>
#include "doubMat.h"

#define DIM 3

void main()
{
    int i,j;
    doubleMatrix identity(DIM,DIM,0.0);
    doubleMatrix dn(DIM,DIM,0.0);
    doubleMatrix inverse(DIM,DIM,0.0);
    doubleMatrix m1(DIM,DIM,0.0);
    doubleMatrix m2(DIM,DIM,0.0);

    for(i=0;i<DIM;++i) {
	for(j=0;j<DIM;++j) {
	    if( i == j ) {
		identity[i][j] = 1;
	    }
	    dn[i][j] = rand()*1.0/RAND_MAX;
	}
    }
    cout << "identity matrix" << endl;
    cout << identity << endl;
    cout << "random matrix" << endl;
    cout << dn << endl;
    cout << "random matrix's inverse" << endl;
    cout << dn.i() << endl;
    cout << "random matrix vertically joined with its inverse" << endl;
    cout << ( dn ^ dn.i() ) << endl;
    cout << "random matrix outer product with its inverse" << endl;
    cout << ( dn & dn.i() ) << endl;
    inverse = dn.i();
    // for some reason this class library has no matrix multiply so we
    // hand code a test of the inverse function
    for(int mrow=0;mrow<DIM;++mrow) {
	for(int mcol=0;mcol<DIM;++mcol) {
	    m1[mrow][mcol] = 0;
	    m2[mrow][mcol] = 0;
	    for(int i=0;i<DIM;++i) {
		m1[mrow][mcol] += inverse[mrow][i] * dn[i][mcol];
		m2[mrow][mcol] += dn[mrow][i] * inverse[i][mcol];
	    }
	    if( fabs(m1[mrow][mcol]) < 1e-14 ) {
		m1[mrow][mcol] = 0;
	    }
	    if( fabs(m2[mrow][mcol]) < 1e-14 ) {
		m2[mrow][mcol] = 0;
	    }
	}
    }
    cout << "random * inverse" << endl;
    cout << m1 << endl;
    cout << "inverse * random" << endl;
    cout << m2 << endl;
    exit( 0 );
}
