#if WINVER == 0x100
 #ifdef __MEDIUM__
  #pragma library ( "win10m" )
 #elif defined(__COMPACT__)
  #pragma library ( "win10c" )
 #elif defined(__LARGE__)
  #pragma library ( "win10l" )
 #elif defined(__SMALL__)
  #pragma library ( "win10s" )
 #endif
#endif
