
#ifndef HELPER_H
#define HELPER_H

HMODULE GetModuleHandle( VOID);
PVOID GetTextfileResourceData( HMODULE hmod, ULONG ulResourceType,
                               ULONG ulResId);

#endif // HELPER_H

