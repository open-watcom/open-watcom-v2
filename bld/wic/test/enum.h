// Test enumerated types conversion

enum {
    a,   b
};

enum c {
    d = 10*4^1+~1,
    e };

struct f {
    enum g {
	g1,
	g2,
	g3
    } * const volatile h;
    int i;
    enum j k;
    enum j1 k0;
    enum j2 k1, k2, k3;
    enum {
	k4,
	k5,
	k6
    } k7;
};

enum noDclr;
