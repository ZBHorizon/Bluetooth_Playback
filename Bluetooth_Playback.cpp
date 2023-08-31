//Bluetooth_Playback.cpp
#include "Bluetooth_Playback.hpp"
#include "resource.h"

//__________deconstructor______________
Bluetooth_Playback::~Bluetooth_Playback()
{
    Shell_NotifyIcon(NIM_DELETE, &nid);
}

//__________________________________________________CheckSystemDesign___________________________________________________
//######################################################################################################################

void CheckSystemDesign(DWORD& lightmode_value, DWORD& transparancy_value) {
    DWORD cbValue = sizeof(DWORD);
    DWORD dwType = 0;

    // Lesen des "SystemUsesLightTheme" Werts
    LSTATUS status1 = RegGetValueW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"SystemUsesLightTheme",
        RRF_RT_REG_DWORD,
        &dwType,
        &lightmode_value,
        &cbValue
    );

    // Lesen des "EnableTransparency" Werts
    LSTATUS status2 = RegGetValueW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"EnableTransparency",
        RRF_RT_REG_DWORD,
        &dwType,
        &transparancy_value,
        &cbValue
    );

    if (status1 != ERROR_SUCCESS || dwType != REG_DWORD) {
        // Fehler beim Lesen des Werts für "SystemUsesLightTheme"
        lightmode_value = -1;
    }

    if (status2 != ERROR_SUCCESS || dwType != REG_DWORD) {
        // Fehler beim Lesen des Werts für "EnableTransparency"
        transparancy_value = -1;
    }
}

void UpdateIcon() {

    return;
}
//_______________________________________________________Initialize________________________________________________________
//#########################################################################################################################

int Bluetooth_Playback::Initialize(HINSTANCE hInstance){
    //klassenvariable definieren
    this->hInstance = hInstance;
    WNDCLASSEX wc{};

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
        NULL, NULL, hInstance, this
    );

    if (this->hWnd == NULL) {
        MessageBox(NULL, "Fenster konnte nicht erstellt werden!", "Fehler", MB_ICONEXCLAMATION | MB_OK);
        return 2;
    }
    // Tray Icon initialisieren
    nid.cbSize = sizeof(NOTIFYICONDATA); 
    nid.hWnd = hWnd; 
    nid.uID = 1; // Eindeutige ID für das Icon (muss eindeutig innerhalb der Anwendung sein)
    nid.uVersion = NOTIFYICON_VERSION; 
    nid.uCallbackMessage = WM_APP + 1; // für die Interaktion (links oder rechtsklick aufs icon)
    //automattisch ID_DARK_ICON für darkmode oder ID_LIGHT_ICON für lightmode nutzen
    DWORD lightmode_value, transparancy_value;
    CheckSystemDesign(lightmode_value, transparancy_value);
    nid.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(lightmode_value == 0 ? ID_DARK_ICON : ID_LIGHT_ICON)); 

    strcpy_s(nid.szTip, sizeof(nid.szTip), "Bluetooth Playback"); 
    nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP; // Flags, die angeben, welche der vorherigen Felder verwendet werden sollen

    // Fügt das Icon zum System-Tray hinzu
    Shell_NotifyIcon(NIM_ADD, &nid); // Fügt das Icon zum System-Tray hinzu
    
    SetWindowLongPtr(this->hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    return 0;
}

//________________________________________________________Run_________________________________________________________
//####################################################################################################################

int Bluetooth_Playback::Run() {
    ShowWindow(this->hWnd, SW_HIDE);
    MSG message;
    while (GetMessage(&message, NULL, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }
    return static_cast<int>(message.wParam);
}

//_______________________________________________________WndProc________________________________________________________
//######################################################################################################################

LRESULT CALLBACK Bluetooth_Playback::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    //auto* bluetooth_playback = reinterpret_cast<Bluetooth_Playback*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));//pointer
    Bluetooth_Playback* bluetooth_playback = reinterpret_cast<Bluetooth_Playback*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    switch (msg) {
        //___________________________________________WM_SETTINGCHANGE___________________________________________________
        case WM_SETTINGCHANGE: {  // Oder welches Windows Message Sie verwenden möchten
            if (bluetooth_playback == nullptr) { 
                return DefWindowProc(hWnd, msg, wParam, lParam); //schutz vor fehler
            }
            DWORD lightmode_value, transparancy_value;
            CheckSystemDesign(lightmode_value, transparancy_value);

            // Überprüfen, ob sich der Modus geändert hat
            if (current_lightmode != lightmode_value) {
                current_lightmode = lightmode_value;

                // Laden des entsprechenden Icons
                NOTIFYICONDATA nidUpdate = {};
                // Entscheiden, welches Icon basierend auf dem Modus zu verwenden ist
                nidUpdate.hIcon = LoadIcon(bluetooth_playback->hInstance, MAKEINTRESOURCE((lightmode_value == 0) ? ID_DARK_ICON : ID_LIGHT_ICON));
                nidUpdate.cbSize = sizeof(NOTIFYICONDATA);
                nidUpdate.hWnd = hWnd;
                nidUpdate.uID = 1;
                nidUpdate.uFlags = NIF_ICON;

                Shell_NotifyIcon(NIM_MODIFY, &nidUpdate);
            }
        }break;
        //______________________________________________WM_APP + 1_______________________________________________________
        case WM_APP + 1:{
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
        }break;
        //_______________________________________________WM_COMMAND_______________________________________________________
        case WM_COMMAND: {
            if (LOWORD(wParam) == 1000) {
                DestroyWindow(hWnd);
            }
        } break;
        case WM_CLOSE: DestroyWindow(hWnd); break;
        case WM_DESTROY: PostQuitMessage(0); break;
        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
            
    }
    return 0;
}