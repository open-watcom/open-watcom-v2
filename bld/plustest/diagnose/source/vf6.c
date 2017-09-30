struct V {
    int v;
    virtual void foo( void );
    V();
    ~V();
};
V::V(){
    foo();
}
V::~V(){
    foo();
}

struct S : virtual V {
    int s;
    virtual void foo( void );
    S();
    ~S();
};
S::S(){
    foo();
}
S::~S(){
    foo();
}
