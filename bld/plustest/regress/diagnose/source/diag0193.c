#define __huge __far        // until we do better job with 32-bit __huge
#define __far16 __far	    // until we do better job with 16-bit __far16

typedef int (__far FI)();
typedef int (__far16 F16I)();
typedef int (__near NI)();
typedef int __huge HI;
FI __far  *XFI;
F16I __far16  *XF16I;
NI __near  *XNI;
HI __huge  __huge *XHI;
void __export __interrupt __loadds __saveregs
     __export __interrupt __loadds __saveregs x11( int, int )
{
}
char *a1[] = { "asdf" "werty" "qwert", "wer" "dfgg" };
char *a2[] = {
    "asdf" "werty" "qwert",
    "wer" "dfgg"
    "asdf",
    "awert",
    "qweqwer"
    "qwerr"
    "qwer",
    "missing"
    "comma",
    0
};
struct X1B {
  virtual void pure() = 0;
  virtual void ident() {}
};

struct X1D : public X1B {
  void pure() {}
  void ident() {}
};

void x11()
{
  X1D d;
  X1B &f(d);
  throw f;              // cannot throw abstract class!
}

extern "C" extern "C" int x31( int );
extern "C" extern "C++" int x32( int );

extern extern "C" int x34;
long int extern "C" x35;
extern "C" long int extern "C" x36;
void x37()
{
    x31(1);
    x32(2);
}

typedef int IFII( int, int );

inline IFII x;

struct S {
    friend IFII x48;
    virtual IFII x49;
    virtual IFII x50;
    virtual IFII x51;
};

template <class T>
    void f(const T&) {}

template <class T>
    void g(volatile T&) {}

template <class T>
    void h(const volatile T&) {}

void ack()
{
    f(1);       // OK
    g(2);       // error
    h(3);       // OK
}

unsigned long x = 0x7ffffffffffffffffffffffffffffffff;
// EOF terminates comment
