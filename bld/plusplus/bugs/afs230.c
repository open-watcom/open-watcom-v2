// hard to parse because of bit-fields and ctor-inits after decl-ids
typedef int T;
struct S {
    int T:1;
    T:0;
};
