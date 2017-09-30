struct S {
    int foo();
    static int goo();
};

typedef int (S::*MPF)();
typedef int (S::*SMPF)();

struct T {
    static int array[2];
    static SMPF array3[];
};

int T::array[] = { 1, 3 };
MPF T::array3[] = { &goo, goo }; // IN STANDARD
#error last line
