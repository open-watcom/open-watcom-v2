/*
 *  Changelist 31737
 *  Should cause an error because the pragma attempts to define code for an already existing function
 */

int foo(int i) { return i; }

#if defined __386__
#pragma aux foo = \
    "mov  eax,1"  \
    parm caller [eax]
#else
#pragma aux foo = \
    "mov  ax,1"  \
    parm caller [ax]
#endif

int main(void)
{
    return foo(5);
}
