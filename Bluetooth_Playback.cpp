#include "Bluetooth_Playback.hpp"


Bluetooth_Playback::~Bluetooth_Playback()
{
    //deconstructor
    Shell_NotifyIcon(NIM_DELETE, &nid);
}

//////////////////////////////////////////////////////////initialize//////////////////////////////////////////////////////////

int Bluetooth_Playback::Initialize(HINSTANCE hInstance){
    
    WNDCLASSEX wc;
    

    // Fensterklasse registrieren
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = Bluetooth_Playback::WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "TrayAppClass";
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, "Fensterklasse konnte nicht registriert werden!", "Fehler", MB_ICONEXCLAMATION | MB_OK);
        return 1;
    }

    // Fenster erstellen
    this->hWnd = ::CreateWindowEx(   //::steht für globale C Funktiion ->für klassenvariablen
        WS_EX_CLIENTEDGE,
        "TrayAppClass",
        "Tray App",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 240, 120,
        NULL, NULL, hInstance, NULL
    );

    // Tray Icon initialisieren
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hWnd;
    nid.uID = 1;
    nid.uVersion = NOTIFYICON_VERSION;
    nid.uCallbackMessage = WM_APP + 1;
    nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    strcpy_s(nid.szTip, sizeof(nid.szTip), "this is a hover field");
    nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;

    Shell_NotifyIcon(NIM_ADD, &nid);

    return 0;
}

//////////////////////////////////////////////////////////Run//////////////////////////////////////////////////////////

int Bluetooth_Playback::Run() {
    ShowWindow(this->hWnd, SW_HIDE);
    MSG message;
    while (GetMessage(&message, NULL, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }
    return static_cast<int>(message.wParam);
}

//////////////////////////////////////////////////////////WndProc//////////////////////////////////////////////////////////

LRESULT CALLBACK Bluetooth_Playback::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_APP + 1:
        if (lParam == WM_RBUTTONDOWN) {
            POINT pt;
            GetCursorPos(&pt);
            HMENU hMenu = CreatePopupMenu();
            AppendMenu(hMenu, MF_STRING, 1000, "Beenden");

            SetForegroundWindow(hWnd);
            TrackPopupMenu(hMenu, TPM_BOTTOMALIGN, pt.x, pt.y, 0, hWnd, NULL);
            PostMessage(hWnd, WM_NULL, 0, 0);

            DestroyMenu(hMenu);
        }
        break;
    case WM_COMMAND:
        if (LOWORD(wParam) == 1000) {
            DestroyWindow(hWnd);
        }
        break;
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}
