#include "lib/common.h"
#include <CommCtrl.h>
#include "mainwindow.h"

#pragma comment(lib, "comctl32.lib")

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR lpCmdLine, int nCmdShow)
{
	INITCOMMONCONTROLSEX icc;
	icc.dwSize = sizeof(icc);
	icc.dwICC = ICC_WIN95_CLASSES|ICC_COOL_CLASSES;
	InitCommonControlsEx(&icc);

	MainWindow wnd;
	wnd.Show(nCmdShow);
	wnd.MainLoop();
	return 0;
}