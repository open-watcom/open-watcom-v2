struct Z7 {
protected:
    static int s;
    int i;
    void foo();
private:
    static int ps;
    int pi;
    void pfoo();
    friend void ack();
};
struct Z11 : Z7 {
    friend void ack();
};

void ack() {
    int *p = &Z7::s;
    int Z7::* mp = &Z7::i;
    void (Z7::* mf)() = &Z7::foo;

    int *pp = &Z7::ps;
    int Z7::* pmp = &Z7::pi;
    void (Z7::* pmf)() = &Z7::pfoo;
    
    int *rp = &Z11::s;
    int Z11::* rmp = &Z11::i;
    void (Z11::* rmf)() = &Z11::foo;
}
void bar() {
    int *p = &Z7::s;
    int Z7::* mp = &Z7::i;
    void (Z7::* mf)() = &Z7::foo;

    int *pp = &Z7::ps;
    int Z7::* pmp = &Z7::pi;
    void (Z7::* pmf)() = &Z7::pfoo;
    
    int *rp = &Z11::s;
    int Z11::* rmp = &Z11::i;
    void (Z11::* rmf)() = &Z11::foo;
}
