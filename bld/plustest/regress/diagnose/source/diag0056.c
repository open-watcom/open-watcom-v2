int bad1 = 1;
int bad1 = 2;

int bad2;
int bad2;

int bad3;
extern int bad3 = 2;

extern const int bad4 = 2;
const int bad4;

extern const int bad5 = 2;
extern const int bad5;
const int bad5 = 3;

struct B1 {
    static int bad6[];
};
int B1::bad6[2] = { 1, 2, 3 };

int bad7[23];
int bad7[];

int bad8[10];
int bad8[] = { 1, 2 };

int bad9[];
int bad9[12];

extern int ok1;
int ok1;

int ok2;
extern int ok2;

extern int ok3 = 3;

extern int ok4[];
int ok4[2] = { 1, 2 };

extern int ok5[];
int ok5[2];

struct O1 {
    static int ok6[];
};

static int O1::ok6[2];

struct O2 {
    static int ok7[];
};

static int O2::ok7[2] = { 1, 2 };

typedef struct OK8 {
    int a;
    int b;
} OK8;

struct O3 {
    static int ok9[];
};

static int O3::ok9[8] = { 1, 2, 3, 4 };
int *ok10 = &(O3::ok9[7]);

struct O4 {
    static int ok11[17];
};

int O4::ok11[];
