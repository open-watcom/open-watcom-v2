#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INCL_DOS
#define INCL_WIN
#define INCL_ERRORS
#include <os2.h>

// make access to member vars available
#include <owobject.ih>

#include <owobjrc.h>
#include <helper.h>

// ------------------------------------------------------------------

MRESULT EXPENTRY NbPageProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{

               OWObject      *somSelf = WinQueryWindowPtr( hwnd, QWL_USER);
               OWObjectData  *somThis = (somSelf) ? OWObjectGetData( somSelf) : NULL;
switch (msg)
   {
   case WM_INITDLG:
      {
                     PSZ           pszObjectId;

      // store window ptr
      WinSetWindowPtr( hwnd, QWL_USER, mp2);
      if (!mp2)
         break;

      // get access to object and object data
      somSelf = PVOIDFROMMP( mp2);
      somThis = (somSelf) ? OWObjectGetData( somSelf) : NULL;
      if (!somThis)
         break;

      // initialize some controls
      WinSendDlgItemMsg( hwnd, IDSPB_TESTVALUE, SPBM_SETLIMITS, MPFROMLONG( 10), MPFROMLONG( 0));

      // fill numeric testvalue
      WinSendDlgItemMsg( hwnd, IDSPB_TESTVALUE, SPBM_SETCURRENTVALUE, MPFROMLONG( _ulTestValue), 0);

      // fill object id field
      pszObjectId = _wpQueryObjectID( somSelf);
      if ((!pszObjectId) || (!*pszObjectId))
         pszObjectId = "(no object id given !!!)";
      WinSetDlgItemText( hwnd, IDENT_OBJECTID, pszObjectId);

      { // ##################################################
      // get temporary access to metaclass data, using the local somThis
      // in underscore macro to access the metaclass member var
      M_OWObjectData *somThis = M_OWObjectGetData( _OWObject);

      if (somThis)
         {
         // add contents to MLE
         WinSendDlgItemMsg( hwnd, IDDLG_MLE_README, MLM_INSERT, MPFROMP( _pszMleText), 0);
         // go to top of MLE
         WinSendDlgItemMsg( hwnd, IDDLG_MLE_README, MLM_SETSEL, 0, 0);

         } // if (somThis)

      } // ##################################################

      // enable GUI control update notifications
      _hwndNotebookPage = hwnd;

      return (MRESULT) FALSE;
      }
      break;

   /* ---------------------------------------------------------------- */

   case WM_USER_UPDATECONTROLS:
      // update controls that can be modified by setup strings
      WinSendDlgItemMsg( hwnd, IDSPB_TESTVALUE, SPBM_SETCURRENTVALUE, MPFROMLONG( _ulTestValue), 0);
      break;

   /* ---------------------------------------------------------------- */

   case WM_DESTROY:
      if (somThis)
         {
         // update member variables from GUI controls
         WinSendDlgItemMsg( hwnd, IDSPB_TESTVALUE, 
                            SPBM_QUERYVALUE, MPFROMP( &_ulTestValue), 0);

         // disable GUI control update notifications
         _hwndNotebookPage = NULLHANDLE;
         }


      break;

   } // end switch

return WinDefDlgProc( hwnd, msg, mp1, mp2);
}

