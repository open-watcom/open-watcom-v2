#include "fail.h"
#include "link14.h"

int ack;

void bar();

int main()
{
    try {
	bar();
    } catch(Scanner e) {
	e = e;
    } catch(...) {
	_fail;
    }
    _PASS;
}
