#include "fail.h"
#include "link15.h"

Scanner::Scanner()
{
}

Scanner::~Scanner()
{
}

static Scanner *p;

int main()
{
    p = new Scanner;
    _PASS;
}
