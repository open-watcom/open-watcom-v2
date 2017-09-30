
class A {
    public:
        A( char * text ) { _text = text; }
        ~A() { }

        char *          _text;
};

void main() {
    A           anA("hello");
    int         i;

    anA = (i==0) ? A("foo") : "foo";
}

#error This should be the only error
