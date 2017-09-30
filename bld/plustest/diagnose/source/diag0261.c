#if !defined( ARCH ) || ( ARCH != 386 )
    #error system dependent test for 386
#else

#define INT3 { int 3 };
void f() {
	_asm INT3
    _asm { int 3 };
}
struct S {
    void f() {
	_asm { int 3 };
    }
};

void f( S *p ) {
    p->f();
}
class IViewObject 
{
public:
    virtual int __stdcall Draw(
	    int (  __stdcall __stdcall *pfnContinue )(
	    int dwContinue) ) = 0;
};
void x22( char __near **n ) {
    char __far **f;
    f = n;		// this should not be allowed
}

#endif
#error at least one error
