#ifndef DX_SHARED_DEFINES
#define DX_SHARED_DEFINES

/* Direct3D color data type */
#ifndef D3DCOLOR_DEFINED
#define D3DCOLOR_DEFINED
typedef DWORD   D3DCOLOR;
#endif

/* DirectX shared primitive data types */
typedef float   D3DVALUE;
typedef float   *LPD3DVALUE;
typedef DWORD   *LPD3DCOLOR;

/* Direct3D vectors */
typedef struct _D3DVECTOR {
    union {
        D3DVALUE    x;
        D3DVALUE    dvX;
    };
    union {
        D3DVALUE    y;
        D3DVALUE    dvY;
    };
    union {
        D3DVALUE    z;
        D3DVALUE    dvZ;
    };
} D3DVECTOR;
typedef D3DVECTOR   *LPD3DVECTOR;

#endif /* DX_SHARED_DEFINES */
