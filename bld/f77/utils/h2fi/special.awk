function init_special_types() {
    # these are hand converted types from basedef.h
    conv_to_int[ "WORD" ] = 2;
    conv_to_int[ "DWORD" ] = 4;
    conv_to_int[ "INT" ] = 4;
    conv_to_int[ "UINT" ] = 4;
    conv_to_int[ "BOOL" ] = 4;
    conv_to_int[ "CHAR" ] = 1;
    conv_to_int[ "UCHAR" ] = 1;
    conv_to_int[ "SHORT" ] = 2;
    conv_to_int[ "USHORT" ] = 2;
    conv_to_int[ "LONG" ] = 4;
    conv_to_int[ "ULONG" ] = 4;
    conv_to_int[ "BYTE" ] = 1;
    conv_to_int[ "PVOID" ] = 4;
    conv_to_int[ "NPVOID" ] = 4;
    conv_to_int[ "PCH" ] = 4;
    is_a_reference[ "PFN" ] = 1;
    # these are hand converted from os2def.h
    conv_to_int[ "APIRET" ] = 4;
    conv_to_int[ "APIRET16" ] = 2;
    conv_to_int[ "APIRET32" ] = 4;
    conv_to_int[ "SHANDLE" ] = 2;
    conv_to_int[ "LHANDLE" ] = 4;
    conv_to_int[ "BOOL" ] = 4;
    conv_to_int[ "BOOL16" ] = 2;
    conv_to_int[ "BOOL32" ] = 4;
    conv_to_int[ "SEL" ] = 2;
    conv_to_int[ "PSZ" ] = 4;
    # these are used in mi.h
    conv_to_int[ "uchar_t" ] = 1;
    conv_to_int[ "ushort_t" ] = 2;
    conv_to_int[ "ulong_t" ] = 4;
    # from bsesub.h
    conv_to_int[ "HKBD" ] = 2;
    conv_to_int[ "HVIO" ] = 2;
    conv_to_int[ "HMOU" ] = 2;
    # from rexxsaa.h
    is_a_struct[ "RXSTRING" ] = 1;
    # from pmspl.h
    conv_to_int[ "SPLERR" ] = 4;
    # types from d:\f77\src\os2\pmwin.h
    conv_to_int[ "MPARAM" ] = 4;
}
