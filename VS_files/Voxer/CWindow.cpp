/////////////////////////////////////////////////////////////////////////////////////////////
// Name: CWindow.cpp
// Desc.: Zawiera klasê okna
// Author: Gerard Wypych
/////////////////////////////////////////////////////////////////////////////////////////////

#include "CWindow.h"
#include "AppSettings.h"

static SDL_Renderer *renderer_game;

// Konstruktor klasy CWindow
CWindow::CWindow()
{
	// Zerowanie wskaŸników
	window = NULL;
	context = NULL;
}

CWindow::~CWindow()
{ 
	// Niszczenie kontekstu OpenGL
	if( context )
		SDL_GL_DeleteContext( context );

	// Niszczenie okna
	if( window )
		SDL_DestroyWindow( window );
		
	if (renderer_game)
		SDL_DestroyRenderer (renderer_game);
}

bool CWindow::create( AppSettings &settings )
{
	// Niszczenie kontekstu OpenGL
	if( context )
		SDL_GL_DeleteContext( context );

	// Niszczenie okna
	if( window )
		SDL_DestroyWindow( window );

	SDL_DisplayMode mode;	// Tryb ekranu (do pobrania aktualnej rozdzielczoœci)
	SDL_GetCurrentDisplayMode( 0, &mode ); // Pozyskiwanie aktualnego trybu ekranu

	// Dla rozdzielczoœci 0 ustawienia rozdzielczoœæi maksymalnej/optymalnej
	if( settings.src_width == 0 || settings.src_height == 0 )
	{
		if( settings.fullscreen ) // Rozdzielczoœæ maksymalna dla trybu pe³noekranowego
		{
			settings.src_width = mode.w;
			settings.src_height = mode.h;
		}	
		else // Rozdzielczoœæ optymalna dla trybu niepe³noerkanowego
		{
			settings.src_height = mode.h - (mode.h / 4);
			settings.src_width =  4/3 * mode.h;			
		}
	}

	// Tworzenie okna w trybie pe³noekranowym lub okienowym
	if( settings.fullscreen )
	{
		window = SDL_CreateWindow(settings.title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			settings.src_width, settings.src_height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN );
	}
	else
	{
		window = SDL_CreateWindow(settings.title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			settings.src_width, settings.src_height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );
	}
	
	// All of this junk is required on some platforms like the GCW0/RG350
	// Otherwise black screen
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, 0);
	SDL_RenderSetLogicalSize(renderer_game, settings.src_width, settings.src_height);
	
	renderer_game = SDL_CreateRenderer(window, -1,
#ifdef NOVSYNC
	0
#else
	SDL_RENDERER_PRESENTVSYNC
#endif
	);
	
	// Make sure background is black
	SDL_SetRenderDrawColor(renderer_game, 0, 0, 0, 255);
	SDL_RenderClear(renderer_game);
	SDL_RenderPresent(renderer_game);
	
	
	// Ustawianie g³êbi kolorów
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );

	// W³¹czenie podwójnego buforowania
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    // Ustawienie wielkoœci bufora g³êbi
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	// Ustawienie multisamplingu
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, settings.MultiSampleSamples);
	
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);

	if(!window)
	{
		SDL_Log("Can't create SDL Window\n");
		return false;
	}
	
	// Tworzenie kontekstu OpenGL dla okna
    context = SDL_GL_CreateContext( window );
    if( !context )
    {
		SDL_Log("Can't create OpenGL context\n");
		return false;
	}
	
	// Ustawienie synchronizacji pionowej
	SDL_GL_SetSwapInterval(1);

	// Zapisanie wymiarów okna
	this->src_width = settings.src_width;
	this->src_height = settings.src_height;

	return true;
}
