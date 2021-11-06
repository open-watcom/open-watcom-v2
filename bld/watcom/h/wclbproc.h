/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2014-2020 The Open Watcom Contributors. All Rights Reserved.
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

#if defined( __WINDOWS__ )
#if defined( _M_I86 )   /* 16-bit Windows 3.1x */

extern void MAKEPROCINSTANCE_WIN(void);
#pragma aux MAKEPROCINSTANCE_WIN "MAKEPROCINSTANCE"
extern void FREEPROCINSTANCE_WIN(void);
#pragma aux FREEPROCINSTANCE_WIN "FREEPROCINSTANCE"

#define MAKEPROCINSTANCE_INLINE \
        "push dx" \
        "push ax" \
        "push bx" \
        "call far ptr MAKEPROCINSTANCE_WIN" \
    __parm [__dx __ax] [__bx] __value [__dx __ax] __modify [__cx __es]

#define FREEPROCINSTANCE_INLINE \
        "push dx" \
        "push ax" \
        "call far ptr FREEPROCINSTANCE_WIN" \
    __parm [__dx __ax] __modify [__bx __cx __es]


#define GETWNDPROC_INLINE       __parm [__dx __ax] __value [__dx __ax]

#else   /* 32-bit extender Windows 3.1x */

#define MAKEPROCINSTANCE_INLINE __parm [__dx __eax] [__bx] __value [__eax]
#define GETWNDPROC_INLINE       __parm [__dx __eax] __value [__eax]

#endif
#endif

#if defined( __WINDOWS_386__ )
typedef INT_PTR             (CALLBACK *DLGPROCx)(HWND,UINT,WPARAM,LPARAM);
typedef void                (CALLBACK *FARPROCx)(void);
typedef int                 (CALLBACK *FONTENUMPROCx)(const LOGFONT *,const TEXTMETRIC *,int,LPARAM);
typedef int                 (CALLBACK *OLDFONTENUMPROCx)(const ENUMLOGFONT *,const NEWTEXTMETRIC *,int,LPARAM);
typedef LRESULT             (CALLBACK *HOOKPROCx)(int,WPARAM,LPARAM);
typedef BOOL                (CALLBACK *WNDENUMPROCx)(HWND,LPARAM);
typedef LRESULT             (CALLBACK *WNDPROCx)(HWND,UINT,WPARAM,LPARAM);
typedef void                (CALLBACK *TIMERPROCx)( HWND, UINT, UINT_PTR, DWORD );
#if defined( INCLUDE_COMMDLG_H )
typedef UINT_PTR            (CALLBACK *__CDHOOKPROCx)(HWND,UINT,WPARAM,LPARAM);
typedef __CDHOOKPROCx       LPOFNHOOKPROCx;
#endif
#else
#define DLGPROCx            DLGPROC
#define FARPROCx            FARPROC
#define FONTENUMPROCx       FONTENUMPROC
#define OLDFONTENUMPROCx    OLDFONTENUMPROC
#define HOOKPROCx           HOOKPROC
#define WNDENUMPROCx        WNDENUMPROC
#define WNDPROCx            WNDPROC
#define TIMERPROCx          TIMERPROC
#if !defined( __WINDOWS__ ) || defined( INCLUDE_COMMDLG_H )
#define LPOFNHOOKPROCx      LPOFNHOOKPROC
#endif
#endif

#if defined( __WINDOWS__ )
extern WNDPROC              GetWndProc( WNDPROCx p );
#pragma aux GetWndProc = GETWNDPROC_INLINE

extern DLGPROC              MakeProcInstance_DLG( DLGPROCx fn, HINSTANCE instance );
extern FONTENUMPROC         MakeProcInstance_FONTENUM( FONTENUMPROCx fn, HINSTANCE instance );
extern OLDFONTENUMPROC      MakeProcInstance_OLDFONTENUM( OLDFONTENUMPROCx fn, HINSTANCE instance );
extern HOOKPROC             MakeProcInstance_HOOK( HOOKPROCx fn, HINSTANCE instance );
extern WNDENUMPROC          MakeProcInstance_WNDENUM( WNDENUMPROCx fn, HINSTANCE instance );
extern WNDPROC              MakeProcInstance_WND( WNDPROCx fn, HINSTANCE instance );
extern TIMERPROC            MakeProcInstance_TIMER( TIMERPROCx fn, HINSTANCE instance );
#if defined( INCLUDE_COMMDLG_H )
extern LPOFNHOOKPROC        MakeProcInstance_OFNHOOK( LPOFNHOOKPROCx fn, HINSTANCE instance );
#endif
#pragma aux MakeProcInstance_DLG         = MAKEPROCINSTANCE_INLINE
#pragma aux MakeProcInstance_FONTENUM    = MAKEPROCINSTANCE_INLINE
#pragma aux MakeProcInstance_OLDFONTENUM = MAKEPROCINSTANCE_INLINE
#pragma aux MakeProcInstance_HOOK        = MAKEPROCINSTANCE_INLINE
#pragma aux MakeProcInstance_WNDENUM     = MAKEPROCINSTANCE_INLINE
#pragma aux MakeProcInstance_WND         = MAKEPROCINSTANCE_INLINE
#pragma aux MakeProcInstance_TIMER       = MAKEPROCINSTANCE_INLINE
#if defined( INCLUDE_COMMDLG_H )
#pragma aux MakeProcInstance_OFNHOOK     = MAKEPROCINSTANCE_INLINE
#endif
#else
#define GetWndProc(p)                       p
#define MakeProcInstance_DLG(f,i)           ((void)i,f)
#define MakeProcInstance_FONTENUM(f,i)      ((void)i,f)
#define MakeProcInstance_OLDFONTENUM(f,i)   ((void)i,f)
#define MakeProcInstance_HOOK(f,i)          ((void)i,f)
#define MakeProcInstance_OFNHOOK(f,i)       ((void)i,f)
#define MakeProcInstance_WNDENUM(f,i)       ((void)i,f)
#define MakeProcInstance_WND(f,i)           ((void)i,f)
#define MakeProcInstance_TIMER(f,i)         ((void)i,f)
#endif

#if defined( __WINDOWS__ ) && defined( _M_I86 )
extern void FreeProcInstance_DLG( DLGPROC f );
extern void FreeProcInstance_FONTENUM( FONTENUMPROC f);
extern void FreeProcInstance_OLDFONTENUM( OLDFONTENUMPROC f);
extern void FreeProcInstance_HOOK( HOOKPROC f );
extern void FreeProcInstance_WNDENUM( WNDENUMPROC f );
extern void FreeProcInstance_WND( WNDPROC f );
extern void FreeProcInstance_TIMER( TIMERPROC f );
#if defined( INCLUDE_COMMDLG_H )
extern void FreeProcInstance_OFNHOOK( LPOFNHOOKPROC f );
#endif
#pragma aux FreeProcInstance_DLG         = FREEPROCINSTANCE_INLINE
#pragma aux FreeProcInstance_FONTENUM    = FREEPROCINSTANCE_INLINE
#pragma aux FreeProcInstance_OLDFONTENUM = FREEPROCINSTANCE_INLINE
#pragma aux FreeProcInstance_HOOK        = FREEPROCINSTANCE_INLINE
#pragma aux FreeProcInstance_WNDENUM     = FREEPROCINSTANCE_INLINE
#pragma aux FreeProcInstance_WND         = FREEPROCINSTANCE_INLINE
#pragma aux FreeProcInstance_TIMER       = FREEPROCINSTANCE_INLINE
#if defined( INCLUDE_COMMDLG_H )
#pragma aux FreeProcInstance_OFNHOOK     = FREEPROCINSTANCE_INLINE
#endif
#else
#define FreeProcInstance_DLG(f)             ((void)f)
#define FreeProcInstance_FONTENUM(f)        ((void)f)
#define FreeProcInstance_OLDFONTENUM(f)     ((void)f)
#define FreeProcInstance_HOOK(f)            ((void)f)
#define FreeProcInstance_OFNHOOK(f)         ((void)f)
#define FreeProcInstance_WNDENUM(f)         ((void)f)
#define FreeProcInstance_WND(f)             ((void)f)
#define FreeProcInstance_TIMER(f)           ((void)f)
#endif

#ifdef __cplusplus
}
#endif
