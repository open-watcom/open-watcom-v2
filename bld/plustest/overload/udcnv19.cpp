#include "dump.h"

struct String {
    String() {}
    String( char ) {}
};

struct A
{
    operator String() const {
	GOOD;
	return String();
    }
    operator int() const {GOOD; return 1;}
    operator float() const {GOOD; return 3.0f; }
};

struct B
{
    const A& GetitsA() {
	A *a = new A();;
	return *a;
    }
};

int main()
{
   String s;
   B b;

   s = b.GetitsA();
   CHECK_GOOD(11);
   return errors != 0;
}
