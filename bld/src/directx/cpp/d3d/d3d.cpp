//---------------------------------------------------------------------------------------
//
// Simple Windows Direct3D Example Code
//
// Jim Shaw 12/3/2004
//
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
// Open Watcom doesn't have this yet
//---------------------------------------------------------------------------------------
#ifdef __WATCOMC__
float sqrtf(float);
#endif

//---------------------------------------------------------------------------------------
// Always include windows.h for windows programs
//---------------------------------------------------------------------------------------
#include <windows.h>
#include <d3d9.h>
#include <d3dx9math.h>

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <assert.h>

//---------------------------------------------------------------------------------------
// Screen dimensions and options
//---------------------------------------------------------------------------------------
#define SCREEN_X   640
#define SCREEN_Y   480
#define RUN_WINDOWED 1
#define USE_ZBUFFER  1

//---------------------------------------------------------------------------------------
// Variable to hold the address of the Windows window
//---------------------------------------------------------------------------------------
HWND game_window;

//---------------------------------------------------------------------------------------
// Name of our window class.  every class has a name.  every window is one of a class.
// this can be pretty much anything
//---------------------------------------------------------------------------------------
char game_class[]="D3DCPPWindow";
static int quit = 0;

//---------------------------------------------------------------------------------------
// function prototypes
//---------------------------------------------------------------------------------------
void draw(void);
void flip(void);
int init_d3d(int, int);
int init_cube(void);
void init_textures(void);
void animate(void);
void set_view(void);
void shutdown_d3d(void);

//---------------------------------------------------------------------------------------
// Direct3D Variables
//---------------------------------------------------------------------------------------
IDirect3D9 *d3d=NULL;
IDirect3DDevice9 *d3d_dev=NULL;
IDirect3DVertexBuffer9 *vertex_buffer=NULL;
IDirect3DTexture9 *texture[6]={0};
int textures_dynamic;
int zbuffer_enabled;
D3DFORMAT texture_format;

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)

typedef struct
{
    D3DXVECTOR3 position; // The position
    D3DCOLOR    color;    // The color
    FLOAT       tu, tv;   // The texture coordinates
} CUSTOMVERTEX;

//---------------------------------------------------------------------------------------
// Cube animation parameters
//---------------------------------------------------------------------------------------
float yaw=0.0f, pitch=0.0f, roll=0.0f, scale=20.0f;
float x=0.0f, y=0.0f, z=100.0f;
float xr=0.0f, yr=0.0f, zr=0.0f;
int animating=1;

//---------------------------------------------------------------------------------------
// Every window has a WindowProc which handles messages Windows sends to them.  These
// are things like key presses, mouse clicks, and thousands of other things you might
// be interested in.
//---------------------------------------------------------------------------------------
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        //this message gets sent to the window when you click the close box on it
        case WM_DESTROY:
            //tell Windows to close the app
            PostQuitMessage(0);
            quit = 1;
            break;

        //this message gets sent when you press a key
        case WM_KEYDOWN:
            switch (wParam)
            {
                //pressed Escape key?
                case VK_ESCAPE:
                    //tell Windows to close the app
                    PostQuitMessage(0);
                    quit = 1;
                    break;

                //keyboard controls
                case 'A':       animating ^= 1; break;
                case 'R':
                    yaw = 0.0f;
                    pitch = 0.0f;
                    roll = 0.0f;
                    x=0.0f;
                    y=0.0f;
                    z=100.0f;
                    scale=20.0f;
                    xr=0.0f;
                    yr=0.0f;
                    zr=0.0f;
                    break;
                case VK_UP:     y += 10.0f; break;
                case VK_DOWN:   y -= 10.0f; break;
                case VK_LEFT:   x -= 10.0f; break;
                case VK_RIGHT:  x += 10.0f; break;
                case VK_PRIOR:  z += 10.0f; break;
                case VK_NEXT:   z -= 10.0f; break;

                case VK_NUMPAD7: yaw += 0.1f; break;
                case VK_NUMPAD4: yaw -= 0.1f; break;
                case VK_NUMPAD8: pitch += 0.1f; break;
                case VK_NUMPAD5: pitch -= 0.1f; break;
                case VK_NUMPAD9: roll += 0.1f; break;
                case VK_NUMPAD6: roll -= 0.1f; break;

                case VK_ADD:      scale = scale + 10.0f; break;
                case VK_SUBTRACT: scale = scale - 10.0f; if (scale < 0.0f) scale = 0.0f; break;

            }
            break;

        //any message you don't process gets passed to this function inside Windows.
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    //any message you do process, return 0 (unless the message documentation says otherwise)
return 0;
}

//---------------------------------------------------------------------------------------
// WinMain
//---------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

    WNDCLASSEX clas;
    MSG msg;
    int style;
    RECT rect;
    int time;
    int elapsed;

    //Here we create the Class we named above
    clas.cbSize = sizeof(WNDCLASSEX);
    clas.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    clas.lpfnWndProc = WindowProc;//<- tell it where the WindowProc is
    clas.cbClsExtra = 0;
    clas.cbWndExtra = 0;
    clas.hInstance = hInstance;
    clas.hIcon = NULL;
    clas.hCursor = NULL;
    clas.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);//<- background colour of window
    clas.lpszMenuName = NULL;
    clas.lpszClassName = game_class;//<- the class name
    clas.hIconSm = 0;
    //do it!
    RegisterClassEx(&clas);

    //style of the window - what boxes do we need (close minimised etc)
    style = WS_CAPTION|WS_SYSMENU|WS_MAXIMIZEBOX|WS_MINIMIZEBOX;
    //create the window
    game_window = CreateWindowEx(0, game_class, "Direct3D", style, CW_USEDEFAULT, CW_USEDEFAULT, 1,1, NULL, NULL, hInstance, 0);

    //adjust the window size so that a SCREEN_X x SCREEN_Y window will fit inside its frame
    rect.left = rect.top = 0;
    rect.right = SCREEN_X;
    rect.bottom = SCREEN_Y;
    AdjustWindowRectEx(&rect, style , FALSE, 0);
    SetWindowPos(game_window, NULL, 0,0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE|SWP_NOZORDER);

    if (init_d3d(RUN_WINDOWED, USE_ZBUFFER) == 0)
    {
        MessageBox(NULL, "A problem occurred creating the Direct3D Device.\nCheck your video drivers and "
                            "DirectX version.\nThis demo requires DirectX9.0 or better.", "DirectX Initialisation Error", MB_ICONWARNING|MB_OK);
        shutdown_d3d();
        return 0;
    }

    //show the window on the desktop
    ShowWindow(game_window, nCmdShow);

    //message processing loop
    //all Windows programs have one of these.  It receives the messages Windows sends to the program and
    //passes them to the WindowProc in the Class we registered for the window.
    quit = 0;
    do
    {
        //Are there any messages waiting?
        while (PeekMessage(&msg, game_window, 0, 0, PM_NOREMOVE))
        {
            //yes!  read it.
            if (GetMessage(&msg, game_window, 0,0) < 0)
                break;

            //pass the message to WindowProc
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        //get the current time
        time = GetTickCount();

        //draw my game!
        animate();
        draw();
        flip();
        //find out how long the frame took to draw
        elapsed = GetTickCount()-time;

    } while (!quit);

    shutdown_d3d();

    return 0;
}

//---------------------------------------------------------------------------------------
// Animate the cube
//---------------------------------------------------------------------------------------
void animate(void)
{
    if (animating)
    {
        yaw   += 0.01f;
        pitch += 0.02f;
        roll  += 0.03f;

        xr += 0.1f;
        yr += 0.2f;
        zr += 0.3f;

        x = (float)sin((double)xr)*10.0f;
        y = (float)sin((double)yr)*10.0f;
        z = (float)sin((double)zr)*10.0f+100.0f;
    }
}

//---------------------------------------------------------------------------------------
// Initialise all the direct3d structures we need
//---------------------------------------------------------------------------------------
int init_d3d(int windowed, int zbuffered)
{
    D3DCAPS9 d3dcaps9;
    D3DDISPLAYMODE d3ddm;
    D3DPRESENT_PARAMETERS d3dpp;
    D3DCAPS9 caps;
    HRESULT hr;
    D3DFORMAT depth_format;

    // Create the D3D device
    d3d = Direct3DCreate9(D3D_SDK_VERSION);
    if (d3d == NULL)
        return 0;

    // Get some information about the device and display adapter
    hr = d3d->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &d3dcaps9);

    if (windowed)
        hr = d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);
    else
        d3ddm.Format = D3DFMT_X8R8G8B8;

    // Work out a depth format
    depth_format = D3DFMT_D32;
    hr = d3d->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3ddm.Format, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D32);
    if (hr != D3D_OK)
    {
        depth_format = D3DFMT_D16;
        hr = d3d->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3ddm.Format, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D16);
        if (hr != D3D_OK)
            return 0;
    }

    // Work out a texture format
    texture_format = D3DFMT_A8R8G8B8;
    hr = d3d->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3ddm.Format, 0, D3DRTYPE_TEXTURE, D3DFMT_A8R8G8B8);
    if (hr != D3D_OK)
    {
        texture_format = D3DFMT_X8R8G8B8;
        hr = d3d->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3ddm.Format, 0, D3DRTYPE_TEXTURE, D3DFMT_X8R8G8B8);
        if (hr != D3D_OK)
        {
            texture_format = D3DFMT_A1R5G5B5;
            hr = d3d->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3ddm.Format, 0, D3DRTYPE_TEXTURE, D3DFMT_A1R5G5B5);
            if (hr != D3D_OK)
            {
                texture_format = D3DFMT_R5G6B5;
                hr = d3d->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3ddm.Format, 0, D3DRTYPE_TEXTURE, D3DFMT_R5G6B5);
                if (hr != D3D_OK)
                    return 0;
            }
        }
    }

    // Fill in the Presentation Parameters
    memset(&d3dpp, 0, sizeof d3dpp);

    if (windowed)
    {
        d3dpp.Windowed              = TRUE;
    }
    else
    {
        d3dpp.Windowed              = FALSE;
        d3dpp.BackBufferWidth       = SCREEN_X;
        d3dpp.BackBufferHeight      = SCREEN_Y;
        d3dpp.BackBufferCount       = 1;
        d3dpp.PresentationInterval  = D3DPRESENT_INTERVAL_ONE;
        d3dpp.FullScreen_RefreshRateInHz = 60;
    }

    if (zbuffered)
    {
        d3dpp.EnableAutoDepthStencil = TRUE;
        d3dpp.AutoDepthStencilFormat = depth_format;
    }
    else
    {
        d3dpp.EnableAutoDepthStencil = FALSE;
        d3dpp.AutoDepthStencilFormat = D3DFMT_UNKNOWN;
    }

    d3dpp.SwapEffect                = D3DSWAPEFFECT_FLIP;
    d3dpp.BackBufferFormat          = d3ddm.Format;
    d3dpp.hDeviceWindow             = game_window;

    // Create the D3DDevice (make sure we take a T&L device if it's available)
    if (d3dcaps9.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
    {
        hr = d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, game_window,
                                      D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
                                      &d3dpp, &d3d_dev);
    }
    else
    {
        hr = d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, game_window,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
                                      &d3dpp, &d3d_dev);
    }
    if (hr != D3D_OK)
        return 0;

    // Get the capabilities of the device
    memset(&caps, 0, sizeof caps);
    hr = d3d_dev->GetDeviceCaps(&caps);

    //Set texture address mode to clamp
    hr = d3d_dev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
    hr = d3d_dev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

    // Set filtering
    // Give us the best filtering available
    hr = d3d_dev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    hr = d3d_dev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
    if (caps.TextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR)
        hr = d3d_dev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    if (caps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR)
        hr = d3d_dev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    if (caps.TextureFilterCaps & D3DPTFILTERCAPS_MINFANISOTROPIC)
        hr = d3d_dev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
    if (caps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFANISOTROPIC)
        hr = d3d_dev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);

    // set maximum ambient light
    hr = d3d_dev->SetRenderState(D3DRS_AMBIENT, RGB(255,255,255));

    // Turn on backface culling
    hr = d3d_dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

    // Turn off D3D lighting
    hr = d3d_dev->SetRenderState(D3DRS_LIGHTING, FALSE);

    if (zbuffered)
    {
        // Turn on the zbuffer
        hr = d3d_dev->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
        hr = d3d_dev->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
        zbuffer_enabled = TRUE;
    }
    else
    {
        // Turn off the zbuffer
        hr = d3d_dev->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
        hr = d3d_dev->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
        zbuffer_enabled = FALSE;
    }

    // Turn off alpga blending
    hr = d3d_dev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

    // Here let's check if we can use dynamic textures
    textures_dynamic = FALSE;
    if (caps.Caps2 & D3DCAPS2_DYNAMICTEXTURES)
        textures_dynamic = TRUE;

    if (init_cube() == 0)
        return 0;

    init_textures();

    return 1;
}

//---------------------------------------------------------------------------------------
// Create a vertexbuffer and put some cube data in it
//---------------------------------------------------------------------------------------
#define V_LEFT   -0.5f
#define V_RIGHT   0.5f
#define V_TOP     0.5f
#define V_BOTTOM -0.5f
#define V_FRONT  -0.5f
#define V_BACK    0.5f

typedef struct
{
    float x,y,z;          // The position
    D3DCOLOR    color;    // The color
    FLOAT       tu, tv;   // The texture coordinates
} STOREVERTEX;

static const STOREVERTEX cube[24]=
{
    {V_LEFT,  V_TOP,    V_FRONT,    0xffffffff,0.0f,0.0f},
    {V_LEFT,  V_BOTTOM, V_FRONT,    0xffffffff,0.0f,1.0f},
    {V_RIGHT, V_BOTTOM, V_FRONT,    0xffffffff,1.0f,1.0f},
    {V_RIGHT, V_TOP,    V_FRONT,    0xffffffff,1.0f,0.0f},

    {V_LEFT,  V_TOP,    V_BACK,     0xffffffff,0.0f,0.0f},
    {V_RIGHT, V_TOP,    V_BACK,     0xffffffff,0.0f,1.0f},
    {V_RIGHT, V_BOTTOM, V_BACK,     0xffffffff,1.0f,1.0f},
    {V_LEFT,  V_BOTTOM, V_BACK,     0xffffffff,1.0f,0.0f},

    {V_LEFT,  V_TOP,    V_BACK,     0xffffffff,0.0f,0.0f},
    {V_LEFT,  V_BOTTOM, V_BACK,     0xffffffff,0.0f,1.0f},
    {V_LEFT,  V_BOTTOM, V_FRONT,    0xffffffff,1.0f,1.0f},
    {V_LEFT,  V_TOP,    V_FRONT,    0xffffffff,1.0f,0.0f},

    {V_RIGHT, V_TOP,    V_BACK,     0xffffffff,0.0f,0.0f},
    {V_RIGHT, V_TOP,    V_FRONT,    0xffffffff,0.0f,1.0f},
    {V_RIGHT, V_BOTTOM, V_FRONT,    0xffffffff,1.0f,1.0f},
    {V_RIGHT, V_BOTTOM, V_BACK,     0xffffffff,1.0f,0.0f},

    {V_LEFT,  V_TOP,    V_BACK,     0xffffffff,0.0f,0.0f},
    {V_LEFT,  V_TOP,    V_FRONT,    0xffffffff,0.0f,1.0f},
    {V_RIGHT, V_TOP,    V_FRONT,    0xffffffff,1.0f,1.0f},
    {V_RIGHT, V_TOP,    V_BACK,     0xffffffff,1.0f,0.0f},

    {V_LEFT,  V_BOTTOM, V_BACK,     0xffffffff,0.0f,0.0f},
    {V_RIGHT, V_BOTTOM, V_BACK,     0xffffffff,0.0f,1.0f},
    {V_RIGHT, V_BOTTOM, V_FRONT,    0xffffffff,1.0f,1.0f},
    {V_LEFT,  V_BOTTOM, V_FRONT,    0xffffffff,1.0f,0.0f},
};

int init_cube(void)
{
    CUSTOMVERTEX *pVertices;
    HRESULT hr;
    int i;

    hr = d3d_dev->CreateVertexBuffer(24 * sizeof CUSTOMVERTEX,
                                          D3DUSAGE_DYNAMIC, D3DFVF_CUSTOMVERTEX,
                                          D3DPOOL_DEFAULT, &vertex_buffer, NULL);

    if (hr != D3D_OK)
        return 0;

    // Fill the vertex buffer
    hr = vertex_buffer->Lock(0, 0, (void**)&pVertices, D3DLOCK_DISCARD);

    if (hr != D3D_OK)
        return 0;

    for (i = 0; i < 24; i++)
    {
        pVertices[i].position.x = cube[i].x;
        pVertices[i].position.y = cube[i].y;
        pVertices[i].position.z = cube[i].z;
        pVertices[i].color = cube[i].color;
        pVertices[i].tu = cube[i].tu;
        pVertices[i].tv = cube[i].tv;
    }
    hr = vertex_buffer->Unlock();

    return 1;
}

//---------------------------------------------------------------------------------------
// Create some basic textures
//---------------------------------------------------------------------------------------
#define TEX_WIDTH 16
#define TEX_HEIGHT 16

static const char pattern[2*TEX_WIDTH*TEX_HEIGHT]=
{
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,
    0,0,0,0,1,1,1,0,1,0,1,1,1,0,0,0,
    0,0,0,0,0,1,0,0,1,0,0,1,0,0,0,0,
    0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,
    0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,1,1,0,0,0,0,0,0,0,0,1,1,0,0,
    0,0,1,1,0,0,0,0,0,0,0,0,1,1,0,0,
    0,0,0,1,1,0,0,0,0,0,0,0,1,0,0,0,
    0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,
    0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,
    0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,
    0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,
    0,0,1,0,0,0,1,0,0,1,0,0,0,1,0,0,
    0,0,1,0,0,0,1,0,0,1,0,0,0,1,0,0,
    0,0,1,0,0,0,1,0,0,1,0,0,0,1,0,0,
    0,0,1,0,0,0,1,0,0,1,0,0,0,1,0,0,
    0,0,1,0,0,0,1,0,0,1,0,0,0,1,0,0,
    0,0,0,1,1,1,0,0,0,1,0,0,0,1,0,0,
    0,0,0,0,0,0,0,0,0,1,0,1,0,1,0,0,
    0,0,0,0,0,0,0,0,0,1,0,1,0,1,0,0,
    0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

};

void init_textures(void)
{
    HRESULT hr;
    int i;
    unsigned int colours[6] =
    {
        0xffff0000,
        0xff00ff00,
        0xff0000ff,
        0xffffff00,
        0xff00ffff,
        0xffff00ff,
    };

    D3DSURFACE_DESC ddsd;
    D3DLOCKED_RECT lr;
    IDirect3DSurface9 *pSurf;

    unsigned char *b;
    unsigned short wcolour;
    unsigned int x,y, width;
    unsigned int tex;

    for (i = 0; i < 6; i++)
    {
        if (textures_dynamic)
        {
            hr = d3d_dev->CreateTexture(TEX_WIDTH, TEX_HEIGHT, 1, D3DUSAGE_DYNAMIC,
                                         texture_format,D3DPOOL_DEFAULT,
                                         &texture[i], NULL);
        }
        else
        {
            hr = d3d_dev->CreateTexture(TEX_WIDTH, TEX_HEIGHT, 1, 0,
                                         texture_format,D3DPOOL_MANAGED,
                                         &texture[i], NULL);

        }

        // Texturing unsupported
        if (hr != D3D_OK)
            return;

        // CreateTexture can silently change the parameters on us

        memset(&ddsd, 0, sizeof ddsd);
        hr = texture[i]->GetLevelDesc(0, &ddsd);


        hr = texture[i]->GetSurfaceLevel(0, &pSurf);
        hr = pSurf->GetDesc(&ddsd);
        hr = pSurf->Release();

        if (textures_dynamic)
            hr = texture[i]->LockRect(0, &lr, NULL, D3DLOCK_DISCARD);
        else
            hr = texture[i]->LockRect(0, &lr, NULL, 0);

        // Writing to textures unsupported (broken driver?)
        if (hr != D3D_OK)
            return;

        b = (unsigned char *)lr.pBits;

        tex = (i&1)*TEX_WIDTH*TEX_HEIGHT;

        switch (ddsd.Format)
        {
            case D3DFMT_X8R8G8B8:
            case D3DFMT_A8R8G8B8:
                width = TEX_WIDTH * 4;

                for (y = 0; y < TEX_HEIGHT; y++)
                {
                    for (x = 0; x < TEX_WIDTH; x++)
                    {
                        *(((unsigned int *)b)+x) = pattern[tex+x+y*TEX_WIDTH]?colours[i]^0xffffff:colours[i];
                    }
                    b += lr.Pitch;
                }
                break;

            case D3DFMT_A1R5G5B5:
                width = TEX_WIDTH * 2;

                wcolour  = (colours[i]>>3)&0x1f;
                wcolour |= (colours[i]>>6)&(0x1f<<5);
                wcolour |= (colours[i]>>9)&(0x1f<<10);
                wcolour |= (colours[i]>>16)&0x8000;

                for (y = 0; y < TEX_HEIGHT; y++)
                {
                    for (x = 0; x < TEX_WIDTH; x++)
                    {
                        *(((unsigned short *)b)+x) = pattern[tex+x+y*TEX_WIDTH]?wcolour^0x7fff:wcolour;
                    }
                    b += lr.Pitch;
                }
                break;

            case D3DFMT_R5G6B5:
                width = TEX_WIDTH * 2;

                wcolour  = (colours[i]>>3)&0x1f;
                wcolour |= (colours[i]>>5)&(0x3f<<5);
                wcolour |= (colours[i]>>11)&(0x1f<<11);

                for (y = 0; y < TEX_HEIGHT; y++)
                {
                    for (x = 0; x < TEX_WIDTH; x++)
                    {
                        *(((unsigned short *)b)+x) = pattern[tex+x+y*TEX_WIDTH]?wcolour^0xffff:wcolour;
                    }
                    b += lr.Pitch;
                }
                break;

            default:
                assert(0);
        }

        hr = texture[i]->UnlockRect(0);
    }
}

//---------------------------------------------------------------------------------------
// Shutdown Direct3D
//---------------------------------------------------------------------------------------
void shutdown_d3d(void)
{
    int i;

    for (i = 0; i < 6; i++)
        if (texture[i]) texture[i]->Release();


    if (vertex_buffer) vertex_buffer->Release();
    if (d3d_dev) d3d_dev->Release();
    if (d3d) d3d->Release();
}

//---------------------------------------------------------------------------------------
// Set the Viewport
//---------------------------------------------------------------------------------------
void set_view(void)
{
    HRESULT hr;

    D3DXMATRIX matWorld;
    D3DXMATRIX matView;
    D3DXMATRIX matProj;

    float A,B,C,D,E,F,AD,BD;
    float zn, zf;

    memset(&matWorld, 0, sizeof matWorld);
    matWorld._11 = 1.0f;
    matWorld._22 = 1.0f;
    matWorld._33 = 1.0f;
    matWorld._44 = 1.0f;
    D3DXMatrixIdentity(&matWorld);
    hr = d3d_dev->SetTransform(D3DTS_WORLD, &matWorld);


    A=(float)cos((double)yaw);  B=(float)sin((double)yaw);
    C=(float)cos((double)pitch);D=(float)sin((double)pitch);
    E=(float)cos((double)roll); F=(float)sin((double)roll);
    AD=A*D;
    BD=B*D;

    memset(&matView, 0, sizeof matView);
    matView._11 = C*E;
    matView._12 = -C*F;
    matView._13 = D;
    matView._21 = BD*E+A*F;
    matView._22 = -BD*F+A*E;
    matView._23 = -B*C ;
    matView._31 = -AD*E+B*F;
    matView._32 = AD*F+B*E;
    matView._33 = A*C;
    matView._41 = x;
    matView._42 = y;
    matView._43 = z;
    matView._44 = 1.0f;

    matView._11 *= scale;
    matView._12 *= scale;
    matView._13 *= scale;
    matView._21 *= scale;
    matView._22 *= scale;
    matView._23 *= scale;
    matView._31 *= scale;
    matView._32 *= scale;
    matView._33 *= scale;

    hr = d3d_dev->SetTransform(D3DTS_VIEW, &matView);

/*
    D3DXMatrixPerspectiveLH(&matProj, 1.0f, 0.75f, 1.0f, 32767.0f);
    2*zn/w  0       0              0
    0       2*zn/h  0              0
    0       0       zf/(zf-zn)     1
    0       0       zn*zf/(zn-zf)  0
*/
    zn = 1.0f;
    zf = 32767.0f;
    memset(&matProj, 0, sizeof matProj);
    matProj._11 = 2.0f / 1.0f;
    matProj._22 = 2.0f / 0.75f;
    matProj._33 = 1.0f/(zf-zn);
    matProj._34 = 1.0f;
    matProj._43 = zn/(zn-zf);
/*
    D3DXMatrixOrthoLH(&matProj, 1.0f, 0.75f, 1.0f, 32767.0f);
    2/w  0    0           0
    0    2/h  0           0
    0    0    1/(zf-zn)   0
    0    0    zn/(zn-zf)  1

    zn = 1.0f;
    zf = 32767.0f;
    memset(&matProj, 0, sizeof matProj);
    matProj._11 = 2.0f / 1.0f;
    matProj._22 = 2.0f / 0.75f;
    matProj._33 = 1.0f/(zf-zn);
    matProj._34 = 1.0f;
    matProj._43 = zn/(zn-zf);
*/
    hr = d3d_dev->SetTransform(D3DTS_PROJECTION, &matProj);
}

//---------------------------------------------------------------------------------------
// Flip between the frontbuffer and backbuffer
//---------------------------------------------------------------------------------------
void flip(void)
{
    HRESULT hr;
    hr = d3d_dev->Present(0,0,0,0);
}

//---------------------------------------------------------------------------------------
// Draw the d3d scene
//---------------------------------------------------------------------------------------
void draw(void)
{
    HRESULT hr;
    int i;

    // Clear the scene
    if (zbuffer_enabled)
        hr = d3d_dev->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0xff000000, 1.0f, 0);
    else
        hr = d3d_dev->Clear(0, NULL, D3DCLEAR_TARGET, 0xff000000, 1.0f, 0);

    // Begin scene
    hr = d3d_dev->BeginScene();

    set_view();

    // Set the vertext format
    hr = d3d_dev->SetStreamSource(0, vertex_buffer, 0, sizeof(CUSTOMVERTEX));
    hr = d3d_dev->SetVertexShader(NULL);
    hr = d3d_dev->SetFVF(D3DFVF_CUSTOMVERTEX);

    // Set texture states
    hr = d3d_dev->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
    hr = d3d_dev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    hr = d3d_dev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    hr = d3d_dev->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

    // Render the vertex buffer contents
    for (i = 0; i < 6; i++)
    {
        // Set the texture
        hr = d3d_dev->SetTexture(0, texture[i]);
        // Draw one of the faces
        hr = d3d_dev->DrawPrimitive(D3DPT_TRIANGLEFAN, 4*i, 2);
    }

    // End the scene
    hr = d3d_dev->EndScene();
}
