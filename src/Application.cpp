
#include <windows.h>
#include "winsparkle.h"
#include "settings.h"
#include <wx/utils.h>
#include "Globals.h"

using namespace winsparkle;


INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR commandLine, INT)
{

	//Set update check info by register info

	win_sparkle_set_appcast_url(FEED_URL);
	win_sparkle_set_app_details(COMPANY_NAME, APP_NAME, L"");
	win_sparkle_set_automatic_check_for_updates(1);
	const int ONE_HOUR = 60 * 60;
	win_sparkle_set_update_check_interval(ONE_HOUR);


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
	}

	return 0;
}