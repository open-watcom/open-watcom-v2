class C;
class D;

class Q {
    friend C::operator Q();
};

class C {
    class S;
    operator S();
    operator Q();
    operator D();
    class S {
	friend C::operator S();
	friend operator int();
    };
public:
    C();
};

class D {
    friend C::operator D();
};
