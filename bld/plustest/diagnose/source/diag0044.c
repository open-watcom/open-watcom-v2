struct V {
    int : 2;
};

struct UV : public virtual V {
    int : 2;
};

struct RV : private virtual V {
    friend class RM;
    int : 2;
};

struct OV : protected virtual V {
    int : 2;
};

struct UM : public UV, public RV, public OV {
    int : 2;
};

struct RM : private UV, private RV, private OV {
    int : 2;
    void foo( void );
};

struct OM : protected UV, protected RV, protected OV {
    int : 2;
};

struct OMD : public OM {
    int : 2;
    void foo( void );
};

V *pV;
UV *pUV;
RV *pRV;
OV *pOV;
UM *pUM;
RM *pRM;
OM *pOM;

void foo( void )
{
    pV = pUV;		// OK
    pV = pRV;
    pV = pOV;
    pV = pUM;		// OK
    pV = pRM;
    pV = pOM;
}

void RM::foo( void )
{
    pV = pUV;		// OK
    pV = pRV;		// OK
    pV = pOV;
    pV = pUM;		// OK
    pV = pRM;		// OK
    pV = pOM;
}

void OMD::foo( void )
{
    pV = pUV;		// OK
    pV = pRV;
    pV = pOV;
    pV = pUM;		// OK
    pV = pRM;
    pV = pOM;
}
