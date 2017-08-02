#include <limits.h>

// use -wx switch

bool foo( long l ) {
    struct {
	unsigned y : 1;
	unsigned z : 3;
    } x;
    typedef enum {
	one,
	two = UCHAR_MAX,
	three,
	four
    } enum1;
    typedef enum {
	five,
	six,
	seven,
	eight
    } enum2;
    enum1 e1 = four;
    enum2 e2 = six;
    unsigned char u;
    bool b = 8;
    b = e1;	// warn (OK)
    b = e2;	// OK
    b = x.y;	// OK
    x.y = 0;	// OK
    x.y = x.z;	// warn (OK)
    int i = b;  // OK
    u = i;	// warn (OK)
    b = l;	// warn (OK)
    u = l;	// warn (OK)
    b = i;	// warn (OK)
    x.y = b;	// OK
    x.z = x.y;	// OK
    return x.y; // OK
}
