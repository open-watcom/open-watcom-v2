
class TEM {
    public:
       TEM( int size=5 ) { };
       ~TEM();
};

class C {
    typedef TEM y;
    y x;

    public:
       C();
       void w();
};
