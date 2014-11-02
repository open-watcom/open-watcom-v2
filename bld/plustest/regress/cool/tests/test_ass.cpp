#include <cool/List.h>
#include <cool/Association.h>
#include <cool/test.h>

#include <cool/List.C>
#include <cool/Pair.C>
#include <cool/Vector.C>
#include <cool/Association.C>

void test_int_list () {
  CoolList<int> l1(1, 1);
  CoolList<int> l2(2, 1, 2);
  CoolList<int> l3(3, 1, 2 , 3);

  typedef CoolPair<int, CoolList<int> > Hack; //##
  CoolAssociation<int,CoolList<int> > a;
  cout << a << endl;
  a.put(1, l1);
  cout << a << endl;
  a.put(2, l2);
  cout << a << endl;
  a.put(3, l3);
  cout << a << endl;
  a.put(4, l3);

  cout << a << endl;

  CoolList<int> l;
  a.get(0, l);
  l.describe(cout); cout << endl;
  a.get(1, l);
  l.describe(cout); cout << endl;
  a.get(2, l);
  l.describe(cout); cout << endl;
  a.get(3, l);
  l.describe(cout); cout << endl;
  a.get(4, l);
  l.describe(cout); cout << endl;
  
}

int main () {
  test_int_list ();
  return 0;
}
