
class C {
    public:
       virtual foo() = 0;
};

class D {
    public:
       foo();
};

void bar() {
    D d;
    throw d;
}

int main() {
    try {
        bar();
    } catch (C c) { // attempt to catch abstract class object
        c.foo();
    }
    return( 0 );
}
