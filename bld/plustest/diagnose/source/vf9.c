struct B1 {
    B1();
    virtual void foo( int = 0, int = 2, int = 3 );
    virtual void bar( int = 0, int = 2, int = 3 );
    virtual void sam( int = 0, int = 2, int = 3 );
    virtual void ack( int = 0 );
};
struct D1 : B1 {
    D1();
    virtual void foo( int = 0, int = 2, int = 3 );
    virtual void bar( int = 0, int = 2 );
            void sam( int = 0, int = 2 );
    virtual void ack( int = 0, int = 2, int = 3 );
};
D1::D1()
{
}
