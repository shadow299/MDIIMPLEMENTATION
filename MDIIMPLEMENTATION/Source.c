#include <Windows.h>
#include "resource.h"

#define INIT_MENU_POS 0
#define HELLO_MENU_POS 1
#define RECT_MENU_POS 2

#define ID_FIRSTCHILD  50000


LRESULT CALLBACK FrameWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK HelloWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK RectWndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK CloseEnumProc(HWND, LPARAM);

//structure for storing data unique to each hello child window
typedef struct tagHELLODATA {
	UINT iColor;
	COLORREF clrText;
} HELLODATA, *PHELLODATA;


//structure for storing data unique to each rect child window
typedef struct tagRECTDATA {
	short cxClient;
	short cyClient;
} RECTDATA, *PRECTDATA;


//global variables
TCHAR szAppName[] = TEXT("MDI Demo");
TCHAR szFrmeClass[] = TEXT("MdiFrame");
TCHAR szHelloClass[] = TEXT("MdiHelloChild");
TCHAR szRectClass[] = TEXT("MdiREctClass");
HINSTANCE hInst;
HMENU hMenuInit, hMenuHello, hMenuRect;
HMENU hMenuInitWindow, hMenuHelloWindow, hMenuRectWindow;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrivInstance, PSTR szCmdLine, int iCmdShow) {
	HWND hwndFrame, hwndClient;
	MSG msg;
	WNDCLASS wndclass;
	hInst = hInstance;
	HACCEL hAccel;

	//register the frame window class

	wndclass.style = CS_VREDRAW | CS_HREDRAW;
	wndclass.lpfnWndProc = FrameWndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIconW(NULL, IDI_WINLOGO);
	wndclass.hCursor = LoadCursorW(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH) (COLOR_APPWORKSPACE + 1);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szFrmeClass;

	//register class
	if (!RegisterClass(&wndclass)) {
		MessageBox(NULL, TEXT("This program requires windows!!"), szAppName, MB_ICONERROR);
		return 0;
	}

	//register hello child window class
	wndclass.style = CS_VREDRAW | CS_HREDRAW;
	wndclass.lpfnWndProc = HelloWndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = sizeof(HANDLE);
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIconW(NULL, IDI_WINLOGO);
	wndclass.hCursor = LoadCursorW(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)(GetStockObject(WHITE_BRUSH));
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szHelloClass;
	RegisterClass(&wndclass);

	//register the rect child window class
	wndclass.style = CS_VREDRAW | CS_HREDRAW;
	wndclass.lpfnWndProc = RectWndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = sizeof(HANDLE);
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIconW(NULL, IDI_WINLOGO);
	wndclass.hCursor = LoadCursorW(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szRectClass;
	RegisterClass(&wndclass);

	//obtain the handle of three possible menu and submenus
	hMenuInit = LoadMenu(hInstance, MAKEINTRESOURCE(MDI_MENUINIT));
	hMenuHello = LoadMenu(hInstance, MAKEINTRESOURCE(MDI_MENUHELLO));
	hMenuRect = LoadMenu(hInstance, MAKEINTRESOURCE(MDI_MENURECT));

	//submenus
	hMenuInitWindow = GetSubMenu(hMenuInit, INIT_MENU_POS);
	hMenuHelloWindow = GetSubMenu(hMenuHello, HELLO_MENU_POS);
	hMenuRectWindow = GetSubMenu(hMenuRect, RECT_MENU_POS);

	//load Accelerator table
	hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));
	


	hwndFrame = CreateWindow(
		szFrmeClass,
		TEXT("MDI DEMO"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		hMenuInit,
		hInstance,
		NULL);

	hwndClient = GetWindow(hwndFrame, GW_CHILD);

	ShowWindow(hwndFrame, iCmdShow);
	UpdateWindow(hwndFrame);

	//slightly modified message loop for MDI support
	while (GetMessage(&msg, NULL, 0, 0)) {
		if (!TranslateMDISysAccel(hwndClient, &msg) && !TranslateAccelerator(hwndClient, hAccel, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	DestroyMenu(hMenuHello);
	DestroyMenu(hMenuRect);
	return msg.wParam;
}

