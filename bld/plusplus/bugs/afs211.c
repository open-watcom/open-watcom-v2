template <class Type>
class Cont {
public:
    Type a;
    Cont(){};
    ~Cont(){};
};


template <class Type>
class d{
public:
    typedef Cont<Type> Cont_Type;

    my_delete( Cont<Type> *old ){
#if 0
        old->~Cont<Type>();
#else
        old->~Cont_Type();
#endif
        delete (void *)old;
    }
};

void main( void ){
    Cont<int> *a;
    d<int> b;

    a = new Cont<int>;
    b.my_delete( a );
}
