/*
  impossible enum
*/
#ifdef __WATCOM_INT64__
enum a {
    A1 = -0x7FFFFFFFFFFFFFFF,
    A2 = 0xFFFFFFFFFFFFFFFF,
    A3 = -0x7FFFFFFFFFFFFFFF
} A;

enum b {
    B1 = 0xFFFFFFFFFFFFFFFF,
    B2 = -0x7FFFFFFFFFFFFFFF,
    B3 = 0xFFFFFFFFFFFFFFFF
} B;
#else
enum g {
    EB = -70000L, EC = 3333333333
};
#endif
