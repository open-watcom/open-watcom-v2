#include "fail.h"

struct OK1 {
    int m;
    void operator =( int );
};
struct OK2 : OK1 {
    int m;
};
struct OK3 {
    int m;
    OK2 f;
};
struct OK4 {
    int m;
    static OK3 k;
};
struct BAD1 {
    int m;
    void operator =( BAD1 & );
};
struct BAD2 : BAD1 {
    int m;
};
struct BAD3 {
    int m;
    BAD2 f;
};
struct OK5 {
    int m;
    static BAD3 k;
};

struct Q {
    union {
	OK1 m1;
	OK2 m2;
	OK3 m3;
	OK4 m4;
	OK5 m5;
    };
};

int main() {
    Q x;
    Q y;
    x = y;
    y = x;
    _PASS;
}
