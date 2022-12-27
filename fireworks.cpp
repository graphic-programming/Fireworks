// 
//     This is program to simulate fireworks. I use two threads.
//     The first thread is to add sprite (velocity, angle, initial position)
//     in the list. The second thread is to compute the trajectory
//     of every sprite in the list. Sprites that are no longer visible
//     on screen are removed from the list.
//
//     To start the animation select Animate -> Play or press Enter
//     To stop  the animation select Animate -> Stop or press Space
//     To speed up revolution press any number fron 0 to 6

#include "stdafx.h"
#include "fireworks.h"
#include "tgafile.h"
#include "linkedlist.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// thread
HANDLE hThread1 = NULL;
HANDLE hThread2 = NULL;
CLinkedList que;

//
CTgaFile file;
unsigned int texture;
double left, right, bottom, top;
float ax, ay;
float dpms;

// Distance attenuation = 1 / a + b * d + c * d^2
float attenuation[3] =  { 0.001f, 0.0f, 0.0f };

PFNGLPOINTPARAMETERFARBPROC glPointParameterfARB = NULL;
PFNGLPOINTPARAMETERFVARBPROC glPointParameterfvARB = NULL;

// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
DWORD WINAPI     ThreadFunc1(LPVOID lpParam);
DWORD WINAPI     ThreadFunc2(LPVOID lpParam);

void Rotate(float *x, float *y, float a1, float b1, float deg);

void OnAddSprite(LPARAM lParam);
void OnAnimateProjectile(LPARAM lParam);

void OnPaint(HDC hDC);
void OnCreate(HWND hWnd, HDC *hDC);
void OnDestroy(HWND hWnd, HDC hDC);
void OnSize(HWND hWnd, int cx, int cy);

void OnFireworksExit(HWND hWnd);

void OnAnimatePlay(HWND hWnd);
void OnAnimateStop(HWND hWnd);

void OnRevolution0(HWND hWnd);
void OnRevolution1(HWND hWnd);
void OnRevolution2(HWND hWnd);
void OnRevolution3(HWND hWnd);
void OnRevolution4(HWND hWnd);
void OnRevolution5(HWND hWnd);
void OnRevolution6(HWND hWnd);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_FIREWORKS, szWindowClass, MAX_LOADSTRING);

	WNDCLASSEXW wcex;

	wcex.cbSize        = sizeof(WNDCLASSEX);
	wcex.style         = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc   = WndProc;
	wcex.cbClsExtra    = 0;
	wcex.cbWndExtra    = 0;
	wcex.hInstance     = hInstance;
	wcex.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FIREWORKS));
	wcex.hCursor       = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName  = MAKEINTRESOURCEW(IDC_FIREWORKS);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm       = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	RegisterClassExW(&wcex);

    // save the instance handle in a global variable
	hInst = hInstance;

	// create the main program window
	int X, Y, nWidth, nHeight, Cx, Cy;
	
    // 480p: 854x480

    Cx = 854;
    Cy = 480;

    nWidth = Cx + 16;
    nHeight = Cy + 58;

    X = (GetSystemMetrics(SM_CXSCREEN) - nWidth) / 2;
    Y = (GetSystemMetrics(SM_CYSCREEN) - nHeight) / 4;

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		                      X, Y,
		                      nWidth, nHeight,
		                      nullptr, nullptr, hInstance, nullptr);

	if (!hWnd) return FALSE;

	// display the main program window
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FIREWORKS));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

//  Processes messages for the main window.
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HDC hDC;

    switch (message)
    {
	case WM_ADD_SPRITE:			OnAddSprite(lParam);			break;
	case WM_ANIMATE_PROJECTILE:	OnAnimateProjectile(lParam);	break;

    case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
			case IDM_FIREWORKS_EXIT:OnFireworksExit(hWnd);	break;

			case IDM_ANIMATE_PLAY:	OnAnimatePlay(hWnd);	break;
			case IDM_ANIMATE_STOP:	OnAnimateStop(hWnd);	break;

			case IDM_REVOLUTION_0:	OnRevolution0(hWnd);		break;
			case IDM_REVOLUTION_1:	OnRevolution1(hWnd);		break;
			case IDM_REVOLUTION_2:	OnRevolution2(hWnd);		break;
			case IDM_REVOLUTION_3:	OnRevolution3(hWnd);		break;
			case IDM_REVOLUTION_4:	OnRevolution4(hWnd);		break;
			case IDM_REVOLUTION_5:	OnRevolution5(hWnd);		break;
			case IDM_REVOLUTION_6:	OnRevolution6(hWnd);		break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
	case WM_PAINT:   OnPaint(hDC);									break;
	case WM_CREATE:  OnCreate(hWnd, &hDC);							break;
	case WM_DESTROY: OnDestroy(hWnd, hDC);							break;
	case WM_SIZE:    OnSize(hWnd, LOWORD (lParam), HIWORD (lParam));break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// execute function OnAddSprite every 1 millisecond
DWORD WINAPI ThreadFunc1(LPVOID lpParam)
{
	HWND* phWnd = (HWND*)lpParam;
	HWND hWnd = *phWnd;

    while(TRUE)
    {
		Sleep(1);
        SendMessage(hWnd, WM_ADD_SPRITE, 0, 0);
    }

	return 0;
}

// execute function OnAnimateProjectile every 40 millisecond
DWORD WINAPI ThreadFunc2(LPVOID lpParam)
{
	HWND* phWnd = (HWND*)lpParam;
	HWND hWnd = *phWnd;
	DWORD tm = 40;

    while(TRUE)
    {
		Sleep(tm);
        SendMessage(hWnd, WM_ANIMATE_PROJECTILE, 0, (LPARAM)&tm);
    }

	return 0;
}

// rotate point (a1, b1) by deg degree around the origin
// new position at point (x, y)
void Rotate(float *x, float *y, float a1, float b1, float deg)
{
	float a2, b2;
	float a, b, s, rad;

	s = (float)sqrt(a1*a1 + b1*b1);
	a1 /= s;
	b1 /= s;

	rad = (deg / 180.0f) * (float)M_PI;
	a2 = (float)cos(rad);
	b2 = (float)sin(rad);

	s = (float)sqrt(a2*a2 + b2*b2);
	a2 /= s;
	b2 /= s;

	a = a1*a2 - b1*b2;
	b = a2*b1 + a1*b2;

	s = (float)sqrt(a*a + b*b);
	a /= s;
	b /=s;

	*x = a;
	*y = b;
}

// add sprites in the list
void OnAddSprite(LPARAM lParam)
{
	float v, a, sx, sy, rx, ry, px, py;
	float s = 17.0f;
	int i, j;

	px = ax;
	py = ay;

	for (i = 0; i < 8; ++i)
	{
		for (j = 0; j < 3; ++j)
		{
			// initial position of a sprite
			sx = s * px;
			sy = s * py;

			// set random value for velocity and angle
			v = 98.0f + 30.0f * ((float)rand() / (float)RAND_MAX);
			a = 30.0f * ((float)rand() / (float)RAND_MAX);

			Rotate(&rx, &ry, px, py, -90.0f + a);
			que.Add(v*rx, v*ry, sx, sy, 0.0f, sx, sy);

			// position next sprite 120 degree apart
			Rotate(&rx, &ry, px, py, 120.0f);
			px = rx;
			py = ry;
		}
	}

	// make the sprite rotate
	Rotate(&rx, &ry, ax, ay, dpms);
	ax = rx;
	ay = ry;
}

// compute the trajectory of every sprite in the list
void OnAnimateProjectile(LPARAM lParam)
{
	DWORD* p = (DWORD*)lParam;
	DWORD d = *p;
	float vx, vy, sx, sy, x, y, t;
	float g = -9.8f;
	LINKEDLIST *Node;

	que.MoveFirst();
	while(!que.IsEndOfQueue())
	{
		// read sprite one by one
		Node = que.Read(&vx, &vy, &sx, &sy, &t);

		// compute new position of sprite
		// using projectile motion formula
		t += ((float)d / 1000.0f);
        x  = sx + vx * t;
        y  = sy + 2.0f*g*t*t + vy*t;

		// remove sprites that are no longer on screen
		if (x < left || x > right || y < bottom || y > top)
		{
			que.MoveNext();
			que.Remove(Node);
		}
		else
		{
			que.Update(x, y, t);
			que.MoveNext();
		}
	}
}

//
void OnPaint(HDC hDC)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_POINT_SPRITE_ARB);
	glDisable(GL_BLEND);

     // axis
     glBegin(GL_LINES);

		  // x - axis
          glColor3f(0.25f,0.0f,0.0f);
		  glVertex3d(left, 0.0, 0.0);
		  glVertex3d(right, 0.0, 0.0);

		   // y - axis
          glColor3f(0.0f,0.25f,0.0f);
		  glVertex3d(0.0, bottom, 0.0);
		  glVertex3d(0.0, top, 0.0);

     glEnd();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_POINT_SPRITE_ARB);
	glEnable(GL_BLEND);

	float x, y;

	// draw sprite
	glColor3f(1.0, 1.0, 0.0);
	glBegin(GL_POINTS);

	que.MoveFirst();
	while(!que.IsEndOfQueue())
	{
		que.Read(&x, &y);

		glVertex3f(x, y, 0.0f);

		que.MoveNext();
	}

	glEnd();

	SwapBuffers(hDC);
}

//
void OnCreate(HWND hWnd, HDC *hDC)
{
    int iPixelFormat;
    HGLRC hglRC;                // rendering context

    // create a pixel format
    static PIXELFORMATDESCRIPTOR pfd = {
         sizeof(PIXELFORMATDESCRIPTOR),   // size of structure.
         1,                               // version number
         PFD_DRAW_TO_WINDOW |             // support window
         PFD_SUPPORT_OPENGL |             // support OpenGl
         PFD_DOUBLEBUFFER,                // double buffered
         PFD_TYPE_RGBA,                   // RGBA type
         24,                              // 24-bit color depth
         0, 0, 0, 0, 0, 0,                // color bits ignored
         0,                               // no alpha buffer
         0,                               // shift bit ignored
         0,                               // no accumulation buffer
         0, 0, 0, 0,                      // accum bits ignored
         32,                              // 32-bit z-buffer
         0,                               // no stencil buffer
         0,                               // no auxiliary buffer
         PFD_MAIN_PLANE,                  // main layer
         0,                               // reserved
         0, 0, 0 };                       // layer masks ignored.

     *hDC = GetDC(hWnd);                                 // get the device context for our window
     iPixelFormat = ChoosePixelFormat(*hDC, &pfd);       // get the best available match of pixel format for the device context
     SetPixelFormat(*hDC, iPixelFormat, &pfd);           // make that the pixel format of the device context
     hglRC = wglCreateContext(*hDC);                     // create an OpenGL rendering context
     wglMakeCurrent(*hDC, hglRC);                        // make it the current rendering context
	
	// load sprite
	if (!file.Load(L"sprite.tga"))
	{
		 MessageBoxA(NULL, "Cannot open file: 'sprite.tga': No such file.", "Fireworks Error", MB_OK);
	     PostMessage(hWnd, WM_COMMAND, IDM_FIREWORKS_EXIT, 0);
		 return;
	}

	 // initialize opengl parameter
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_POINT_SPRITE_ARB);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	int type = file.byteperpixel == 3 ? GL_RGB : GL_RGBA;
	gluBuild2DMipmaps(GL_TEXTURE_2D, type, file.width, file.height, type, GL_UNSIGNED_BYTE, file.data);

   // With this we will load the list of extensions the hardware
   // this demo is running on supports.
   char *extension = (char*)glGetString(GL_EXTENSIONS);

   // Now that we have all of the extensions the hardware supports lets test if the
   // GL_ARB_point_parameters extension is there.
   if(strstr(extension, "GL_ARB_point_parameters") == NULL)
	{
		MessageBoxA(NULL, "extension bug", "Fireworks Error", MB_OK);
		PostMessage(hWnd, WM_COMMAND, IDM_FIREWORKS_EXIT, 0);
		return;
	}

   // get the functions
   glPointParameterfARB = (PFNGLPOINTPARAMETERFARBPROC)wglGetProcAddress("glPointParameterfARB");
   glPointParameterfvARB = (PFNGLPOINTPARAMETERFVARBPROC)wglGetProcAddress("glPointParameterfvARB");

   if(!glPointParameterfARB || !glPointParameterfvARB)
	{
		MessageBoxA(NULL, "extension bug", "Fireworks Error", MB_OK);
		PostMessage(hWnd, WM_COMMAND, IDM_FIREWORKS_EXIT, 0);
		return;
	}

	// The first specifies the threshold that once passed, will cause the
	// point sprites to fade, not shrink.  The second is the minimum value
	// of the sprites.  The third is the max value of the sprite.  The
	// fourth sets the distance attenuation.
	glPointParameterfARB(GL_POINT_FADE_THRESHOLD_SIZE_ARB, 80.0f);
	glPointParameterfARB(GL_POINT_SIZE_MIN_ARB, 0.1f);
	glPointParameterfARB(GL_POINT_SIZE_MAX_ARB, 20.0f);
	glPointParameterfvARB(GL_POINT_DISTANCE_ATTENUATION_ARB, attenuation);

	// Calculate texture coords for each sprite.
	glTexEnvf(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);

	// 
	HMENU hMenu = GetMenu(hWnd);
	EnableMenuItem(hMenu, IDM_ANIMATE_PLAY,   MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(hMenu, IDM_ANIMATE_STOP,   MF_BYCOMMAND | MF_DISABLED);

	// initial position of sprite
	float rad = (71.0f / 180.0f) * (float)M_PI;
	ax = (float)cos(rad);
	ay = (float)sin(rad);

	float s = (float)sqrt(ax*ax + ay*ay);
	ax /= s;
	ay /= s;

	// create a thread
	DWORD dwThreadId;
	static HWND hwnd = hWnd;
	hThread1 = CreateThread( NULL, 0, ThreadFunc1, &hwnd, CREATE_SUSPENDED, &dwThreadId);
	if (hThread1 == NULL) OutputDebugString(L"..... Create thread 1 failed.\n" );

	hThread2 = CreateThread( NULL, 0, ThreadFunc2, &hwnd, 0, &dwThreadId);
	if (hThread2 == NULL) OutputDebugString(L"..... Create thread 2 failed.\n" );

	// set revolution speed to 0
	SendMessage(hWnd, WM_COMMAND, IDM_REVOLUTION_0, 0);
}

//
void OnDestroy(HWND hWnd, HDC hDC)
{
	// close thread
	if(hThread1 != NULL)    CloseHandle(hThread1);
	if(hThread2 != NULL)    CloseHandle(hThread2);

	//
	glDeleteTextures(1, &texture);

	//
	que.RemoveAll();

	HGLRC hglRC;                // rendering context

	hglRC = wglGetCurrentContext(); // get current OpenGL rendering context
	wglMakeCurrent(hDC, NULL);      // make the rendering context not current
	wglDeleteContext(hglRC);        // delete the rendering context
	ReleaseDC(hWnd, hDC);           // releases a device context

	PostQuitMessage(0); // close the program.
}

//
void OnSize(HWND hWnd, int cx, int cy)
{
	double ox, oy, width, height;

	// ratio and proportion
	//  
	//  height     cy
	// -------- = ----
	//  width      cx
	//  

	width  = 250.0;
	height = width * ((double)cy/(double)cx);

	ox = width  / 2.0;
	oy = height / 2.0;

	left   = -ox;
	right  = left + width;
	bottom = -oy;
	top    = bottom + height;

	glViewport(0, 0, cx, cy);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(left, right, bottom, top, 0.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}

//
void OnFireworksExit(HWND hWnd)
{
	DestroyWindow(hWnd);
}

void OnAnimatePlay(HWND hWnd)
{
	HMENU hMenu = GetMenu(hWnd);
	EnableMenuItem(hMenu, IDM_ANIMATE_PLAY,   MF_BYCOMMAND | MF_DISABLED);
	EnableMenuItem(hMenu, IDM_ANIMATE_STOP,   MF_BYCOMMAND | MF_ENABLED);

	ResumeThread(hThread1);
}

//
void OnAnimateStop(HWND hWnd)
{
	HMENU hMenu = GetMenu(hWnd);
	EnableMenuItem(hMenu, IDM_ANIMATE_PLAY,   MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(hMenu, IDM_ANIMATE_STOP,   MF_BYCOMMAND | MF_DISABLED);

	SuspendThread(hThread1);
}

//
void OnRevolution0(HWND hWnd)
{
	HMENU hMenu = GetMenu(hWnd);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_0,   MF_BYCOMMAND | MF_CHECKED);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_1,   MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_2,   MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_3,   MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_4,   MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_5,   MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_6,   MF_BYCOMMAND | MF_UNCHECKED);

	dpms = 0.0f;
}

//
void OnRevolution1(HWND hWnd)
{
	HMENU hMenu = GetMenu(hWnd);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_0,   MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_1,   MF_BYCOMMAND | MF_CHECKED);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_2,   MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_3,   MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_4,   MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_5,   MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_6,   MF_BYCOMMAND | MF_UNCHECKED);

	dpms = 1.0f;
}

//
void OnRevolution2(HWND hWnd)
{
	HMENU hMenu = GetMenu(hWnd);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_0,   MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_1,   MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_2,   MF_BYCOMMAND | MF_CHECKED);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_3,   MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_4,   MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_5,   MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_6,   MF_BYCOMMAND | MF_UNCHECKED);

	dpms = 2.0f;
}

//
void OnRevolution3(HWND hWnd)
{
	HMENU hMenu = GetMenu(hWnd);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_0,   MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_1,   MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_2,   MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_3,   MF_BYCOMMAND | MF_CHECKED);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_4,   MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_5,   MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_6,   MF_BYCOMMAND | MF_UNCHECKED);

	dpms = 3.0f;
}

//
void OnRevolution4(HWND hWnd)
{
	HMENU hMenu = GetMenu(hWnd);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_0,   MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_1,   MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_2,   MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_3,   MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_4,   MF_BYCOMMAND | MF_CHECKED);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_5,   MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_6,   MF_BYCOMMAND | MF_UNCHECKED);

	dpms = 4.0f;
}

//
void OnRevolution5(HWND hWnd)
{
	HMENU hMenu = GetMenu(hWnd);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_0,   MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_1,   MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_2,   MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_3,   MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_4,   MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_5,   MF_BYCOMMAND | MF_CHECKED);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_6,   MF_BYCOMMAND | MF_UNCHECKED);

	dpms = 5.0f;
}

//
void OnRevolution6(HWND hWnd)
{
	HMENU hMenu = GetMenu(hWnd);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_0,   MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_1,   MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_2,   MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_3,   MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_4,   MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_5,   MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu,  IDM_REVOLUTION_6,   MF_BYCOMMAND | MF_CHECKED);

	dpms = 6.0f;
}

//
