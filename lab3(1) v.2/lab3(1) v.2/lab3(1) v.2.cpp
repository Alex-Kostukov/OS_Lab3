#include "framework.h"
#include "lab3(1) v.2.h"

#pragma optimize( "", off )

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

DWORD thread1, thread2, thread4; // потоки
CRITICAL_SECTION critsec; // объявление критической секции
LPVOID fiber, threadfiber; // нити
wchar_t str1[5], str2[5]; // вывод в static
HWND hStatic, hStatic2, hButton1, hButton2, hStatic3, hStatic4; // элементы главного окна

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

DWORD WINAPI NoCritSecFunc(PVOID pParam);
DWORD WINAPI CritSecFunc(PVOID pParam);
DWORD WINAPI SendMsgFunc(PVOID pParam);
VOID __stdcall FunctionFiber(LPVOID lpParameter);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB31V2, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB31V2));

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

    return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
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
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB31V2));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LAB31V2);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    HWND hWnd = CreateWindowW(szWindowClass, L"Lab3", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 320, 260, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case ID_BUTTON3:
        {

            HANDLE event1 = OpenEvent(EVENT_MODIFY_STATE, FALSE, L"event1"); //  открываем дескриптор события
            if (event1 == NULL) {
                SetWindowText(hStatic3, L"Can't open Event");
                return GetLastError();
            }

            DWORD result, thread3;

            HANDLE Mythead3;
            Mythead3 = CreateThread(NULL, 0, SendMsgFunc, 0, 0, &thread3);

            SetWindowText(hStatic3, L"Wait end of thread");
            WaitForSingleObject(Mythead3, INFINITE);
            GetExitCodeThread(Mythead3, &result);
            CloseHandle(Mythead3);
            SetWindowText(hStatic3, L"Thread end");

            if (result == 0) {
                SetWindowText(hStatic3, L"Can't find the window");
                return GetLastError();
            }

            SetEvent(event1);
            CloseHandle(event1);	// закрываем декриптор в текущем процессе 									

            SetWindowText(hStatic3, L"Send msg to prog");

            break;
        }
        case ID_BUTTON4:
        {
            threadfiber = ConvertThreadToFiber(NULL);		// Для использования волокна преобразуем первичный поток в волокно
            if (threadfiber == NULL)
                SetWindowText(hStatic4, L"Error thread to fiber");
            else
            {
                SetWindowText(hStatic4, L"Waiting fiber");
                fiber = CreateFiber(0, FunctionFiber, NULL);		// Создаем дополнительное волокно
                if (fiber == NULL)
                    SetWindowText(hStatic4, L"Error create fiber");
                else
                {
                    SwitchToFiber(fiber);		// Переключаемся на вторичное волокно для его выполнения
                    DeleteFiber(fiber);
                    SetWindowText(hStatic4, L"Fiber success");
                }
                ConvertFiberToThread();		// Преобразуем волокно обратно в поток
            }
        }
        break;
        case ID_BUTTON1:
        {
            CreateThread(NULL, 0, NoCritSecFunc, 0, 0, &thread2);
            break;
        }
        case ID_BUTTON2:
        {
            CreateThread(NULL, 0, CritSecFunc, 0, 0, &thread1);
            break;
        }
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_CREATE:
    {
        CreateWindow(L"button", L"NoCS", WS_CHILD | WS_VISIBLE,
            20, 20, 80, 30, hWnd, (HMENU)ID_BUTTON1, hInst, NULL);
        hStatic2 = CreateWindow(L"static", L"", WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE,
            120, 20, 160, 30, hWnd, NULL, hInst, NULL);
        CreateWindow(L"button", L"CS", WS_CHILD | WS_VISIBLE,
            20, 60, 80, 30, hWnd, (HMENU)ID_BUTTON2, hInst, NULL);
        hStatic = CreateWindow(L"static", L"", WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE,
            120, 60, 160, 30, hWnd, NULL, hInst, NULL);

        hButton1 = CreateWindow(L"button", L"msg", WS_CHILD | WS_VISIBLE,
            20, 100, 80, 30, hWnd, (HMENU)ID_BUTTON3, hInst, NULL);
        hStatic3 = CreateWindow(L"static", L"", WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE,
            120, 100, 160, 30, hWnd, NULL, hInst, NULL);

        hButton2 = CreateWindow(L"button", L"fiber", WS_CHILD | WS_VISIBLE,
            20, 140, 80, 30, hWnd, (HMENU)ID_BUTTON4, hInst, NULL);
        hStatic4 = CreateWindow(L"static", L"", WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE,
            120, 140, 160, 30, hWnd, NULL, hInst, NULL);

        InitializeCriticalSection(&critsec); // инициализация критической секции
        SetTimer(hWnd, 10, 1000, NULL);

        return 0;
    }

    case WM_TIMER:
    {
        EnterCriticalSection(&critsec);
        SetWindowText(hStatic, str1);
        LeaveCriticalSection(&critsec);
        SetWindowText(hStatic2, str2);
        return 0;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: Add any drawing code that uses hdc here...
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
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


DWORD WINAPI CritSecFunc(PVOID pParam)
{
    int a = 1;
    while (true)
    {
        EnterCriticalSection(&critsec);
        for (int i = 0; i < 5; i++) str1[i] = (char)a;
        a++;
        LeaveCriticalSection(&critsec);
    }

    return 0;
}

DWORD WINAPI NoCritSecFunc(PVOID pParam) // точка входа потока
{
    int a = 1;
    while (true)
    {
        for (int i = 0; i < 5; i++) str2[i] = (char)a;
        a++;
    }

    return 0;
}

DWORD WINAPI SendMsgFunc(PVOID pParam)
{
    Sleep(5000);

    HWND test = FindWindow(NULL, L"Lab33");
    if (test == NULL) {
        return 0;
    }
    else {
        PostMessage(test, WM_COMMAND, MAKEWPARAM(1, 0), NULL);
        return 1;
    }
}

VOID __stdcall FunctionFiber(LPVOID lpParameter)
{
    Sleep(2000);
    SwitchToFiber(threadfiber);
}
