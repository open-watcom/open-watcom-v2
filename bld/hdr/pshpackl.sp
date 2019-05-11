:: push packing used by C library
:segment LINUX
#pragma pack( __push, 8 )
:elsesegment RDOS | QNX
#pragma pack( __push, 1 )
:elsesegment DOS
#ifdef _M_IX86
 #pragma pack( __push, 1 )
#else
 #pragma pack( __push, 8 )
#endif
:endsegment
