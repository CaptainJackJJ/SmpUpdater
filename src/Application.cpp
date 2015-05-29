
#include <windows.h>
#include "winsparkle.h"
#include "settings.h"
#include <wx/utils.h>
#include "Globals.h"
#include "utils\log.h"
#include "utils\StringUtil.h"
#include <shlobj.h>

using namespace winsparkle;


INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR commandLine, INT)
{
	wchar_t LogPath[MAX_PATH];
	HRESULT hr = SHGetFolderPathW(0, CSIDL_MYDOCUMENTS, 0, 0, LogPath);
	if (!SUCCEEDED(hr))
		assert(0);

	std::string strLogPath = CStringUtil::WideToAnsi(LogPath) + "\\";

	CLog::Init(strLogPath, CStringUtil::WideToAnsi(APP_NAME), "Updater");

#ifdef _DEBUG
	CLog::SetLogLevel(LOG_LEVEL_DEBUG);
#else
	CLog::SetLogLevel(LOG_LEVEL_NORMAL);
#endif

	CLog::Log(LOGINFO, "Startup %s", UPDATER_VERSION);

	//Set update check info by register info

	win_sparkle_set_appcast_url(FEED_URL);
	win_sparkle_set_app_details(COMPANY_NAME, APP_NAME, L"");
	win_sparkle_set_automatic_check_for_updates(1);
	const int ONE_HOUR = 60 * 60;
	win_sparkle_set_update_check_interval(ONE_HOUR);

	Settings::WriteConfigValue("LastCheckTime", "0");

	//---If there is an installer waiting for lunch, then lunch it. 
	// To lunch installer when os start
	// Luncher will auto run SMP

	std::wstring PatchPath;
	Settings::ReadConfigValue(REGISTER_PATCH_PATH, PatchPath);

	if (PatchPath != L"")//means there is a Patch need be lunch
	{
		if (IsSmpRunning())
			TerminateSmp();

		LaunchPatch(PatchPath);
		CLog::Log(LOGINFO, "Launch patch when startup");
	}

	//***


	win_sparkle_init();

	while (true)
	{
		//---Wait SMP's connect

		
		//---Set update check info by the info that SMP sent

		//win_sparkle_cleanup();
		//win_sparkle_set_appcast_url(feedUrl);
		//win_sparkle_init();

		Sleep(1000);
	}

	return 0;
}