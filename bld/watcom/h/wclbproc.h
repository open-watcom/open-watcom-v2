/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2014-2017 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  WIN16/WIN386 MakeProcInstance.../FreeProcInstance...
*               for callback function prototypes
*
****************************************************************************/


#ifdef __cplusplus
extern "C" {
#endif

#if defined( __WINDOWS_386__ )
typedef INT_PTR             (CALLBACK *DLGPROCx)(HWND,UINT,WPARAM,LPARAM);
typedef void                (CALLBACK *FARPROCx)(void);
typedef int                 (CALLBACK *FONTENUMPROCx)(const LOGFONT *,const TEXTMETRIC *,int,LPARAM);
typedef int                 (CALLBACK *OLDFONTENUMPROCx)(const ENUMLOGFONT *,const NEWTEXTMETRIC *,int,LPARAM);
typedef LRESULT             (CALLBACK *HOOKPROCx)(int,WPARAM,LPARAM);
typedef BOOL                (CALLBACK *WNDENUMPROCx)(HWND,LPARAM);
typedef UINT_PTR            (CALLBACK *__CDHOOKPROCx)(HWND,UINT,WPARAM,LPARAM);
typedef LRESULT             (CALLBACK *WNDPROCx)(HWND,UINT,WPARAM,LPARAM);
typedef __CDHOOKPROCx       LPOFNHOOKPROCx;
#else
#define DLGPROCx            DLGPROC
#define FARPROCx            FARPROC
#define FONTENUMPROCx       FONTENUMPROC
#define OLDFONTENUMPROCx    OLDFONTENUMPROC
#define HOOKPROCx           HOOKPROC
#define LPOFNHOOKPROCx      LPOFNHOOKPROC
#define WNDENUMPROCx        WNDENUMPROC
#define WNDPROCx            WNDPROC
#endif

#if defined( __WINDOWS__ )
extern WNDPROC              GetWndProc( WNDPROCx p );
extern DLGPROC              MakeProcInstance_DLG( DLGPROCx fn, HINSTANCE instance );
extern FONTENUMPROC         MakeProcInstance_FONTENUM( FONTENUMPROCx fn, HINSTANCE instance );
extern OLDFONTENUMPROC      MakeProcInstance_OLDFONTENUM( OLDFONTENUMPROCx fn, HINSTANCE instance );
extern HOOKPROC             MakeProcInstance_HOOK( HOOKPROCx fn, HINSTANCE instance );
extern LPOFNHOOKPROC        MakeProcInstance_OFNHOOK( LPOFNHOOKPROCx fn, HINSTANCE instance );
extern WNDENUMPROC          MakeProcInstance_WNDENUM( WNDENUMPROCx fn, HINSTANCE instance );
extern WNDPROC              MakeProcInstance_WND( WNDPROCx fn, HINSTANCE instance );
#else
#define GetWndProc(p)                       p
#define MakeProcInstance_DLG(f,i)           f
#define MakeProcInstance_FONTENUM(f,i)      f
#define MakeProcInstance_OLDFONTENUM(f,i)   f
#define MakeProcInstance_HOOK(f,i)          f
#define MakeProcInstance_OFNHOOK(f,i)       f
#define MakeProcInstance_WNDENUM(f,i)       f
#define MakeProcInstance_WND(f,i)           f
#endif

#if defined( __WINDOWS__ ) && defined( _M_I86 )
void FreeProcInstance_DLG( DLGPROC f );
void FreeProcInstance_FONTENUM( FONTENUMPROC f);
void FreeProcInstance_OLDFONTENUM( OLDFONTENUMPROC f);
void FreeProcInstance_HOOK( HOOKPROC f );
void FreeProcInstance_OFNHOOK( LPOFNHOOKPROC f );
void FreeProcInstance_WNDENUM( WNDENUMPROC f );
void FreeProcInstance_WND( WNDPROC f );
#else
#define FreeProcInstance_DLG(f)
#define FreeProcInstance_FONTENUM(f)
#define FreeProcInstance_OLDFONTENUM(f)
#define FreeProcInstance_HOOK(f)
#define FreeProcInstance_OFNHOOK(f)
#define FreeProcInstance_WNDENUM(f)
#define FreeProcInstance_WND(f)
#endif

#ifdef __cplusplus
}
#endif
