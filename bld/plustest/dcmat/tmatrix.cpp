#include "compmat.h"
#define matrix complexMatrix

int main() {
    matrix x(2, 2, complex(1.0, sqrt(3.0)));
    x[1][1]	 = -x[0][0];
    x[0][1]	 = conj(x[1][0]);
    
    cout <<"     x "<< "\n"
	<<      x  << "\n";
    cout  <<"sqrt(x)"<< "\n"
	<< sqrt(x) << "\n";
    cout  <<"conj(x)"<< "\n"
	<< conj(x) << "\n";
    cout  <<"cosh(x)"<< "\n"
	<< cosh(x) << "\n";
    cout  <<"sinh(x)"<< "\n"
	<< sinh(x) << "\n";
    cout  <<" cos(x)"<< "\n"
	<<  cos(x) << "\n";
    cout  <<" sin(x)"<< "\n"
	<<  sin(x) << "\n";
    cout  <<" abs(x)"<< "\n"
	<<  abs(x) << "\n";
    cout  <<"norm(x)"<< "\n"
	<< norm(x) << "\n";
    cout  <<" arg(x)"<< "\n"
	<<  arg(x) << "\n";
    cout  <<"real(x)"<< "\n"
	<< real(x) << "\n";
    cout  <<"imag(x)"<< "\n"
	<< imag(x) << "\n";
    
    cout<<"     x "<< "\n"
	<<      x  << "\n"
	<<"sqrt(x)"<< "\n"
	<< sqrt(x) << "\n"
	<<"conj(x)"<< "\n"
	<< conj(x) << "\n"
	<<"cosh(x)"<< "\n"
	<< cosh(x) << "\n"
	<<"sinh(x)"<< "\n"
	<< sinh(x) << "\n"
	<<" cos(x)"<< "\n"
	<<  cos(x) << "\n"
	<<" sin(x)"<< "\n"
	<<  sin(x) << "\n"
	<<" abs(x)"<< "\n"
	<<  abs(x) << "\n"
	<<"norm(x)"<< "\n"
	<< norm(x) << "\n"
	<<" arg(x)"<< "\n"
	<<  arg(x) << "\n"
	<<"real(x)"<< "\n"
	<< real(x) << "\n"
	<<"imag(x)"<< "\n"
	<< imag(x) << "\n"
	;
    return 0;
}
