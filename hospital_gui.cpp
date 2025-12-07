#include <windows.h>
#include <string>
#include <vector>

#pragma comment(lib, "comctl32.lib")

#define IDC_EDIT_NAME     101
#define IDC_RADIO_NORMAL  102
#define IDC_RADIO_EMER    103
#define IDC_BTN_ADD       104
#define IDC_BTN_SERVE     105
#define IDC_LIST_NORMAL   106
#define IDC_LIST_EMER     107
#define IDC_BTN_CLEAR     108

HINSTANCE g_hInst = NULL;
std::vector<std::wstring> normalQueue;
std::vector<std::wstring> emergQueue;

void AddListboxString(HWND hList, const std::wstring &s) {
    SendMessageW(hList, LB_ADDSTRING, 0, (LPARAM)s.c_str());
}

void UpdateListboxes(HWND hNormalList, HWND hEmerList) {
    SendMessageW(hNormalList, LB_RESETCONTENT, 0, 0);
    SendMessageW(hEmerList, LB_RESETCONTENT, 0, 0);
    for (auto &p : normalQueue) AddListboxString(hNormalList, p);
    for (auto &p : emergQueue) AddListboxString(hEmerList, p);
}

std::wstring GetWindowTextWstr(HWND hWnd) {
    int len = GetWindowTextLengthW(hWnd);
    std::wstring buf;
    buf.resize(len + 1);
    GetWindowTextW(hWnd, &buf[0], len + 1);
    buf.resize(len);
    return buf;
}

void ShowMessage(HWND parent, const wchar_t* title, const std::wstring &msg) {
    MessageBoxW(parent, msg.c_str(), title, MB_OK | MB_ICONINFORMATION);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND hEditName, hRadioNormal, hRadioEmer, hBtnAdd, hBtnServe;
    static HWND hListNormal, hListEmer, hBtnClear;

    switch (msg) {
    case WM_CREATE:
        CreateWindowW(L"STATIC", L"Hospital Patient Queue Management System",
                      WS_VISIBLE | WS_CHILD | SS_CENTER,
                      10, 10, 560, 24, hWnd, NULL, g_hInst, NULL);

        CreateWindowW(L"STATIC", L"Patient Name:",
                      WS_VISIBLE | WS_CHILD,
                      10, 50, 100, 20, hWnd, NULL, g_hInst, NULL);

        hEditName = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
                      WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
                      115, 48, 250, 24, hWnd, (HMENU)IDC_EDIT_NAME, g_hInst, NULL);

        hRadioNormal = CreateWindowW(L"BUTTON", L"Normal",
                      WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
                      380, 48, 100, 20, hWnd, (HMENU)IDC_RADIO_NORMAL, g_hInst, NULL);

        hRadioEmer = CreateWindowW(L"BUTTON", L"Emergency",
                      WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
                      380, 70, 100, 20, hWnd, (HMENU)IDC_RADIO_EMER, g_hInst, NULL);

        SendMessageW(hRadioNormal, BM_SETCHECK, BST_CHECKED, 0);

        hBtnAdd = CreateWindowW(L"BUTTON", L"Add Patient",
                      WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                      115, 86, 120, 28, hWnd, (HMENU)IDC_BTN_ADD, g_hInst, NULL);

        hBtnServe = CreateWindowW(L"BUTTON", L"Serve Next Patient",
                      WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                      245, 86, 170, 28, hWnd, (HMENU)IDC_BTN_SERVE, g_hInst, NULL);

        CreateWindowW(L"STATIC", L"Normal Queue",
                      WS_VISIBLE | WS_CHILD,
                      10, 130, 260, 20, hWnd, NULL, g_hInst, NULL);

        CreateWindowW(L"STATIC", L"Emergency Queue",
                      WS_VISIBLE | WS_CHILD,
                      300, 130, 260, 20, hWnd, NULL, g_hInst, NULL);

        hListNormal = CreateWindowW(L"LISTBOX", NULL,
                      WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_NOTIFY | LBS_HASSTRINGS | WS_VSCROLL,
                      10, 155, 260, 200, hWnd, (HMENU)IDC_LIST_NORMAL, g_hInst, NULL);

        hListEmer = CreateWindowW(L"LISTBOX", NULL,
                      WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_NOTIFY | LBS_HASSTRINGS | WS_VSCROLL,
                      300, 155, 260, 200, hWnd, (HMENU)IDC_LIST_EMER, g_hInst, NULL);

        hBtnClear = CreateWindowW(L"BUTTON", L"Clear All Queues",
                      WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                      200, 370, 200, 28, hWnd, (HMENU)IDC_BTN_CLEAR, g_hInst, NULL);

        UpdateListboxes(hListNormal, hListEmer);
        break;

    case WM_COMMAND: {
        int id = LOWORD(wParam);
        int code = HIWORD(wParam);

        HWND hListNormal = GetDlgItem(hWnd, IDC_LIST_NORMAL);
        HWND hListEmer = GetDlgItem(hWnd, IDC_LIST_EMER);
        HWND hEditName = GetDlgItem(hWnd, IDC_EDIT_NAME);
        HWND hRadioNormal = GetDlgItem(hWnd, IDC_RADIO_NORMAL);
        HWND hRadioEmer = GetDlgItem(hWnd, IDC_RADIO_EMER);

        if (id == IDC_BTN_ADD && code == BN_CLICKED) {
            std::wstring name = GetWindowTextWstr(hEditName);

            while (!name.empty() && iswspace(name.back())) name.pop_back();
            while (!name.empty() && iswspace(name.front())) name.erase(name.begin());

            if (name.empty()) {
                ShowMessage(hWnd, L"Input Error", L"Please enter a patient name.");
            } else {
                if (SendMessageW(hRadioEmer, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                    emergQueue.push_back(name);
                    ShowMessage(hWnd, L"Success", L"Emergency patient added.");
                } else {
                    normalQueue.push_back(name);
                    ShowMessage(hWnd, L"Success", L"Normal patient added.");
                }
                SetWindowTextW(hEditName, L"");
                UpdateListboxes(hListNormal, hListEmer);
            }
        }

        else if (id == IDC_BTN_SERVE && code == BN_CLICKED) {
            if (!emergQueue.empty()) {
                std::wstring served = emergQueue.front();
                emergQueue.erase(emergQueue.begin());
                ShowMessage(hWnd, L"Serving Patient", L"Serving Emergency patient: " + served);
            } else if (!normalQueue.empty()) {
                std::wstring served = normalQueue.front();
                normalQueue.erase(normalQueue.begin());
                ShowMessage(hWnd, L"Serving Patient", L"Serving Normal patient: " + served);
            } else {
                ShowMessage(hWnd, L"Info", L"No patients in queue.");
            }
            UpdateListboxes(hListNormal, hListEmer);
        }

        else if (id == IDC_BTN_CLEAR && code == BN_CLICKED) {
            normalQueue.clear();
            emergQueue.clear();
            UpdateListboxes(hListNormal, hListEmer);
            ShowMessage(hWnd, L"Cleared", L"All queues cleared.");
        }
    } break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProcW(hWnd, msg, wParam, lParam);
    }
    return 0;
}
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int nCmdShow)
{
    g_hInst = hInst;

    const wchar_t CLASS_NAME[] = L"HospitalQueueClass";

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClassW(&wc);

    HWND hWnd = CreateWindowExW(
        0,
        CLASS_NAME,
        L"Hospital Patient Queue Management System",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 450,
        NULL, NULL, hInst, NULL);

    if (!hWnd) return 0;

    ShowWindow(hWnd, nCmdShow);

    MSG msg = {};
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return 0;
}

