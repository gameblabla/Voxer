/////////////////////////////////////////////////////////////////////////////////////////////
// Name: CLogic.cpp
// Desc.: Zawiera klasê odpowiedzialn¹ za czêœæ logiczn¹
// Author: Gerard Wypych
/////////////////////////////////////////////////////////////////////////////////////////////

#include "CLogic.h"
#include "CApp.h"

#include "CCamera.h"
#include "CWindow.h"
#include "CGame.h"

#include "CIngame.h"
#include "CTurret.h"
#include "CGameover.h"

void CLogic::DT_Begin()
{
	// Pobieranie czasu dzia³ania programu
	DT_Start = SDL_GetTicks();
}

void CLogic::DT_End()
{
	// Obliczanie up³ywu czasu
	DeltaTime = SDL_GetTicks() - DT_Start;
	
	// Liczenie klatek na sekundê z aktualizacj¹ co 1 sekundê
	++FPS_Buffer; FPS_ResetTime -= DeltaTime;
	if( FPS_ResetTime <= 0 )
	{
		FPS = FPS_Buffer;
		FPS_Buffer = 0;
		FPS_ResetTime = 1000;
	}
}

const int CLogic::DT_Return()
{
	return DeltaTime;
}

bool CLogic::check_events()
{	
	// Dopóki s¹ zdarzenia do odebrania
	while( SDL_PollEvent( &ev ) )
	{
		if( ev.type == SDL_QUIT )
		{
			return true;	// Zamykanie programu
		}

		#ifdef NO_TEXT_INPUT
		// Wprowadzanie tekstu
		if( ev.type == SDL_TEXTINPUT )
		{
			// Jeœli status gry to GAMEOVER i pierwsza litera tekstu to nie spacja
			if( app->game->game_status == GAMEOVER && ev.text.text[0] != ' ' )
			{
				// Rozszerz nazwê gracza o dodatkow¹ literê, max. 16 znaków
				app->game->ingame->gameover->name += ev.text.text[0];
				if( app->game->ingame->gameover->name.size() > 16 )
					app->game->ingame->gameover->name.resize(16);
			}
		}
		#endif

		// Wciœniêcie przycisku
		if( ev.type == SDL_KEYDOWN )
		{
			switch( ev.key.keysym.sym )
			{
				// Jeœli wciœniêtu klawisz ESCAPE:
				// Cofniêcie do poprzedniego ekranu gry
				// Lub wyjœcie z programu
				case SDLK_ESCAPE:
					if( app->game->game_status ==  MENU )
						app->game->game_status = EXIT;
					else
					if( app->game->game_status == INGAME )
					{
						app->game->game_status = MENU;
						Mix_Pause( 0 );
					}
					else
					if( app->game->game_status == SHOP )
						app->game->game_status = INGAME;
					else
					if( app->game->game_status == HIGHSCORES )
						app->game->game_status = MENU;
					else
					if( app->game->game_status == AUTHOR )
						app->game->game_status = MENU;
					break;

				case SDLK_BACKSPACE:
					#ifdef NO_TEXT_INPUT
					// Jeœli status gry to GAMEOVER i nazwa gracza ma wiêcej ni¿ 0 liter to usuñ 1 literê
					if( app->game->game_status == GAMEOVER && app->game->ingame->gameover->name.size() > 0)
					{
						app->game->ingame->gameover->name.resize(16);
						app->game->ingame->gameover->name = "DEFAULT";
					}
					#else
					// Jeœli status gry to GAMEOVER i nazwa gracza ma wiêcej ni¿ 0 liter to usuñ 1 literê
					if( app->game->game_status == GAMEOVER && app->game->ingame->gameover->name.size() > 0)
						app->game->ingame->gameover->name.pop_back();
					#endif
					break;

				case SDLK_p:
					if( debug_mode ) // Jeœli w³¹czony tryb debugowania
						mouse_release = !mouse_release; // W³¹czenie/wy³¹czenie sterowania kamer¹ za pomoc¹ myszki
					break;

				case SDLK_o:
					if( debug_mode ) // Jeœli w³¹czony tryb debugowania
					{
						mouse_center = !mouse_center; // W³¹czenie/wy³¹czenie p³ynnego sterowania kamer¹ za pomoc¹ myszki
						SDL_SetRelativeMouseMode( static_cast<SDL_bool>(mouse_center) ); // W³¹czenie/wy³¹czenie trybu relatywnego
					}
					break;

				case SDLK_F1:
					debug_mode = !debug_mode; break; // W³¹czenie/wy³¹czenie trybu debugowania (wolnej kamery)

				case SDLK_F2:
					draw_FPS = !draw_FPS; break; // W³¹czenie/wy³¹czenie rysowania iloœci klatek na sekundê

				case SDLK_F3:
					SDL_GL_SetSwapInterval( !SDL_GL_GetSwapInterval() ); break; // W³¹czenie wy³¹czenie synchronizacji pionowej
			}
		}
	}
	
	if( debug_mode )
		CameraControl(); // Sterowanie kamer¹

	return false;
}

void CLogic::CameraControl( void )
{
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);
	// Ustawianie prêdkoœci kamery w zale¿oœci od up³ywu czasu
	app->camera->SetSpeed(4.0 * DeltaTime/1000);

	// Poruszanie kamer¹ za pomoc¹ klawiatury
	if( keystate[SDL_SCANCODE_W] )
		app->camera->MoveForward();
	if( keystate[SDL_SCANCODE_A] )
		app->camera->MoveLeft();
	if( keystate[SDL_SCANCODE_S] )
		app->camera->MoveBackward();
	if( keystate[SDL_SCANCODE_D] )
		app->camera->MoveRight();
	if( keystate[SDL_SCANCODE_E] )
		app->camera->MoveUp();
	if( keystate[SDL_SCANCODE_Q] )
		app->camera->MoveDown();

	// Obracanie kamer¹ za pomoc¹ klawiatury
	if(	keystate[SDL_SCANCODE_RIGHT] )
		app->camera->Rotate( 0.0, 45.0 * DeltaTime/1000, 0.0 );

	if(	keystate[SDL_SCANCODE_LEFT] )
		app->camera->Rotate( 0.0, -45.0 * DeltaTime/1000, 0.0 );	

	if(	keystate[SDL_SCANCODE_UP] )
		app->camera->Rotate(45.0 * DeltaTime/1000, 0.0, 0.0);

	if(	keystate[SDL_SCANCODE_DOWN] )
		app->camera->Rotate(-45.0 * DeltaTime/1000, 0.0, 0.0);

	if(	keystate[SDL_SCANCODE_J] )
		app->camera->Rotate(0.0, 0.0, -45.0 * DeltaTime/1000);

	if(	keystate[SDL_SCANCODE_K] )
		app->camera->Rotate(0.0, 0.0, 45.0 * DeltaTime/1000);

	if( keystate[SDL_SCANCODE_SPACE] )
	{
		app->camera->SetRotate( 0.0, 0.0, 0.0 );
		app->camera->SetPos( 0.0, 0.0, -2.1445 );
	}

	// Obracanie kamer¹ za pomoc¹ myszki
	if(!mouse_release)
	{
		int mPosX, mPosY;

		if(mouse_center)
		{
			SDL_GetRelativeMouseState( &mPosX, &mPosY ); // Pobieranie przemieszczenia kursora myszki
			app->camera->Rotate(0.2 * mPosY, 0.2 * mPosX, 0.0); // Ustawianie obrotu
		}
		else
		{
			SDL_GetMouseState( &mPosX, &mPosY ); // Pobieranie wspó³rzêdnych kursora myszki
			const int CenterX = app->window->src_width/2;
			const int CenterY = app->window->src_height/2;
			app->camera->Rotate( 0.4 * DeltaTime/1000 * (mPosY-CenterY), 0.4 * DeltaTime/1000 * (mPosX-CenterX), 0.0); // Ustawianie obrotu
		}
	}
}
