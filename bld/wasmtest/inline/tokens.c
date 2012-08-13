#ifdef __cplusplus
extern "C" {
#endif

#define INT3 int 3

void test( void )
{
    _asm {
        xor ax,dx
        or  ax,dx
        and ax,dx
        not ax
        INT3
    }
}

#ifdef __cplusplus
}
#endif
