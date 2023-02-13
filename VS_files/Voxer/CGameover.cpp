/////////////////////////////////////////////////////////////////////////////////////////////
// Name: CGameover.cpp
// Desc.: Ekran po przegranej
// Author: Gerard Wypych
/////////////////////////////////////////////////////////////////////////////////////////////

#include "CGameover.h"
#include "CMesh.h"
#include "CButton.h"
#include "CIngame.h"
#include "CHighscores.h"

CGameover::CGameover()
{
	background = NULL;
	close = NULL;
	ingame = NULL;
	name.clear();
}

CGameover::~CGameover()
{
	if( background )
		delete background;
	if( close )
		delete close;
}

bool CGameover::init( CIngame *Ingame )
{
	// WskaŸnik do obiektu nadrzêdnego
	this->ingame = Ingame;
	
	// Wczytywanie t³a
	background = new CMesh();
	if( !background ) return false;
	if( !background->Load( "Ingame/Graphic/background.obj", "Ingame/Graphic/gameover.png" ) )
		return false;
	background->z = 0.0;

	// Wczytywanie przycisku
	close = new CButton();
	if( !close ) return false;
	if( !close->Load( "Ingame/Graphic/continue.obj", "Ingame/Graphic/continue1.png" ) )
		return false;
	if( !close->LoadTex2( "Ingame/Graphic/continue2.png" ) )
		return false;
	close->x = 0.531;
	close->y = -0.35;
	close->z = 0.0;
	close->height = 0.083;
	close->width = 0.719;
	return true;
}

bool CGameover::logic( CCursor *cursor )
{
	// Obs³uga przycisku, jeœli klikniêty i wielkoœc nazwy gracza ró¿na od 0
	if( close->Logic( cursor ) && name.size() != 0 )
	{
		// Otwórz plik i ustaw kursor na jego koñcu
		std::fstream file( home_highscores, std::ios::out | std::ios::app );
		if( !file )
		{
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, 
				"Data loading error...", home_highscores, NULL);
			return false;
		}

		// Zapisz wynik
		file << name << ' ' << ingame->scores << std::endl;

		// Zamknij plik
		file.close();
		return true;
	}

	return false;
}

void CGameover::draw()
{
	background->Draw(); // Rysowanie t³a

	// Kolor tekstu
	SDL_Color text_color;
	text_color.a = 0; text_color.r = 0; text_color.g = 65; text_color.b = 145;

	// Wyœwietlanie wpisywanej nazwy gracza (ka¿da litera w osobnej kratce)
	for( unsigned int i = 0; i < name.size() && i < 16; ++i )
	{
		convert.str("");
		convert << "                        " << name[i] << "                        ";
		fname[i].RenderText( convert.str().c_str(), text_color, F_Center, 0.12, 1.2, -0.93 + (i * 0.125), 0.05, 0.0 );
	}
	
	close->Draw(); // Rysowanie przycisku
}

void CGameover::restart()
{
	name.clear(); // Resetowanie nazwy gracza
}
