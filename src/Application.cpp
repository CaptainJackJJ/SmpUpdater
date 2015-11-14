
#include <windows.h>
#include "winsparkle.h"
#include "settings.h"
#include <wx/utils.h>
#include "Globals.h"
#include "utils\log.h"
#include "utils\StringUtil.h"
#include <shlobj.h>

using namespace winsparkle;


LRESULT CALLBACK onMainWndMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;    
	default:
		return DefWindowProc(wnd, msg, wParam, lParam);
	}
	return 0;
}

bool registerMyClass()
{
	WNDCLASSEX  wce = { 0 };
	wce.cbSize = sizeof(wce);
	wce.style = CS_VREDRAW | CS_HREDRAW;
	wce.lpfnWndProc = &onMainWndMessage;  
	wce.hInstance = GetModuleHandle(0);
	wce.hIcon = LoadIcon(0, MAKEINTRESOURCE(IDI_WINLOGO));
	wce.hCursor = LoadCursor(0, MAKEINTRESOURCE(IDC_ARROW));
	wce.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_BTNFACE + 1);
	wce.lpszClassName = L"ClassName"; 
	wce.hIconSm = wce.hIcon;
	return 0 != RegisterClassEx(&wce);
}

bool createMyWindow(int cmdShow)
{
	HWND mainWnd = CreateWindowEx(0, L"ClassName", L"Demo", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		0, 0, GetModuleHandle(0), 0);
	if (0 != mainWnd) 
	{
		//ShowWindow(mainWnd, cmdShow);
		//UpdateWindow(mainWnd);
		return true;
	}
	else 
	{
		return false;
	}
}

int messageLoop() 
{
	MSG msg;
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return static_cast<int>(msg.wParam);
}



INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR commandLine, INT cmdShow)
{
	wchar_t LogDir[MAX_PATH];
	HRESULT hr = SHGetFolderPathW(0, CSIDL_MYDOCUMENTS, 0, 0, LogDir);
	if (!SUCCEEDED(hr))
		assert(0);

	std::wstring strLogDir = LogDir;

	CLog::Init(strLogDir + "\\", UPDATER_NAME);

#ifdef _DEBUG
	CLog::SetLogLevel(LOG_LEVEL_DEBUG);
#else
	CLog::SetLogLevel(LOG_LEVEL_NORMAL);
#endif

	CLog::Log(LOGINFO, "Startup %s", UPDATER_VERSION);

	//Set update check info by register info

	win_sparkle_set_app_details(COMPANY_NAME, APP_NAME);

	win_sparkle_init();

	if (registerMyClass() && createMyWindow(cmdShow))
	{
		return messageLoop();
	}
	else 
	{
		std::ostringstream msg;
		msg << "Create main wnd failed, error code£º" << GetLastError();
		MessageBoxA(0, msg.str().c_str(), 0, MB_OK | MB_ICONSTOP);
		return 0;
	}

	return 0;
}