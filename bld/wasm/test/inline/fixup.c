#ifdef __cplusplus
extern "C" {
#endif

long x = -1;

extern void __test( void );
#pragma aux __test = "mov eax,dword ptr [x + eax + ebx]"

void test( void ) {
    __test();
}

#ifdef __cplusplus
}
#endif
