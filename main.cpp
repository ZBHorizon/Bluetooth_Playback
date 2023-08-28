#include "Bluetooth_Playback.hpp"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    Bluetooth_Playback btinstance;
    int errorcode = btinstance.Initialize(hInstance);
    if (errorcode != 0) return errorcode;

    return btinstance.Run();
    
}


