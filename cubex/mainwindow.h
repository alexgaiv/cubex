#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H_

#include <Windows.h>
#include <CommCtrl.h>
#include "resources.h"
#include "basewindow.h"
#include "glframe.h"

class MainWindow : public BaseWindow
{
public:
	MainWindow();
	~MainWindow();
private:
	GLFrame *gl_frame;
	HWND hToolbar, hRebar;
	HIMAGELIST hImgList, hGrayedImgList;
	HMENU hSettingsMenu;

	void InitToolbar();
	void EnableCancelBtn(bool enabled) {
		SendMessage(hToolbar, TB_ENABLEBUTTON, IDC_CANCEL, (BOOL)enabled);
	}
	WindowInfoStruct GetWindowInfo();

	static INT_PTR WINAPI AboutDialogProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
	
	LRESULT OnCreate(UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnSize(UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnNotify(UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnFaceRotate(UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCubeSolved(UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDestroy(UINT msg, WPARAM wParam, LPARAM lParam);

	BEGIN_MSG_MAP
		MSG_HANDLER(WM_CREATE, OnCreate)
		MSG_HANDLER(WM_SIZE, OnSize)
		MSG_HANDLER(WM_COMMAND, OnCommand)
		MSG_HANDLER(WM_NOTIFY, OnNotify)
		MSG_HANDLER(WM_ROTATEFACE, OnFaceRotate)
		MSG_HANDLER(WM_CUBESOLVED, OnCubeSolved)
		MSG_HANDLER(WM_DESTROY, OnDestroy)
	END_MSG_MAP
};

#endif // _MAIN_WINDOW_H_