#include "common.h"
#include <CommCtrl.h>
#include "mainwindow.h"

#pragma comment(lib, "comctl32.lib")

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR lpCmdLine, int nCmdShow)
{
	INITCOMMONCONTROLSEX icc;
	icc.dwSize = sizeof(icc);
	icc.dwICC = ICC_COOL_CLASSES|ICC_BAR_CLASSES;
	InitCommonControlsEx(&icc);

	MainWindow wnd;
	wnd.Show(nCmdShow);

	HACCEL hAccel = LoadAccelerators(NULL, "AccelTable");

	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(wnd, hAccel, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return 0;
}