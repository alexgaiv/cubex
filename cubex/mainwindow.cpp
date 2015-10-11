#include "mainwindow.h"

MainWindow::MainWindow() : gl_frame(NULL)
{
	this->Create("Cubex", CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, WS_OVERLAPPEDWINDOW, 0, NULL);
}

MainWindow::~MainWindow()
{
	if (gl_frame) delete gl_frame;
}

void MainWindow::InitToolbar()
{
	HINSTANCE hInst = GetModuleHandle(NULL);
	hImgList = ImageList_Create(24, 24, ILC_MASK|ILC_COLOR32, 3, 0);
	hGrayedImgList = ImageList_Create(24, 24, ILC_MASK|ILC_COLOR32, 3, 0);

	const int numBtns = 4;
	int ids[numBtns] = { IDI_NEWGAME, IDI_MIXUP, IDI_CANCEL, IDI_ABOUT };

	for (int i = 0; i < numBtns; i++) {
		HICON hIcon = (HICON)LoadImage(hInst, MAKEINTRESOURCE(ids[i]), IMAGE_ICON, 24, 24, 0);
		ImageList_AddIcon(hImgList, hIcon);

		if (ids[i] == IDI_CANCEL) {
			HICON cancel_grayed = (HICON)LoadImage(hInst, MAKEINTRESOURCE(IDI_CANCEL_GRAYED), IMAGE_ICON, 24, 24, 0);
			ImageList_AddIcon(hGrayedImgList, cancel_grayed);
		}
		else ImageList_AddIcon(hGrayedImgList, hIcon);
	}

	hToolbar = CreateWindow(TOOLBARCLASSNAME, "",
		WS_CHILD|CCS_NOPARENTALIGN|CCS_NODIVIDER|TBSTYLE_FLAT|TBSTYLE_WRAPABLE|TBSTYLE_TOOLTIPS,
		0, 0, 0, 0, m_hwnd, NULL, hInst, NULL);

	HWND hToolbar2 = CreateWindow(TOOLBARCLASSNAME, "",
		WS_CHILD|CCS_NOPARENTALIGN|CCS_NODIVIDER|TBSTYLE_FLAT|TBSTYLE_WRAPABLE|TBSTYLE_TOOLTIPS,
		0, 0, 0, 0, m_hwnd, NULL, hInst, NULL);

	SendMessage(hToolbar, TB_ADDSTRING, (WPARAM)hInst, IDS_TOOLBARNAMES);
	SendMessage(hToolbar2, TB_ADDSTRING, (WPARAM)hInst, IDS_MENU_ABOUT);

	TBBUTTON buttons[numBtns] = { };

	for (int i = 0; i < numBtns; i++) {
		buttons[i].iBitmap = i;
		buttons[i].idCommand = IDC_NEWGAME + i;
		buttons[i].fsState = TBSTATE_ENABLED;
		buttons[i].fsStyle = TBSTYLE_BUTTON;
		buttons[i].iString = i < numBtns - 1 ? i : 0;
	}
	
	SendMessage(hToolbar, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
	SendMessage(hToolbar, TB_SETIMAGELIST, 0, (LPARAM)hImgList);
	SendMessage(hToolbar, TB_SETDISABLEDIMAGELIST, 0, (LPARAM)hGrayedImgList);
	SendMessage(hToolbar, TB_ADDBUTTONS, numBtns - 1, (LPARAM)buttons);
	SendMessage(hToolbar, TB_SETMAXTEXTROWS, 0, 0);

	SendMessage(hToolbar2, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
	SendMessage(hToolbar2, TB_SETIMAGELIST, 0, (LPARAM)hImgList);
	SendMessage(hToolbar2, TB_ADDBUTTONS, 1, (LPARAM)&buttons[numBtns - 1]);
	SendMessage(hToolbar2, TB_SETMAXTEXTROWS, 0, 0);

	DWORD tbBtnSize = SendMessage(hToolbar, TB_GETBUTTONSIZE, 0, 0);
	hRebar = CreateWindowEx(WS_EX_TOOLWINDOW, REBARCLASSNAME, "",
		WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|CCS_NODIVIDER,
		0, 0, 0, 0,
		m_hwnd, NULL, hInst, NULL);

	REBARBANDINFO rbBand[2] = { };
	for (int i = 0; i < 2; i++) {
		rbBand[i].cbSize = REBARBANDINFOA_V6_SIZE;
		rbBand[i].fMask = RBBIM_STYLE|RBBIM_CHILD|RBBIM_CHILDSIZE;
		rbBand[i].fStyle = RBBS_CHILDEDGE|RBBS_NOGRIPPER;
		rbBand[i].hwndChild = i == 0 ? hToolbar : hToolbar2;
		rbBand[i].cxMinChild = LOWORD(tbBtnSize);
		rbBand[i].cyMinChild = HIWORD(tbBtnSize);
		SendMessage(hRebar, RB_INSERTBAND, i, (LPARAM)&rbBand[i]);
	}
	SendMessage(hRebar, RB_MAXIMIZEBAND, 0, 0);
}

WindowInfoStruct MainWindow::GetWindowInfo()
{
	WindowInfoStruct wi = { };
	HINSTANCE hInst = GetModuleHandle(NULL);

	wi.hCursor = LoadCursor(NULL, IDC_ARROW);
	wi.hIcon = (HICON)LoadImage(hInst, MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, 32, 32, 0);
	wi.hIconSmall = (HICON)LoadImage(hInst, MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, 16, 16, 0);
	wi.lpClassName = "mainwindow";
	return wi;
}

INT_PTR WINAPI MainWindow::AboutDialogProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
	{
		HINSTANCE hInst = GetModuleHandle(NULL);
		HICON hIcon = (HICON)LoadImage(hInst,
			MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, 24, 24, 0);
		SendMessage(hwndDlg, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

		WCHAR caption[100] = L"";
		LoadStringW(hInst, IDS_ABOUT, caption, 20);
		SetWindowTextW(hwndDlg, caption);
		return TRUE;
	}
	case WM_COMMAND:
		WORD id = LOWORD(wParam);
		if (id == IDOK || id == IDCANCEL) {
			EndDialog(hwndDlg, 0);
			return TRUE;
		}
	};
		
	return FALSE;
}

LRESULT MainWindow::OnCreate(UINT msg, WPARAM wParam, LPARAM lParam)
{
	InitToolbar();
	EnableCancelBtn(false);

	RECT wndRect, barRect = { };
	GetClientRect(m_hwnd, &wndRect);
	GetClientRect(hRebar, &barRect);

	gl_frame = new GLFrame();
	gl_frame->CreateParam("Cubex", 0, barRect.bottom, wndRect.right,
		wndRect.bottom - barRect.bottom, WS_VISIBLE|WS_CHILD, 0, m_hwnd);
	SetFocus(gl_frame->m_hwnd);

	return 0;
}

LRESULT MainWindow::OnCommand(UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) == BN_CLICKED) {
		switch(LOWORD(wParam)) 
		{
		case IDC_NEWGAME:
			{
			gl_frame->ResetCube();
			EnableCancelBtn(false);
			break;
			}
		case IDC_MIXUP:
			gl_frame->MixUpCube();
			EnableCancelBtn(false);
			break;
		case IDC_CANCEL:
			gl_frame->CancelMove();
			if (!gl_frame->CanCancelMove()) {
				EnableCancelBtn(false);
			}
			break;
		case IDC_ABOUT:
			DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ABOUT_DIALOG), m_hwnd, AboutDialogProc);
			break;
		};
	}
	return 0;
}

LRESULT MainWindow::OnFaceRotate(UINT msg, WPARAM wParam, LPARAM lParam) {
	EnableCancelBtn(true);
	return 0;
}

LRESULT MainWindow::OnCubeSolved(UINT msg, WPARAM wParam, LPARAM lParam) {
	EnableCancelBtn(false);
	return 0;
}

LRESULT MainWindow::OnSize(UINT msg, WPARAM wParam, LPARAM lParam)
{
	int w = LOWORD(lParam);
	int h = HIWORD(lParam);

	RECT barRect = { };
	GetClientRect(hRebar, &barRect);

	SetWindowPos(hRebar, HWND_TOP, 0, 0, w, barRect.bottom,
		SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOOWNERZORDER|SWP_NOZORDER);
	GetClientRect(hRebar, &barRect);

	SendMessage(hRebar, RB_MAXIMIZEBAND, 0, 0);
	SendMessage(hRebar, RB_SHOWBAND, 1, TRUE);

	SetWindowPos(gl_frame->m_hwnd, HWND_TOP, 0, 0, w, h - barRect.bottom,
		SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOOWNERZORDER|SWP_NOZORDER);
	return 0;
}

LRESULT MainWindow::OnDestroy(UINT msg, WPARAM wParam, LPARAM lParam)
{
	ImageList_Destroy(hImgList);
	ImageList_Destroy(hGrayedImgList);
	PostQuitMessage(0);
	return 0;
}