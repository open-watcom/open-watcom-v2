// it's complicated but it would be nice if these were link time constants also
// AFS
struct S {
    int a;
};

int S::* v1 = { &S::a };        // linker constant
int S::* v2 = { 0 };            // linker constant
int S::* a1[] = { &S::a };      // needs .fn_init
int S::* a2[] = { 0 };          // needs .fn_init
