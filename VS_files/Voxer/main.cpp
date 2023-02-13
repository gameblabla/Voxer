/////////////////////////////////////
// __      __                      //
// \ \    / /                      //
//  \ \  / /___ __  __ ___  _ __   //
//   \ \/ // _ \\ \/ // _ \| '__|  //
//    \  /| (_) |>  <|  __/| |     //
//     \/  \___//_/\_\\___||_|     //                       
//                                 //
/////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
// Name: main.cpp
// Desc.: Plik zawieraj¹cy g³ówn¹ funkcjê programu
// Author: Gerard Wypych
/////////////////////////////////////////////////////////////////////////////////////////////

#include "CApp.h"
#include "AppSettings.h"

int main( int argc, char *argv[] )
{
	CApp app;
	AppSettings settings;

	// Zerowanie i konfigurowanie ustawieñ aplikacji
	memset( &settings, 0, sizeof( AppSettings ) );
#ifdef FORCE_FULLSCREEN
	settings.fullscreen = 1;
#else
	settings.fullscreen = 0;
#endif
	settings.src_width  = 0;
	settings.src_height = 0;
	settings.MultiSampleSamples = 4;
	settings.title = "Voxer || version 1.0 beta || Author: Gerard Wypych";

	// Inicjalizacja z zadanymi ustawieniami
	if( !app.init( settings ) )
		return -1;

	// Wykonywanie programu
	app.run();

	return 0;
}
