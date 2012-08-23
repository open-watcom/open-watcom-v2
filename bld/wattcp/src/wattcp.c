#ifdef __HIGHC__

#if __HIGHC__ <= 0x0331
#include <language.cf>
#endif

#include "wattcp.h"
#include "strings.h"

#pragma Calling_convention(PASCAL)
#define DLL_PROCESS_DETACH 0    
#define DLL_PROCESS_ATTACH 1    

/*
 * For imported data, we redefine the imported name with the
 * -import switch when linking, and then use a macro to dereference
 * the imported name to get to the real variable.
 */

#define IMP_PREFIX(name)  imp_##name
#define IMPVAR(name,type) (***(type ***)((BYTE*)(&IMP_PREFIX(name))+2))

DWORD DllEntryPoint (void *hDll, DWORD reason, void *reserved)
{
#if defined(USE_DEBUG)
  if (reason == DLL_PROCESS_ATTACH)
     outs ("WATTCP.DLL: Process attach\n"); /* cannot use _LANG() here */
#endif
  return (1);
}

#endif   /* __HIGHC__ */
