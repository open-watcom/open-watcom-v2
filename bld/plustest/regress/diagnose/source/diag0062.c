// generating ctors,dtors, and assignment operators should only
// report access errors on private accesses of immediate base classes
struct PRI {
private:
    PRI();
    ~PRI();
    PRI & operator =( PRI & );
};
struct PRO {
protected:
    PRO();
    ~PRO();
    PRO & operator =( PRO & );
};
struct PUB {
public:
    PUB();
    ~PUB();
    PUB & operator =( PUB & );
};

PRI *pri;
PRO *pro;
PUB *pub;

struct OK : PUB, PRO {
    OK();
    ~OK();
    OK & operator =( OK & );
};

struct ERR : PUB, PRI, PRO {
    ERR();
    ~ERR();
    ERR & operator =( ERR & );
};
struct OK_EQ : PUB, PRO, PRI {
    int x;
};
struct ERR_EQ : PUB, PRO, PRI {
    int x;
};
void bar( OK_EQ *o1, OK_EQ *o2, ERR_EQ *e1, ERR_EQ *e2 )
{
    OK_EQ ok1 = *o1;
    OK_EQ ok2;
    ERR_EQ err1 = *e1;
    ERR_EQ err2;

    *o1 = *o2;
    *e1 = *e2;
}
