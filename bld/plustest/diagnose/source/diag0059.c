struct B {
public:
    int B_pub;
private:
    int B_pri;
protected:
    int B_pro;
    friend void B_friend( void );
};

struct DPUB : public B {
public:
    int DPUB_pub;
private:
    int DPUB_pri;
protected:
    int DPUB_pro;
    friend void DPUB_friend( void );
};

struct DPRI : private B {
public:
    int DPRI_pub;
private:
    int DPRI_pri;
protected:
    int DPRI_pro;
    friend void DPRI_friend( void );
};

struct DPRO : protected B {
public:
    int DPRO_pub;
private:
    int DPRO_pri;
protected:
    int DPRO_pro;
    friend void DPRO_friend( void );
};

B *pB;
DPUB *pDPUB;
DPRI *pDPRI;
DPRO *pDPRO;

void foo( void )
{
    pB->B_pub = 1;
    pB->B_pri = 1;
    pB->B_pro = 1;
    pDPUB->DPUB_pub = 1;
    pDPUB->DPUB_pri = 1;
    pDPUB->DPUB_pro = 1;
    pDPRI->DPRI_pub = 1;
    pDPRI->DPRI_pri = 1;
    pDPRI->DPRI_pro = 1;
    pDPRO->DPRO_pub = 1;
    pDPRO->DPRO_pri = 1;
    pDPRO->DPRO_pro = 1;
    pDPUB->B_pub = 1;
    pDPUB->B_pri = 1;
    pDPUB->B_pro = 1;
    pDPRI->B_pub = 1;
    pDPRI->B_pri = 1;
    pDPRI->B_pro = 1;
    pDPRO->B_pub = 1;
    pDPRO->B_pri = 1;
    pDPRO->B_pro = 1;
}

void DPRI_friend( void )
{
    pB->B_pub = 1;
    pB->B_pri = 1;
    pB->B_pro = 1;
    pDPUB->DPUB_pub = 1;
    pDPUB->DPUB_pri = 1;
    pDPUB->DPUB_pro = 1;
    pDPRI->DPRI_pub = 1;
    pDPRI->DPRI_pri = 1;
    pDPRI->DPRI_pro = 1;
    pDPRO->DPRO_pub = 1;
    pDPRO->DPRO_pri = 1;
    pDPRO->DPRO_pro = 1;
    pDPUB->B_pub = 1;
    pDPUB->B_pri = 1;
    pDPUB->B_pro = 1;
    pDPRI->B_pub = 1;
    pDPRI->B_pri = 1;
    pDPRI->B_pro = 1;
    pDPRO->B_pub = 1;
    pDPRO->B_pri = 1;
    pDPRO->B_pro = 1;
}

void DPRO_friend( void )
{
    pB->B_pub = 1;
    pB->B_pri = 1;
    pB->B_pro = 1;
    pDPUB->DPUB_pub = 1;
    pDPUB->DPUB_pri = 1;
    pDPUB->DPUB_pro = 1;
    pDPRI->DPRI_pub = 1;
    pDPRI->DPRI_pri = 1;
    pDPRI->DPRI_pro = 1;
    pDPRO->DPRO_pub = 1;
    pDPRO->DPRO_pri = 1;
    pDPRO->DPRO_pro = 1;
    pDPUB->B_pub = 1;
    pDPUB->B_pri = 1;
    pDPUB->B_pro = 1;
    pDPRI->B_pub = 1;
    pDPRI->B_pri = 1;
    pDPRI->B_pro = 1;
    pDPRO->B_pub = 1;
    pDPRO->B_pri = 1;
    pDPRO->B_pro = 1;
}

void DPUB_friend( void )
{
    pB->B_pub = 1;
    pB->B_pri = 1;
    pB->B_pro = 1;
    pDPUB->DPUB_pub = 1;
    pDPUB->DPUB_pri = 1;
    pDPUB->DPUB_pro = 1;
    pDPRI->DPRI_pub = 1;
    pDPRI->DPRI_pri = 1;
    pDPRI->DPRI_pro = 1;
    pDPRO->DPRO_pub = 1;
    pDPRO->DPRO_pri = 1;
    pDPRO->DPRO_pro = 1;
    pDPUB->B_pub = 1;
    pDPUB->B_pri = 1;
    pDPUB->B_pro = 1;
    pDPRI->B_pub = 1;
    pDPRI->B_pri = 1;
    pDPRI->B_pro = 1;
    pDPRO->B_pub = 1;
    pDPRO->B_pri = 1;
    pDPRO->B_pro = 1;
}
