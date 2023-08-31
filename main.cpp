//Main.cpp
#include "Bluetooth_Playback.hpp"


int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
    Bluetooth_Playback btinstance;
    int errorcode = btinstance.Initialize(hInstance);
    if (errorcode != 0) return errorcode;

    return btinstance.Run();
}