/*
  impossible enum
*/
#ifdef __WATCOM_INT64__
#error fill in test after MJC has scanner working
#else
enum g {
    EB = -70000L, EC = 3333333333
};
#endif
