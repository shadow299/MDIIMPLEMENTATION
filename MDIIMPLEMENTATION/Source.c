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

LRESULT CALLBACK FrameWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static HWND hwndClient;
	CLIENTCREATESTRUCT ClientCreate;
	HWND hwndChild;
	MDICREATESTRUCT mdiCreate;

	switch (message) {
	case WM_CREATE:
		//create the client window
		ClientCreate.hWindowMenu = hMenuInitWindow;
		ClientCreate.idFirstChild = ID_FIRSTCHILD;

		hwndClient = CreateWindow(TEXT("MDICLIENT"), NULL, WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE, 0, 0, 0, 0, hwnd, (HMENU)1, hInst, (PSTR)&ClientCreate);

		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_FILE_NEWHELLO:
			//create hello child window
			mdiCreate.szClass = szHelloClass;
			mdiCreate.szTitle = TEXT("HELLO");
			mdiCreate.hOwner = hInst;
			mdiCreate.x = CW_USEDEFAULT;
			mdiCreate.y = CW_USEDEFAULT;
			mdiCreate.cy = CW_USEDEFAULT;
			mdiCreate.cx = CW_USEDEFAULT;
			mdiCreate.style = 0;
			mdiCreate.lParam = 0;

			hwndChild = (HWND)SendMessage(hwndClient, WM_MDICREATE, 0, (LPARAM)(LPMDICREATESTRUCT)&mdiCreate);
			return 0;

		case ID_FILE_NEWRECTANGLE:
			//create a child rect
			mdiCreate.szClass = szRectClass;
			mdiCreate.szTitle = TEXT("RECT");
			mdiCreate.hOwner = hInst;
			mdiCreate.x = CW_USEDEFAULT;
			mdiCreate.y = CW_USEDEFAULT;
			mdiCreate.cy = CW_USEDEFAULT;
			mdiCreate.cx = CW_USEDEFAULT;
			mdiCreate.style = 0;
			mdiCreate.lParam = 0;

			hwndChild = (HWND)SendMessage(hwndClient, WM_MDICREATE, 0, (LPARAM)(LPMDICREATESTRUCT)&mdiCreate);
			return 0;

		case ID_FILE_CLOSE:
			//close the active window
			hwndChild =(HWND) SendMessage(hwndClient, WM_MDIGETACTIVE, 0, 0);

			if (SendMessage(hwndChild, WM_QUERYENDSESSION, 0, 0))
				SendMessage(hwndClient, WM_MDIDESTROY, (HWND)hwndChild, 0);
			return 0;

		case ID_FILE_EXIT:
			//exit the program
			SendMessage(hwnd, WM_CLOSE, 0, 0);
			return 0;

		case ID_WINDOW_TILE:
			SendMessage(hwndClient, WM_MDITILE, 0, 0);
			return 0;

		case ID_WINDOW_CASCADE:
			SendMessage(hwndClient, WM_MDICASCADE, 0, 0);
			return 0;

		case ID_WINDOW_ARRANGEICONS:
			SendMessage(hwndClient, WM_MDIICONARRANGE, 0, 0);
			return 0;

		case ID_WINDOW_CLOSEALL:
			//attempt to close all windows
			EnumChildWindows(hwndClient, CloseEnumProc, 0);
			return 0;

		default:
			//pass to active child
			hwndChild = (HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0, 0);
			if (IsWindow(hwndChild)) {
				SendMessage(hwndChild, WM_COMMAND, wParam, lParam);
			}
			break;

		}
		break;

	case WM_QUERYENDSESSION:
	case WM_CLOSE:
		SendMessage(hwnd, WM_COMMAND, ID_WINDOW_CLOSEALL, 0);
		if (NULL != GetWindow(hwndClient, GW_CHILD)) {
			return 0;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefFrameProc(hwnd, hwndClient, message, wParam, lParam);//DefWindowProc is not used
}

BOOL CALLBACK CloseEnumProc(HWND hwnd, LPARAM lParam) {
	if (GetWindow(hwnd, GW_OWNER)) {
		return TRUE;
	}
	SendMessage(GetParent(hwnd), WM_MDIRESTORE, (HWND)hwnd, 0);
	if (!SendMessage(hwnd, WM_QUERYENDSESSION, 0, 0)) {
		return TRUE;
	}
	SendMessage(GetParent(hwnd), WM_MDIDESTROY, (HWND)hwnd, 0);
	return TRUE;
}

LRESULT CALLBACK HelloWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static COLORREF clrTextArray[] = { RGB(0, 0, 0), RGB(255, 0, 0), RGB(0, 255, 0), RGB(0, 0, 255), RGB(255, 255, 255) };
	static HWND hwndClient, hwndFrame;
	HDC hdc;
	HMENU hMenu;
	PHELLODATA pHelloData;
	PAINTSTRUCT ps;
	RECT rect;

	switch (message) {
	case WM_CREATE:
		//allocate memory for window private data
		pHelloData = (PHELLODATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(HELLODATA));

		pHelloData->iColor = ID_COLOR_BLACK;
		pHelloData->clrText = RGB(0, 0, 0);
		SetWindowLong(hwnd, 0, (long)pHelloData);

		//save window handles;
		hwndClient = GetParent(hwnd);
		hwndFrame = GetParent(hwndClient);
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_COLOR_BLACK:
		case ID_COLOR_RED:
		case ID_COLOR_GREEN:
		case ID_COLOR_BLUE:
		case ID_COLOR_WHITE:
			//change text color
			pHelloData = (PHELLODATA) GetWindowLong(hwnd, 0);

			hMenu = GetMenu(hwndFrame);
			CheckMenuItem(hMenu, pHelloData->iColor, MF_UNCHECKED);
			pHelloData->iColor = wParam;
			CheckMenuItem(hMenu, pHelloData->iColor, MF_CHECKED);
			pHelloData->clrText = clrTextArray[wParam - ID_COLOR_BLACK];
			InvalidateRect(hwnd, NULL, FALSE);
			return 0;
		}
		break;

	case WM_PAINT:
		//paint the window
		hdc = BeginPaint(hwnd, &ps);
		pHelloData = (PHELLODATA) GetWindowLong(hwnd, 0);
		SetTextColor(hdc, pHelloData->clrText);

		GetClientRect(hwnd, &rect);
		DrawText(hdc, TEXT("Hello World"), -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

		EndPaint(hwnd, &ps);
		return 0;

	case WM_MDIACTIVATE:
		//set the hello menu if gaining focus
		if (lParam == (LPARAM)hwnd) {
			SendMessage(hwndClient, WM_MDISETMENU, (HMENU)hMenuHello, (LPARAM)hMenuHelloWindow);
		}
		pHelloData = (PHELLODATA) GetWindowLong(hwnd, 0);
		CheckMenuItem(hMenuHello, pHelloData->iColor, (lParam == (LPARAM)hwnd) ? MF_CHECKED : MF_UNCHECKED);

		//set init menu if loosing focus
		if (lParam != (LPARAM)hwnd) {
			SendMessage(hwndClient, WM_MDISETMENU, (WPARAM)hMenuInit, (LPARAM)hMenuInitWindow);
		}
		DrawMenuBar(hwndFrame);
		return 0;

	case WM_QUERYENDSESSION:
	case WM_CLOSE:
		if (IDOK != MessageBox(hwnd, TEXT("Ok to close window ?"), TEXT("Hello"), MB_ICONQUESTION | MB_OKCANCEL))
			return 0;
		break;

	case WM_DESTROY:
		pHelloData = (PHELLODATA)GetWindowLong(hwnd, 0);
		HeapFree(GetProcessHeap(), 0, pHelloData);
		return 0;

	}
	return DefMDIChildProc(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK RectWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static HWND hwndClient, hwndFrame;
	HBRUSH hBrush;
	HDC hdc;
	PRECTDATA pRectData;
	PAINTSTRUCT ps;
	int xLeft, xRight, yTop, yBottom;
	short nRed, nGreen, nBlue;

	switch (message) {
	case WM_CREATE:
		//Allocate memory for window private data
		pRectData = (PRECTDATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(RECTDATA));
		SetWindowLong(hwnd, 0, (long)pRectData);

		//start the timer 
		SetTimer(hwnd, 1, 250, NULL);

		//save window handles
		hwndClient = GetParent(hwnd);
		hwndFrame = GetParent(hwndClient);
		return 0;

	case WM_SIZE:
		if (wParam != SIZE_MINIMIZED) {
			pRectData = (PRECTDATA)GetWindowLong(hwnd, 0);
			pRectData->cxClient = LOWORD(lParam);
			pRectData->cyClient = HIWORD(lParam);
		}
		break;

	case WM_TIMER:
		pRectData = (PRECTDATA)GetWindowLong(hwnd, 0);
		xLeft = rand() % pRectData->cxClient;
		xRight = rand() % pRectData->cxClient;
		yTop = rand() % pRectData->cyClient;
		yBottom = rand() % pRectData->cyClient;
		nRed = rand() & 255;
		nGreen = rand() & 255;
		nBlue = rand() & 255;

		hdc = GetDC(hwnd);
		hBrush = CreateSolidBrush(RGB(nRed, nGreen, nBlue));
		SelectObject(hdc, hBrush);

		Rectangle(hdc, min(xLeft, xRight), min(yTop, yBottom), max(xLeft, xRight), max(yTop, yBottom));
		ReleaseDC(hwnd, hdc);
		DeleteObject(hBrush);
		return 0;

	case WM_PAINT:
		//clear the window
		InvalidateRect(hwnd, NULL, TRUE);
		hdc = BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
		return 0;

	case WM_MDIACTIVATE:
		if (lParam == (LPARAM)hwnd) {
			SendMessage(hwndClient, WM_MDISETMENU, (WPARAM)hMenuRect, (LPARAM)hMenuRectWindow);
		}
		else {
			SendMessage(hwndClient, WM_MDISETMENU, (WPARAM) hMenuInit, (LPARAM) hMenuInitWindow);
		}
		DrawMenuBar(hwnd);
		return 0;

	case WM_DESTROY:
		pRectData = (PRECTDATA)GetWindowLong(hwnd, 0);
		HeapFree(GetProcessHeap(), 0, pRectData);
		KillTimer(hwnd, 1);
		return 0;
	}
	return DefMDIChildProc(hwnd, message, wParam, lParam);
}

