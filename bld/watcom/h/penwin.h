#ifndef	__PENWIN_H
#define	__PENWIN_H

#ifndef	__WINDOWS_H
#include <windows.h>
#endif

#ifdef	__cplusplus
extern	"C" {
#endif

#pragma	pack(push,1)

#define	RC_LDEFAULT	(0xFFFFFFFFL)
#define	MAXOEMDATAWORDS	6
#define	MAXDICTIONARIES	16
#define	cbRcLanguageMax	44
#define	cbRcUserMax	32
#define	cbRcrgbfAlcMax	32
#define	cwRcReservedMax	8
#define	MAXHOTSPOT	8

typedef	int	REC;
typedef	LONG	SYV;
typedef	SYV FAR *LPSYV;
typedef	HANDLE	HREC;
typedef	int	CL;
typedef	LONG	ALC;
typedef	UINT	HKP;
typedef	int (CALLBACK *LPDF)(int,LPVOID,LPVOID,int,DWORD,DWORD);
typedef	struct tagPENPACKET {
	UINT	wTabletX;
	UINT	wTabletY;
	UINT	wPDK;
	UINT	rgwOemData[MAXOEMDATAWORDS];
} PENPACKET,FAR *LPPENPACKET;
typedef	BOOL (CALLBACK *LPFNRAWHOOK)(LPPENPACKET);
typedef	struct tagGUIDE {
	int	xOrigin;
	int	yOrigin;
	int	cxBox;
	int	cyBox;
	int	cxBase;
	int	cyBase;
	int	cHorzBox;
	int	cVertBox;
	int	cyMid;
} GUIDE,FAR *LPGUIDE;
typedef	BOOL(CALLBACK *RCYIELDPROC)(VOID);
typedef	struct tagRC {
	HREC	hrec;
	HWND	hwnd;
	UINT	wEventRef;
	UINT	wRcPreferences;
	LONG	lRcOptions;
	RCYIELDPROC	lpfnYield;
	BYTE	lpUser[cbRcUserMax];
	UINT	wCountry;
	UINT	wIntlPreferences;
	char	lpLanguage[cbRcLanguageMax];
	LPDF	rglpdf[MAXDICTIONARIES];
	UINT	wTryDictionary;
	CL	clErrorLevel;
	ALC	alc;
	ALC	alcPriority;
	BYTE	rgbfAlc[cbRcrgbfAlcMax];
	UINT	wResultMode;
	UINT	wTimeOut;
	LONG	lPcm;
	RECT	rectBound;
	RECT	rectExclude;
	GUIDE	guide;
	UINT	wRcOrient;
	UINT	wRcDirect;
	int	nInkWidth;
	COLORREF	rgbInk;
	DWORD	dwAppParam;
	DWORD	dwDictParam;
	DWORD	dwRecognizer;
	UINT	rgwReserved[cwRcReservedMax];
} RC,FAR *LPRC;
typedef	HANDLE	HPENDATA;
typedef	struct tagSYC {
	UINT	wStrokeFirst;
	UINT	wPntFirst;
	UINT	wStrokeLast;
	UINT	wPntLast;
	BOOL	fLastSyc;
} SYC,FAR *LPSYC;
typedef	struct tagSYE {
	SYV	syv;
	LONG	lRecogVal;
	CL	cl;
	int	iSyc;
} SYE,FAR *LPSYE;
typedef	struct tagSYG {
	POINT	rgpntHotSpots[MAXHOTSPOT];
	int	cHotSpot;
	int	nFirstBox;
	LONG	lRecogVal;
	LPSYE	lpsye;
	int	cSye;
	LPSYC	lpsyc;
	int	cSyc;
} SYG,FAR *LPSYG;
typedef	int (CALLBACK *ENUMPROC)(LPSYV,int,LPVOID);
BOOL	WINAPI	SetPenHook(HKP,LPFNRAWHOOK);
REC	WINAPI	Recognize(LPRC);
REC	WINAPI	RecognizeData(LPRC,HPENDATA);
BOOL	WINAPI	TrainInk(LPRC,HPENDATA,LPSYV);
REC	WINAPI	ProcessWriting(HWND,LPRC);
BOOL	WINAPI	CorrectWriting(HWND,LPSTR,UINT,LPRC,DWORD,DWORD);
UINT	WINAPI	EnumSymbols(LPSYG,WORD,ENUMPROC,LPVOID);
BOOL	WINAPI	DictionarySearch(LPRC,LPSYE,int,LPSYV,int);

#pragma	pack(pop)

#ifdef	__cplusplus
}
#endif

#endif
