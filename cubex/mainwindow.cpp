#include "mainwindow.h"

MainWindow::MainWindow()
	: gl_frame(NULL)
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
	hImgList = ImageList_Create(24, 24, ILC_MASK|ILC_COLOR24, 0, 3);
	hGrayedImgList = ImageList_Create(24, 24, ILC_MASK|ILC_COLOR24, 0, 3);

	const int numBtns = 3;
	int ids[numBtns] = { IDI_NEWGAME, IDI_SHUFFLE, IDI_CANCEL };
	for (int i = 0; i < numBtns; i++) {
		HBITMAP b = LoadBitmap(hInst, MAKEINTRESOURCE(ids[i]));
		ImageList_Add(hImgList, b, b);
		if (ids[i] == IDI_CANCEL) {
			HBITMAP cancel_grayed = LoadBitmap(hInst, MAKEINTRESOURCE(IDI_CANCEL_GRAYED));
			ImageList_Add(hGrayedImgList, cancel_grayed, b);
			DeleteObject(cancel_grayed);
		}
		else ImageList_Add(hGrayedImgList, b, b);
		DeleteObject(b);
	}

	hToolbar = CreateWindow(TOOLBARCLASSNAME, "",
		WS_CHILD|CCS_NOPARENTALIGN|CCS_NODIVIDER|TBSTYLE_FLAT|TBSTYLE_WRAPABLE|TBSTYLE_TOOLTIPS,
		0, 0, 0, 0, m_hwnd, NULL, hInst, NULL);

	SendMessage(hToolbar, TB_ADDSTRING, (WPARAM)hInst, IDS_TOOLBARNAMES);

	TBBUTTON buttons[numBtns] = { };

	for (int i = 0; i < numBtns; i++) {
		buttons[i].iBitmap = i;
		buttons[i].idCommand = IDC_NEWGAME + i;
		buttons[i].fsState = TBSTATE_ENABLED;
		buttons[i].fsStyle = TBSTYLE_BUTTON;
		buttons[i].iString = i;
	}
	
	SendMessage(hToolbar, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
	SendMessage(hToolbar, TB_SETIMAGELIST, 0, (LPARAM)hImgList);
	SendMessage(hToolbar, TB_SETDISABLEDIMAGELIST, 0, (LPARAM)hGrayedImgList);
	SendMessage(hToolbar, TB_ADDBUTTONS, numBtns, (LPARAM)buttons);
	SendMessage(hToolbar, TB_SETMAXTEXTROWS, 0, 0);

	DWORD tbBtnSize = SendMessage(hToolbar, TB_GETBUTTONSIZE, 0, 0);
	hRebar = CreateWindowEx(0, REBARCLASSNAME, "",
		WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|CCS_NODIVIDER|RBS_AUTOSIZE|RBS_BANDBORDERS,
		0, 0, 0, 0,
		m_hwnd, NULL, hInst, NULL);

	REBARBANDINFO rbBand = { };
	rbBand.cbSize = REBARBANDINFOA_V3_SIZE;
	rbBand.fMask = RBBIM_STYLE|RBBIM_CHILD|RBBIM_CHILDSIZE;
	rbBand.fStyle = RBBS_CHILDEDGE|RBBS_NOGRIPPER;
	rbBand.hwndChild = hToolbar;
	rbBand.cxMinChild = LOWORD(tbBtnSize);
	rbBand.cyMinChild = HIWORD(tbBtnSize);

	SendMessage(hRebar, RB_INSERTBAND, 0, (LPARAM)&rbBand);
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

LRESULT MainWindow::OnCreate(UINT msg, WPARAM wParam, LPARAM lParam)
{
	InitToolbar();
	EnableCancelBtn(false);

	RECT wndRect, barRect = { };
	GetClientRect(m_hwnd, &wndRect);
	GetClientRect(hRebar, &barRect);

	gl_frame = new GLFrame();
	gl_frame->CreateParam("Cubex", 0, barRect.bottom - barRect.top, wndRect.right - wndRect.left,
		wndRect.bottom - wndRect.top, WS_VISIBLE|WS_CHILD, 0, m_hwnd);

	return 0;
}

LRESULT MainWindow::OnCommand(UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) == BN_CLICKED) {
		switch(LOWORD(wParam)) 
		{
		case IDC_NEWGAME:
			gl_frame->ResetCube();
			EnableCancelBtn(false);
			break;
		case IDC_SHUFFLE:
			gl_frame->ShuffleCube();
			EnableCancelBtn(false);
			break;
		case IDC_CANCEL:
			gl_frame->CancelMove();
			if (!gl_frame->CanCancelMove()) {
				EnableCancelBtn(false);
			}
			break;
		};
	}
	return 0;
}

LRESULT MainWindow::OnFaceRotate(UINT msg, WPARAM wParam, LPARAM lParam)
{
	EnableCancelBtn(true);
	return 0;
}

LRESULT MainWindow::OnSize(UINT msg, WPARAM wParam, LPARAM lParam)
{
	int w = LOWORD(lParam);
	int h = HIWORD(lParam);

	RECT barRect = { };
	GetWindowRect(hRebar, &barRect);

	SetWindowPos(hRebar, HWND_TOP, 0, 0,
			LOWORD(lParam), barRect.bottom - barRect.top,
			SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOOWNERZORDER|SWP_NOZORDER);

	SetWindowPos(gl_frame->m_hwnd, HWND_TOP, 0, 0,
		w, h - (barRect.bottom - barRect.top),
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