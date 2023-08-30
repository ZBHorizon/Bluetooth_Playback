#include <windows.h>


class Bluetooth_Playback {
/////////////////////////////////////////////////////////////////////public//////////////////////////////////////////////////////////////
public:
	//destructor
	~Bluetooth_Playback();

	// Klassen Funktion
	int Initialize(HINSTANCE hInstance);
	int Run();

/////////////////////////////////////////////////////////////////////private//////////////////////////////////////////////////////////////

private:
	// Klassen Funktion
	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

	// Klassen Variablen
	NOTIFYICONDATA nid;
	HWND hWnd;
	HINSTANCE hInstance;
};