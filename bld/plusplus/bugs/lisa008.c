// Status: no work to do
// this should give an error on 3rd declaration: that's how the standard reads
#include <iostream.h>

void f(int i, int j);
void f(int i, int j = 99 ); // OK: redeclaration of f(int, int)
void f(int i = 88, int j=99  ); // ERROR

void f(int i, int j)
{
    cout << j << '\n';

}


void main()
{
    f( 1, 2);
    f( 3 );
}
