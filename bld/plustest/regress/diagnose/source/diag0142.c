struct Y1 {
    Y1(Y1);
};
struct Y4 {
    Y4(const Y4,int=0);
};
struct Y7 {
    static Y7 &xx;
    int i;
    Y7(Y7&s,Y7=xx) : i(s.i) {};
};
struct Y13 {
    static Y13 &xx;
    int i;
    Y13(Y13 const &s,Y13=xx) : i(s.i) {};
};
struct Y17 {
    Y17(Y17&);
};
struct Y20 {
    Y20(Y20 const &);
};
struct Y23 {
    static Y23 &xx;
    int i;
    Y23(Y23&s,Y23 & =xx) : i(s.i) {};
};
struct Z1 {
    void foo() const;
};

void (Z1::* mp)() = &Z1::foo;

struct X1 {
    X1(int a) : i(a) {}
    int i;
};

X1 & bad1 = 1;
X1 const & ok1 = 2;

X1 make_X1();

void bad2( X1 & );
void ok2( X1 const & );

void x14()
{
    bad2(3);
    ok2(4);
}

X1 & bad3 = make_X1();
X1 const &ok3 = make_X1();
