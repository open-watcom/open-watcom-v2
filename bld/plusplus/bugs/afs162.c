// compile -w4
// low priority: I don't think this is nice but it isn't the end of the world (AFS)
struct S {
    int x;
    S( int n ) : x(n) {}
    struct N;
private:
    struct N {
    };
};
