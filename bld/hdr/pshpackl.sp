:: push packing used by C library
:segment LINUX
#pragma pack( __push, 8 )
:elsesegment
#ifdef _M_IX86
 #pragma pack( __push, 1 )
#else
 #pragma pack( __push, 8 )
#endif
:endsegment
