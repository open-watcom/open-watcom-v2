/* how does cfront 3.0 manage this? */
typedef struct X { int gi; } GX;

void f () {
    struct X { int ni; };
    struct Y : GX { int i; X x; } y;
    int g = y.x.gi;             // error: cfront 2, LPI, Oregon
    int n = y.x.ni;             // error: cfront 3
}
