#include "mainwindow.h"

MainWindow::MainWindow() : gl_frame(NULL)
{
	gl_frame = new GLFrame();
	this->Create("Cubex", CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, WS_OVERLAPPEDWINDOW, 0, NULL);
}

void MainWindow::InitToolbar()
{
	HINSTANCE hInst = GetModuleHandle(NULL);
	const int numBtns = 5;
	hImgList = ImageList_Create(24, 24, ILC_MASK|ILC_COLOR32, numBtns, 0);
	hGrayedImgList = ImageList_Create(24, 24, ILC_MASK|ILC_COLOR32, numBtns, 0);
	
	int ids[numBtns] = { IDI_NEWGAME, IDI_MIXUP, IDI_CANCEL, IDI_SETTINGS, IDI_ABOUT };

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
		WS_CHILD|CCS_NOPARENTALIGN|CCS_NODIVIDER|TBSTYLE_FLAT|TBSTYLE_TOOLTIPS|CCS_NORESIZE,
		0, 0, 0, 0, m_hwnd, NULL, hInst, NULL);

	HWND hToolbar2 = CreateWindow(TOOLBARCLASSNAME, "",
		WS_CHILD|CCS_NOPARENTALIGN|CCS_NODIVIDER|TBSTYLE_FLAT|TBSTYLE_TOOLTIPS|CCS_NORESIZE,
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

	buttons[3].fsStyle |= BTNS_DROPDOWN;
	
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
	hRebar = CreateWindowEx(WS_EX_TOOLWINDOW, REBARCLASSNAME, NULL,
		WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|CCS_NODIVIDER,
		0, 0, 0, 0,
		m_hwnd, NULL, hInst, NULL);

	REBARBANDINFO rbBand = { };
	rbBand.cbSize = REBARBANDINFOA_V6_SIZE;
	rbBand.fMask = RBBIM_STYLE|RBBIM_CHILD|RBBIM_CHILDSIZE;
	rbBand.fStyle = RBBS_CHILDEDGE|RBBS_NOGRIPPER;
    rbBand.cyChild = LOWORD(tbBtnSize);
    rbBand.cyMinChild = LOWORD(tbBtnSize);
    rbBand.cx = 0;

	rbBand.hwndChild = hToolbar;
	rbBand.cxMinChild = numBtns * HIWORD(tbBtnSize);
	SendMessage(hRebar, RB_INSERTBAND, -1, (LPARAM)&rbBand);

	rbBand.cxMinChild = HIWORD(tbBtnSize);
	rbBand.hwndChild = hToolbar2;
	SendMessage(hRebar, RB_INSERTBAND, -1, (LPARAM)&rbBand);

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
	hSettingsMenu = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDM_SETTINGS));
	hSettingsMenu = GetSubMenu(hSettingsMenu, 0);

	RECT wndRect, barRect = { };
	GetClientRect(m_hwnd, &wndRect);
	GetClientRect(hRebar, &barRect);

	gl_frame->CreateParam("Cubex", 0, barRect.bottom, wndRect.right,
		wndRect.bottom - barRect.bottom, WS_VISIBLE|WS_CHILD, 0, m_hwnd);
	SetFocus(gl_frame->m_hwnd);

	EnableCancelBtn(gl_frame->CanCancelMove());
	CheckMenuRadioItem(hSettingsMenu, IDM_2, IDM_7, IDM_2 + gl_frame->GetCubeSize() - 2, MF_BYCOMMAND);

	if (GLEW_ARB_shader_objects) {
		DWORD white = 0;
		HKEY key = NULL;
		LONG r = RegCreateKey(HKEY_CURRENT_USER, "software\\Alexander Gaivanuk\\Cubex\\1.1", &key);
		if (r == ERROR_SUCCESS)
		{
			DWORD type = 0, size = sizeof(DWORD);
			RegQueryValueEx(key, "white", NULL, &type, (BYTE *)&white, &size);
			if (white != 0)
				gl_frame->SetCubeStyle(true);
			RegCloseKey(key);
		}
		CheckMenuRadioItem(hSettingsMenu, IDM_BLACK, IDM_WHITE, white ? IDM_WHITE : IDM_BLACK, MF_BYCOMMAND);
	}
	else {
		DeleteMenu(hSettingsMenu, 0, MF_BYPOSITION);
	}
	return 0;
}

LRESULT MainWindow::OnCommand(UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) == BN_CLICKED) {
		WORD id = LOWORD(wParam);
		switch(id) 
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
		case IDM_BLACK:
		case IDM_WHITE:
		{
			gl_frame->SetCubeStyle(id == IDM_WHITE);
			CheckMenuRadioItem(hSettingsMenu, IDM_BLACK, IDM_WHITE, id, MF_BYCOMMAND);

			HKEY key = NULL;
			LONG r = RegCreateKey(HKEY_CURRENT_USER, "software\\Alexander Gaivanuk\\Cubex\\1.1", &key);
			if (r == ERROR_SUCCESS)
			{
				DWORD value = id == IDM_WHITE;
				RegSetValueEx(key, "white", 0, REG_DWORD, (BYTE *)&value, sizeof(DWORD));
				RegCloseKey(key);
			}

			break;
		}
		default:
			if (id >= IDM_2 && id <= IDM_7) {
				if (gl_frame->ChangeCubeSize(id - IDM_2 + 2)) {
					CheckMenuRadioItem(hSettingsMenu, IDM_2, IDM_7, id, MF_BYCOMMAND);
					EnableCancelBtn(gl_frame->CanCancelMove());
				}
			}
			break;
		};
	}
	return 0;
}

LRESULT MainWindow::OnNotify(UINT msg, WPARAM wParam, LPARAM lParam)
{
	NMHDR *hdr = (NMHDR *)lParam;
	if (hdr->code == TBN_DROPDOWN)
	{
		NMTOOLBAR *t = (NMTOOLBAR *)lParam;
		if (t->iItem == IDC_SETTINGS) {
			RECT btnRect = { };
			SendMessage(hdr->hwndFrom, TB_GETRECT, IDC_SETTINGS, (LPARAM)&btnRect);

			MapWindowPoints(hdr->hwndFrom, HWND_DESKTOP, (LPPOINT)&btnRect, 2);
			TrackPopupMenu(hSettingsMenu, TPM_LEFTALIGN|TPM_TOPALIGN|TPM_RIGHTBUTTON,
				btnRect.left, btnRect.bottom, 0, m_hwnd, NULL);
		}
	}
	return TBDDRET_DEFAULT;
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
	SetWindowPos(gl_frame->m_hwnd, HWND_TOP, 0, 0, w, h - barRect.bottom,
		SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOOWNERZORDER|SWP_NOZORDER);
	return 0;
}

LRESULT MainWindow::OnDestroy(UINT msg, WPARAM wParam, LPARAM lParam)
{
	delete gl_frame;
	ImageList_Destroy(hImgList);
	ImageList_Destroy(hGrayedImgList);
	DestroyMenu(hSettingsMenu);
	PostQuitMessage(0);
	return 0;
}