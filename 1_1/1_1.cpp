﻿/*
Написать программу, строящую прямоугольник в центре окна. При нажатии
на левую кнопку мыши его размеры уменьшаются, а при нажатии на правую
кнопку - увеличиваются на 10 пикселов.
*/

#include "framework.h"
#include "1_1.h"

#define MAX_LOADSTRING 100

/**
 * Тип перечисления с событиями для мыши:
 * no - нет события;
 * left - нажата левая клавиша мыши;
 * right - нажата правая клавиша мыши.
*/
enum Mouse_Event { no, left, right };

// Структура хранит информацию о размерах прямоугольника
struct Size
{
	int size_x; // горизонтальный размер
	int size_y; // вертикальный размер
};

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


/**
 * Функция регистрирует класс окна.
 * @param hInstance - дескриптор приложения.
 * @return atom окна в случае успешной регистрации класса окна. Иначе
 * возвращается 0.
 */
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


/**
 * Функция сохраняет маркер экземпляра и создает главное окно.
 * @param hInstance - дескриптор приложения;
 * @param nCmdShow - режим отображения окна.
 * @return TRUE, если главное окно приложения создано. Иначе FALSE.
 */
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // сохранить маркер экземпляра в глобальной переменной

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


/**
 * Функция рисует прямоугольник по центру клиентской области окна.
 * @param hdc - дескриптор контекста устройства;
 * @param wnd_size - структура с размерами клиентской области окна;
 * @param rect_size - структура с размерами прямоугольника;
 * @param event - событие, случившееся с мышкой;
 * @param hPens - массив с перьями для рисования прямоугольника.
 */
void display_rect(HDC hdc, Size wnd_size, Size rect_size, Mouse_Event event, HPEN hPens[])
{
	// Выбираем перо, с которым будет нарисован прямоугольник
	SelectObject(hdc, hPens[event]);
	// Рисуем прямоугольник
	int x_left_up = (wnd_size.size_x - rect_size.size_x) / 2;
	int x_right_down = (wnd_size.size_x + rect_size.size_x) / 2;
	int y_left_up = (wnd_size.size_y - rect_size.size_y) / 2;
	int y_right_down = (wnd_size.size_y + rect_size.size_y) / 2;
	Rectangle(hdc, x_left_up, y_left_up, x_right_down, y_right_down);
}


/**
 * Функция обрабатывает сообщения в главном окне.
 * @param hWnd - дексриптор главного окна;
 * @param message - номер сообщения;
 * @wParam - параметр содержит дополнительные сведения о сообщении;
 * @lParam - параметр содержит дополнительные сведения о сообщении.
 * @return .
 */
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc; // дескриптор контекста устройства
	int x, y;
	static bool created_now; // параметр показыает, создано ли окно только что
	static HPEN hPens[3]; // массив из перьев для рисования прямоугольника на экране
	static Mouse_Event mouse_event; // параметр хранит событие с мышкой
	static Size wnd_size; // структура с размерами клиентской области окна
	static Size rect_size; // структура с размерами прямоугольника

	switch (message)
	{
	case WM_CREATE:
	{
		// Когда окно создается
		// Создаются перья для рисования прямоугольника в разных ситуациях
		hPens[no] = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
		hPens[left] = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
		hPens[right] = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
		created_now = true; // окно создано только что
		break;
	}
	case WM_SIZE:
	{
		// Когда изменяются размеры окна
		wnd_size.size_x = LOWORD(lParam);
		wnd_size.size_y = HIWORD(lParam);
		if (created_now)
		{
			// Если окно создано только что, задаются размеры для прямоугольника
			created_now = false;
			rect_size.size_x = wnd_size.size_x / 2;
			rect_size.size_y = wnd_size.size_y / 2;
		}
		break;
	}
	case WM_LBUTTONDOWN:
	{
		// Когда нажимается левая кнопка мыши
		// Нужно уменьшить размеры прямоугольника
		rect_size.size_x -= D;
		rect_size.size_y -= D;
		mouse_event = left;
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		// Когда нажимается правая кнопка мыши
		// Нужно увеличить размеры прямоугольника
		rect_size.size_x += D;
		rect_size.size_y += D;
		mouse_event = right;
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	}
	case WM_COMMAND:
	{
		// Когда кликаются пункты меню
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
		// Когда нужно перерисовать окно
		PAINTSTRUCT ps; // структура со всей информацией, необходимой для перерисовки окна
		hdc = BeginPaint(hWnd, &ps);
		display_rect(hdc, wnd_size, rect_size, mouse_event, hPens);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
	{
		// Когда нужно завершить программу
		// Удаляем созданные перья
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