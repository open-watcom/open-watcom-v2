int;

enum;

enum A;

enum B { Z1, Z2 };

struct C1;

struct C2 { int a; };

struct C2;

typedef struct S C;

struct Q {
    class C *p;
};

struct S *p1;
class S *p2;
union S *p3;

union U {
    int a;
    int b;
};

union U *p4;
struct U *p5;
class U *p6;

typedef enum E { E1, E2, E3 } EA;

enum E *p7;
enum EA *p8;
