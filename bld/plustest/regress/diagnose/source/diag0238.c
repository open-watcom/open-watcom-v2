class A {                   // abstract class
public:
    virtual int pure() = 0;
};    

class S {                   // private copy constructor
    S( S const & );
public:
    S();
    static void poo();
};

class T;                    // undefined class

class U {                   // private destructor
    ~U();
public:
    U();
    static void poo();
};


static void S::poo()
{
    throw S();
}

void foo()
{
    try {
        S::poo();
    } catch( S val ) {
    } catch( T val ) {
    } catch( U val ) {
    } catch( A val ) {
    }
}
