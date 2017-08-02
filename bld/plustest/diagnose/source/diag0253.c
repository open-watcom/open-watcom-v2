void x1() {
    enum E1 {};
    enum E2 {A};
    if( sizeof( E1 ) == sizeof( E2 ) );	// should always be true
}

void x7() {
    struct S {
	void foo();
	static void bar();
    };
}

// macro __u will automatically generate a unique name
#define __up(a,b)       a##b
#define __ue(a,b)       __up(a,b)
#define __u             __ue(__u,__LINE__)

template <class S, class C, class U, class E>
    struct Template {
	struct S *__u;
	struct C *__u;
	struct U *__u;
	struct E *__u;

	class S *__u;
	class C *__u;
	class U *__u;
	class E *__u;

	union S *__u;
	union C *__u;
	union U *__u;
	union E *__u;

	enum S *__u;
	enum C *__u;
	enum U *__u;
	enum E *__u;
    };

struct Struct {
    int __u;
    int m;
};

class Class {
public:
    int __u;
    int m;
};

union Union {
    int f;
    int m;
};

enum Enum {
    A, B, C
};

Template<Struct,Class,Union,Enum> __u;

int good;

#define IF_WATCOM_GE(a) #if a #endif

IF_WATCOM_GE(1)

#error last line
