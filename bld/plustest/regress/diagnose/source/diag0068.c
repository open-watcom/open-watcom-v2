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

// can access PRO() in prologue but not in body
OK::OK()
{
    0,PUB();
    0,PRO();		// BAD
    0,PRI();		// BAD
}
// can access ~PRO() in epilogue but not in body
OK::~OK()
{
    pub->~PUB();
    pro->~PRO();	// BAD
    pri->~PRI();	// BAD
}
OK & OK::operator =( OK &x )
{
    *pub = x;
    *pro = x;		// BAD
    *pri = x;		// BAD
    return x;
}

struct ERR : PUB, PRI, PRO {
    ERR();
    ~ERR();
    ERR & operator =( ERR & );
};

// cannot access PRI() in prologue
// can access PRO() in prologue but not in body
ERR::ERR()
{
    0,PUB();
    0,PRO();		// BAD
    0,PRI();		// BAD
}
// cannot access ~PRI() in epilogue
// can access ~PRO() in epilogue but not in body
ERR::~ERR()
{
    pub->~PUB();
    pro->~PRO();	// BAD
    pri->~PRI();	// BAD
}
ERR & ERR::operator =( ERR &x )
{
    *pub = x;
    *pro = x;		// BAD
    *pri = x;		// BAD
    return x;
}
struct OK_EQ : PUB, PRO, PRI {
    int x;
};
struct ERR_EQ : PUB, PRO, PRI {
    int x;
};
void bar( OK_EQ *o1, OK_EQ *o2, ERR_EQ *e1, ERR_EQ *e2 )
{
    *o1 = *o2;
    *e1 = *e2;
}
