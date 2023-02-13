/////////////////////////////////////////////////////////////////////////////////////////////
// Name: CApp.cpp
// Desc.: Zawiera podstawow¹ klasê aplikacji
// Author: Gerard Wypych
/////////////////////////////////////////////////////////////////////////////////////////////

#include "CApp.h"
#include "AppSettings.h"

#include "CWindow.h"
#include "CLogic.h"
#include "CCamera.h"
#include "CGraphic.h"

#include "CGame.h"

#include "CFont.h"

CApp::CApp()
{
	// Zerowanie wskaŸników
	graphic	= NULL;
	logic	= NULL;
	window	= NULL;
	camera	= NULL;

	game	= NULL;
}

CApp::~CApp()
{
	// Czyszczenie pamiêci, usuwanie obiektów jeœli istniej¹
	if( graphic )
		delete graphic;
	if( logic )
		delete logic;
	if( window )
		delete window;
	if( camera )
		delete camera;

	if( game )
		delete game;

	// Wy³¹czenie miksera dŸwiêków, zatrzymywanie odtwarzanych dŸwiêków
	Mix_CloseAudio(); 

	// Usuwanie czcionki
	DestroyFont();
}

bool CApp::init( AppSettings settings )
{
	// Kopiowanie ustawieñ
	this->settings = settings;

	// Inicjalizowanie biblioteki SDL
	if( SDL_Init( SDL_INIT_EVERYTHING ) < 0 )
	{
		/*SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, 
			"Initializing Error (SDL_Init)", SDL_GetError(), NULL);*/
		SDL_Log("Initializing Error (SDL_Init)\n");
		return false;
	}

	// W³¹czanie miksera dŸwiêków
	if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 1024 ) == -1 )
	{
		SDL_Log("Initializing Error","Mix_OpenAudio failed\n");
		return false;
	}

	// Tworzenie obiektu odpowiedzialnego za czêœæ graficzn¹
	graphic = new CGraphic(this);
	if( !graphic ) return false;

	// Tworzenie obiektu odpowiedzialnego za czêœæ logiczn¹
	logic = new CLogic(this);
	if( !logic ) return false;
	
	// Tworzenie obiektu odpowiadaj¹cego za obs³ugê okna
	window = new CWindow;
	if( !window ) return false;

	// Tworzenie okna
	if( !window->create( settings ) )
	{
		SDL_Log("Initializing Error (window->create)\n");
		return false;
	}

	// Konfigurowanie ustawieñ graficznych
	graphic->init( settings );

	// Wczytywanie czcionki 
	if( !InitFont( "System/calibrib.ttf", 48 ) )
		return false;

	// Tworzenie obiektu kamery
	camera = new CCamera;
	if( !camera ) return false;
	
	// Ustawianie pozycji kamery
	camera->SetPos( 0.0, 0.0, -2.1445 );

	// Tworzenie obiektu gry w³asciwej
	game = new CGame();
	if( !game ) return false;
	if( !game->init( this ) )
	{
		SDL_Log("Initializing Error (game->init)\n");
		return false;
	}

	// Ustawianie trybu relatywnego myszki (przechwytywanie ruchów myszki)
	if( SDL_SetRelativeMouseMode( SDL_TRUE ) != 0 )
	{
		SDL_Log("Cannot set Relative Mouse Mode\n");
		return false;
	}
	
	#ifdef NO_TEXT_INPUT
	// Rozpoczêcie wprowadzania tekstu
	SDL_StartTextInput();
	#endif
	
	return true;
}

void CApp::run()
{
	// Sprawdzanie zdarzeñ, 
	// W razie proœby o zamkniêcie programu, koñczy pêtle
	while( !logic->check_events() )
	{
		logic->DT_Begin(); // Rozpoczêcie pomiaru up³ywu czasu

		// Obs³uga czêœci logicznej gry
		// W razie proœby o zamkniêcie programu, koñczy pêtle
		if( game->logic() ) break;

		// Rysowanie obrazu w buforze
		graphic->draw();

		// Przerzucenie obrazu z bufora na ekran
		SDL_GL_SwapWindow( window->window );

		logic->DT_End(); // Zakoñczenie pomiaru up³ywu czasu
	}
}
