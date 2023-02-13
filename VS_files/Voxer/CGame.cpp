/////////////////////////////////////////////////////////////////////////////////////////////
// Name: CGame.cpp
// Desc.: Klasa opisuj¹ca kursor
// Author: Gerard Wypych
/////////////////////////////////////////////////////////////////////////////////////////////

#include "CGame.h"

#include "CMenu.h"
#include "CIngame.h"
#include "CHighscores.h"
#include "CAuthor.h"

#include "CCursor.h"

#include "CLogic.h"
#include "CApp.h"
#include "CWindow.h"

#include "CShop.h"
#include "CGameover.h"
#include "CTurret.h"

CGame::CGame()
{
	menu = NULL;
	ingame = NULL;
	highscores = NULL;
	author = NULL;

	cursor = NULL;

	app = NULL;

	game_status = MENU; // Domyœlny status gry: MENU
	convert.str(" ");
	
	music = NULL;
}

CGame::~CGame()
{
	if( menu )
		delete menu;
	if( ingame )
		delete ingame;
	if( highscores )
		delete highscores;
	if( author )
		delete author;

	if( cursor )
		delete cursor;

	if( music)
		Mix_FreeMusic( music );
}

bool CGame::init( CApp *App )
{
	// Wczytywanie kursora
	cursor = new CCursor();
	if( !cursor ) return false;
	if( !cursor->load( "System/cursor.obj", "System/cursor1.png", "System/cursor2.png" ) )
	{
		SDL_Log("Can't load cursor obj \n");
		return false;
	}
	
	// Inicjalizowanie menu
	menu = new CMenu();
	if( !menu ) return false;
	if( !menu->init( App ) )
	{
		SDL_Log("menu->init error \n");
		return false;
	}

	// Inicjalizowanie ingame
	ingame = new CIngame();
	if( !ingame ) return false;
	if( !ingame->init( App ) )
	{
		SDL_Log("ingame->init error \n");
		return false;
	}
	
	// Inicjalizowanie highscores
	highscores = new CHighscores();
	if( !highscores ) return false;
	if( !highscores->init() ) 	{
		SDL_Log("highscores->init error \n");
		return false;
	}
	
	// Inicjalizowanie author
	author = new CAuthor();
	if( !author ) return false;
	if( !author->init( this ) ) 	{
		SDL_Log("author->init error \n");
		return false;
	}
	
	app = App; // Kopiowanie wskaŸnika do obiektu nadrzêdnego

	// Wczytywanie muzyki
#ifdef OGG_MUSIC
	music = Mix_LoadMUS( "Music/music.ogg" );
#else
	music = Mix_LoadMUS( "Music/Finding the Balance.mp3" );
#endif
	if( !music )
	{
		SDL_Log("Cannot load Music file, missing decoder in SDL_mixer?\n");
		return false;
	}
	Mix_PlayMusic( music, -1 ); // Odtwarzanie muzyki - w nieskoñczonoœæ

	return true;
}

void CGame::draw()
{
	// Rysowanie w zale¿noœci od statusu gry

	switch(game_status)
	{
	case MENU:
		menu->draw(); break;
	case INGAME:
		ingame->draw(); break;
	case SHOP:
		ingame->draw(); 
		ingame->shop->draw(); break;
	case GAMEOVER:
		ingame->draw(); 
		ingame->gameover->draw(); break;
	case HIGHSCORES:
		highscores->draw(); break;
	case AUTHOR:
		author->draw(); break;
	}

	// Jeœli rysowanie FPS w³¹czone
	if( app->logic->draw_FPS )
	{
		// Kolor tekstu
		SDL_Color text_color = {0, 0, 0, 0};
		
		// Rysowanie tekstu - Liczba FPS
		convert.str("");
		convert << "FPS: " << app->logic->FPS;
		FPS_text.RenderText( convert.str().c_str(), text_color, F_Left, 0.1, 1.0, -2.0, 1.4, 0.001);
	}

	cursor->draw( &game_status ); // Rysowanie kursora
}

bool CGame::logic()
{
	cursor->logic( app ); // Obs³uga kursora

	if( game_status == EXIT )
		return true; // Proœba o zamkniêcie programu

	// Obs³uga czêœci logicznej w zaleznoœci od statusu gry

	switch(game_status)
	{
	case MENU:
		game_status = menu->logic( cursor ); break;
	case INGAME:
		ingame->logic( cursor, app->logic->DeltaTime ); break;
	case SHOP:
		ingame->logic( cursor, app->logic->DeltaTime );
		if( ingame->shop->logic( cursor ) )
			game_status = INGAME;
		break;
	case GAMEOVER:
		ingame->logic( cursor, app->logic->DeltaTime );
		if( ingame->gameover->logic( cursor ) )
		{
			// Ustawianie domyœlnych wartoœci rozgrywki
			game_status = INGAME;
			ingame->scores = 0;
			ingame->level = 0;
			ingame->turret->ReloadSpeedRatio = 1.0;
			ingame->turret->RotateSpeedRatio = 1.0;
			ingame->BulletSpeedRatio = 1.0;
			ingame->shields = 4;
			ingame->money = 0;
		}
		break;
	case HIGHSCORES:
		if( highscores->logic( cursor ) )
			game_status = MENU;
		break;
	case AUTHOR:
		if( author->logic( cursor, app->logic->DeltaTime ) )
			game_status = MENU;
		break;
	}
	return false;
}
