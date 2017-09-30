struct A {
    const int i1 = 0;
    static int i2 = 0;
    static const i3 = 0;

    virtual void f1() = 0;
    virtual void f2() = i3;
    virtual void f3() = 1;
    virtual void f4() = 1 - 1;
};

int main() {
    return 0;
}
