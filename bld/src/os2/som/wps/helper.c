#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INCL_DOS
#define INCL_WIN
#define INCL_ERRORS
#include <os2.h>

#ifndef DosQueryModFromEIP
APIRET APIENTRY  DosQueryModFromEIP(HMODULE *phMod,
                                     ULONG *pObjNum,
                                     ULONG BuffLen,
                                     PCHAR pBuff,
                                     ULONG *pOffset,
                                     ULONG Address);
#endif

// ------------------------------------------------------------------

HMODULE GetModuleHandle( VOID)
{
         HMODULE        hmod = NULLHANDLE;
         ULONG          ulObjectNumber  = 0;
         ULONG          ulOffset        = 0;
         CHAR           szModName[ _MAX_PATH];
DosQueryModFromEIP( &hmod, &ulObjectNumber,
                    sizeof( szModName), szModName,
                    &ulOffset, (ULONG) GetModuleHandle);
return hmod;
}

// ------------------------------------------------------------------

PVOID GetTextfileResourceData( HMODULE hmod, ULONG ulResourceType,
                               ULONG ulResId)
{
         APIRET         rc = NO_ERROR;
         PVOID          pvFileContents  = NULL;
         PSZ            pszFileContents = NULL;
         ULONG          ulResourceSize;
         PSZ            pszEof;

do {
   // copy read-only resource to own memory to
   // - append zero byte
   // - discard EOF character
   rc = DosGetResource( hmod, ulResourceType, ulResId, &pvFileContents);
   if (rc != NO_ERROR)
      break;
   DosQueryResourceSize( hmod, ulResourceType, ulResId, &ulResourceSize);
   pszFileContents = malloc( ulResourceSize + 1);
   if (!pszFileContents)
      break;
   memcpy( pszFileContents, pvFileContents, ulResourceSize);
   pszEof = pszFileContents + ulResourceSize;
   *pszEof = 0;
   pszEof--;
   if (*pszEof == 0x1a)
      *pszEof = 0;

   } while (FALSE);

return pszFileContents;
}

