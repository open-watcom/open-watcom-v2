struct {
    int i;
};

struct bitFields {
    int a: 10;
    int :0;
    int c;
    int d;
};

struct {
    int a;
    struct b b1;
    struct { int c; } d;
    struct e {
	union {
	    int f;
	};
	struct {
	    int g;
	} *h;
	int i;
	struct j {
	    int k;
	} l, *m;
    } o;
} name;
