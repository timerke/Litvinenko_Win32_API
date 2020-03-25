/*
Написать программу, строящую прямоугольник в центре окна. При нажатии
на левую кнопку мыши его размеры уменьшаются, а при нажатии на правую
кнопку - увеличиваются на 10 пикселов.
*/

#include "framework.h"
#include "1_1.h"

#define MAX_LOADSTRING 100

enum Mouse { No, Left, Right };

// Глобальные переменные
HINSTANCE hInst; // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING]; // текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING]; // имя класса главного окна
const int D = 10; // насколько увеличивать или уменьшать размеры прямоугольника

// Объявления функций, включенных в этот модуль кода
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// Инициализация глобальных строк
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_MY11, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Выполнить инициализацию приложения
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY11));

	MSG msg;

	// Цикл основного сообщения
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}


//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY11));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MY11);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}


//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}


// Функция для рисования прямоугольника
void display_rect(HDC hdc, int sx, int sy, int rx, int ry, Mouse event, HPEN hPens[])
{
	if (event == No)
	{
		SelectObject(hdc, hPens[0]);
	}
	else if (event == Left)
	{
		SelectObject(hdc, hPens[1]);
	}
	else if (event == Right)
	{
		SelectObject(hdc, hPens[2]);
	}
	Rectangle(hdc, (sx - rx) / 2, (sy - ry) / 2, (sx + rx) / 2, (sy + ry) / 2);
}


//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	static bool is_created_now; // параметр показыает, создано ли окно только что
	int x, y;
	static int size_x, size_y; // горизонтальный и вертикальный размеры клиентской области окна
	static int rect_x, rect_y; // горизонтальный и вертикальный размеры прямоугольника
	static HPEN hPens[3];
	static Mouse mouse_event; // параметр хранит событие с мышкой
	switch (message)
	{
	case WM_CREATE:
	{
		// Сообщение при создании окна
		hPens[0] = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
		hPens[1] = CreatePen(PS_SOLID, 2, RGB(0, 0, 128));
		hPens[2] = CreatePen(PS_SOLID, 2, RGB(128, 0, 0));
		is_created_now = true; // окно создано только что
		break;
	}
	case WM_SIZE:
	{
		// Сообщение при изменении размера окна
		size_x = LOWORD(lParam);
		size_y = HIWORD(lParam);
		if (is_created_now)
		{
			is_created_now = false;
			// Исходные размеры прямоугольника
			rect_x = size_x / 2;
			rect_y = size_y / 2;
		}
		break;
	}
	case WM_LBUTTONDOWN:
	{
		// Сообщение при нажатии на левую кнопку мыши
		// Уменьшить размеры прямоугольника
		rect_x -= D;
		rect_y -= D;
		mouse_event = Left;
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		// Сообщение при нажатии на правую кнопку мыши
		// Уменьшить размеры прямоугольника
		rect_x += D;
		rect_y += D;
		mouse_event = Right;
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	}
	case WM_COMMAND:
	{
		// Сообщения при работе с меню
		int wmId = LOWORD(wParam);
		// Разобрать выбор в меню
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		hdc = BeginPaint(hWnd, &ps);
		display_rect(hdc, size_x, size_y, rect_x, rect_y, mouse_event, hPens);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
	{
		for (auto h : hPens)
		{
			DeleteObject(h);
		}
		PostQuitMessage(0);
		break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}