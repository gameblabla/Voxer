/////////////////////////////////////////////////////////////////////////////////////////////
// Name: CHighscores.cpp
// Desc.: Ekran z najwy¿szymi wynikami
// Author: Gerard Wypych
/////////////////////////////////////////////////////////////////////////////////////////////

#include "CHighscores.h"

#include "CCursor.h"
#include "CMesh.h"
#include "CButton.h"

char home_highscores[128];

CHighscores::CHighscores()
{
	background = NULL;
	close = NULL;

	// Resetowanie listy punktów i nazw graczy
	for( unsigned int i = 0; i < 10; ++i )
	{
		score_txt[i] = -1;
		name_txt[i].clear();
	}
}

CHighscores::~CHighscores()
{
	if( background )
		delete background;
	if( close )
		delete close;
}

bool CHighscores::init()
{
#ifdef HOME_SUPPORT
	char home_config[128];
	FILE* file_ptr;
	
	snprintf(home_config, sizeof(home_highscores), "%s/.config", getenv("HOME"));
	snprintf(home_highscores, sizeof(home_highscores), "%s/.config/voxer/", getenv("HOME"));

	if(access( home_config, F_OK ) == -1)  mkdir(home_config, 0755);
	if(access( home_highscores, F_OK ) == -1)  mkdir(home_highscores, 0755);
	
	snprintf(home_highscores, sizeof(home_highscores), "%s/.config/voxer/highscores.txt", getenv("HOME"));
	
	// File doesn't exist, create it
	if (access(home_highscores, F_OK) != 0) {
		file_ptr = fopen(home_highscores, "w");
		fclose(file_ptr);
	}

	
#else
	snprintf(home_highscores, sizeof(home_highscores), "Highscores/highscores.txt");
#endif
	
	// Wczytywanie t³a
	background = new CMesh();
	if( !background )
		return false;
	if( !background->Load( "Highscores/Graphic/background.obj","Highscores/Graphic/background.png" ) )
		return false;

	// Wczytywanie przycisku close
	close = new CButton();
	if( !close )
		return false;
	if( !close->Load( "Highscores/Graphic/close.obj", "Highscores/Graphic/close1.png" ) )
		return false;
	if( !close->LoadTex2( "Highscores/Graphic/close2.png" ) )
		return false;
	close->x = -1.975;
	close->y = 1.347;
	close->z = 0.001;
	close->width = 0.306;
	close->height = 0.067;

	// Odœwie¿enie listy wyników
	refresh();

	return true;
}

void CHighscores::draw()
{
	background->Draw(); // Rysowanie t³a
	close->Draw(); // Rysowanie przycisku

	// Kolor tekstu
	SDL_Color text_color;
	text_color.a = 0; text_color.r = 0; text_color.g = 65; text_color.b = 145;
	
	// Wyœwietlanie wyników
	for( unsigned int i = 0; i < 10; ++i )
	{
		if( score_txt[i] == -1 ) // Jeœli brak wyniku to zakoñcz wyœwietlanie
			break;

		// Wyœwietlanie nazw graczy
		convert.str("");
		convert << "       " << name_txt[i] << "       ";
		name[i].RenderText( convert.str().c_str(), text_color, F_Center, 0.12, 1.2, -0.5, 0.878 - (i * 0.25), 0.0 );

		// Wyœwietlanie liczby punktów
		convert.str("");
		convert << "       " << score_txt[i] << "       ";
		score[i].RenderText( convert.str().c_str(), text_color, F_Center, 0.12, 1.2, 1.244, 0.878  - (i * 0.25), 0.0 );
	}
}

bool CHighscores::logic( CCursor *cursor )
{
	if( close->Logic( cursor ) ) // Obs³uga przycisku
		return true;

	return false;
}

bool CHighscores::refresh()
{
	// Resetowanie listy puntków i nazw graczy
	for( unsigned int i = 0; i < 10; ++i )
	{
		score_txt[i] = -1;
		name_txt[i].clear();
	}
	
	std::ifstream file( home_highscores ); // Otwieranie pliku
	if( !file )
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, 
			"Data loading error...", home_highscores, NULL);
		return false;
	}

	// Zmienne tymczasowe
	std::string name_tmp;
	int score_tmp = -1;
	bool file_damaged = false; // Flaga uszkodzenia pliku

	for( int i = 0; i < 10; ++i )
	{
		while( !file.eof() ) // Do koñca pliku
		{
			// Wczytywanie wyniku
			file >> name_tmp;
			file >> score_tmp;

			if( file.fail() ) // Jeœli b³¹d wczytywania
			{
				if( !file.eof() ) // I brak koñca pliku
					file_damaged = true; // Plik uszkodzony
				file.clear(); // Czyszczenie flag pliku
				// Przeskoczenie do nastêpnej linijki pliku
				file.ignore( std::numeric_limits < std::streamsize >::max(), '\n' ); 
				continue;
			}

			// Sortowanie wyników
			if( i-1 >= 0 && score_tmp > score_txt[i-1] )
				continue;
			if( i-1 >= 0 && score_tmp == score_txt[i-1] )
			{
				for( unsigned int j = 0; j < 10; ++j )
				{
					if( score_txt[j] == score_tmp && name_txt[j] == name_tmp )
						goto przerwanie;
				}
				name_txt[i] = name_tmp;
				score_txt[i] = score_tmp;
				
				przerwanie:
				continue;
			}
			if( score_txt[i] < score_tmp )
			{
				name_txt[i] = name_tmp;
				score_txt[i] = score_tmp;
			}
		}
		
		file.clear(); // Czyszczenie flag pliku
		file.seekg(0); // Skok na pocz¹tek pliku
	}

	if(file_damaged)
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, 
			"Data loading error... File damaged", home_highscores, NULL);

	file.close();

	return true;
}
