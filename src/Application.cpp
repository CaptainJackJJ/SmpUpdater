
#include <windows.h>
#include "winsparkle.h"

const char* feedUrl = "http://winsparkle.org/example/appcast.xml";
const wchar_t *company_name = L"winsparkle.org";
const wchar_t *app_name = L"WinSparkle Example App";
const wchar_t *app_version = L"1.0";

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR commandLine, INT)
{
	//Set update check info by register info

	win_sparkle_set_appcast_url(feedUrl);
	win_sparkle_set_app_details(company_name, app_name, app_version);
	win_sparkle_set_automatic_check_for_updates(1);
	win_sparkle_set_update_check_interval(3600);

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