#include <Windows.h>
#include "common.h"
#include "appwindow.h"

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR lpCmdLine, int nCmdShow)
{
	AppWindow wnd;
	wnd.Show(nCmdShow);
	wnd.MainLoop();
	return 0;
}