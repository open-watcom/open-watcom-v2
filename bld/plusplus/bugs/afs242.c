// we don't handle non-type parms that require array decay
#include <iostream.h>

char * p[2] = {"test","code"};

template <char ** str>
class test
{
public:
  int val(void)
  {
    cout << *(*str) << endl;
    return 5;
  }
};

template <char ** p>
class testA:public test<p>{};

void main(void)
{
  test<p> t;
  testA<p> u;

  cout << t.val() << endl;
  cout << u.val() << endl;
}
