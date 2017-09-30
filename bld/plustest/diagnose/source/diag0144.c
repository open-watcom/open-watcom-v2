// should diagnose cases where default ctor cannot be generated
struct Nothing {
    int _f;
};
struct HasDefault {
    int _f;
    HasDefault();
};
struct HasNonDefault {
    int _f;
    HasNonDefault( int );
};
//#pragma on(cdopt);

// as a direct non-virtual base
struct DNVB1 : Nothing { };
struct DNVB2 : HasDefault { };
struct DNVB3 : HasNonDefault { };
DNVB1 dnvb1;
DNVB2 dnvb2;
DNVB3 dnvb3;
// as a direct virtual base
struct DVB1 : virtual Nothing { };
struct DVB2 : virtual HasDefault { };
struct DVB3 : virtual HasNonDefault { };
DVB1 dvb1;
DVB2 dvb2;
DVB3 dvb3;
// as an indirect non-virtual base
struct X30DNVB1 : Nothing { };
struct X30DNVB2 : HasDefault { };
struct X30DNVB3 : HasNonDefault { };
struct INVB1 : X30DNVB1 {};
struct INVB2 : X30DNVB2 {};
struct INVB3 : X30DNVB3 {};
INVB1 invb1;
INVB2 invb2;
INVB3 invb3;
// as an indirect virtual base
struct X40DVB1 : virtual Nothing { };
struct X40DVB2 : virtual HasDefault { };
struct X40DVB3 : virtual HasNonDefault { };
struct IVB1 : X40DVB1 {};
struct IVB2 : X40DVB2 {};
struct IVB3 : X40DVB3 {};
IVB1 ivb1;
IVB2 ivb2;
IVB3 ivb3;
// as a member
struct M1 { Nothing x; };
struct M2 { HasDefault x; };
struct M3 { HasNonDefault x; };
M1 m1;
M2 m2;
M3 m3;
// as an array member
struct AM1 { Nothing x[2]; };
struct AM2 { HasDefault x[2]; };
struct AM3 { HasNonDefault x[2]; };
AM1 am1;
AM2 am2;
AM3 am3;
