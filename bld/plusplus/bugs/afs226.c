// should we warn about this?
struct S {
    virtual inline ~S() = 0;
    int a;
};

S::~S() {
}

int x = 3;
