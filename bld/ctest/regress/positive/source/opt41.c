/*
 *  Codegen error in far pointer handling under -ol+ -ot
 */

#include "fail.h"

char s[] = "12345678";
char c;

void test(char far *s)
{
    char i;

    for (i = 0; i < 8; i++)
        c = s[i];             /* Generated bad code or crashes compiler */
}

int main(void)
{
    test(s);

    if (c != '8')  _fail;
    _PASS;
}
