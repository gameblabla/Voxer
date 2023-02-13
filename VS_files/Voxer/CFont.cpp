/////////////////////////////////////////////////////////////////////////////////////////////
// Name: CFont.cpp
// Desc.: Rysowanie napisów i obs³uga czcionki
// Author: Gerard Wypych
/////////////////////////////////////////////////////////////////////////////////////////////

#include "CFont.h"

TTF_Font* font; // Czcionka

// Wczytywanie czcionki
bool InitFont( char *font_file, int ptsize )
{
	// Inicjalizowanie TTF engine
	if(TTF_Init() == -1)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, 
			"Initializing Error","TTF_Init failed", NULL);
		return false;
	}

	// Otwieranie pliku z czcionk¹
	font = TTF_OpenFont(font_file, ptsize);
	if (font == NULL)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, 
			"Initializing Error","TTF_Init failed", NULL);
		return false;
	}

	return true;
}

// Usuwanie czcionki
void DestroyFont()
{
	if( font )
		TTF_CloseFont( font ); // Zamykanie pliku z czcionk¹
}

CFont::~CFont()
{
	// Usuwanie tekstur napisu
	if( texture )
		glDeleteTextures(1, &texture); 
	if( text_surface )
		SDL_FreeSurface(text_surface);
}

// Wyœwietlanie tekstu [tekst, kolor, wyrównanie, wysokoœci, wspó³czynnik szerokoœci, pozycja x, y, z]
void CFont::RenderText( const char * text, SDL_Color color, FontAlign align, double height, double width_ratio, double x, double y, double z )
{
    if( strcmp( text, current_text.c_str() ) != 0 || text_surface == 0 )
	{
		// Ustawienie aktualnego tesktu
		current_text.assign( text );
		if( text_surface )
			SDL_FreeSurface(text_surface);

		// Rysowanie tekstu na teksturze (SDL)
		text_surface = TTF_RenderText_Blended( font, current_text.c_str(), color );
		if( !text_surface )
			return;
			
		SDL_Surface* formatted_surface = SDL_ConvertSurfaceFormat(text_surface, SDL_PIXELFORMAT_ARGB8888, 0);
		SDL_FreeSurface(text_surface);
		text_surface = formatted_surface;
		
		if( texture )
			glDeleteTextures( 1, &texture );

		// Wczytywanie tekstury do pamiêci karty graficznej (OpenGL)

		glGenTextures( 1, &texture );

		glBindTexture(GL_TEXTURE_2D, texture);
	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		
		glPixelStorei(GL_UNPACK_ROW_LENGTH, text_surface->pitch / text_surface->format->BytesPerPixel);
	
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, text_surface->w, text_surface->h,  0, GL_BGRA, GL_UNSIGNED_BYTE, text_surface->pixels);
	}
	
	// Ustawianie tekstury
	glBindTexture(GL_TEXTURE_2D, texture);

	// Rysowanie napisu w przestrzeni 3D
	glPushMatrix();
		glEnable(GL_TEXTURE_2D);

		glTranslated( x, y, z ); // Ustawienie pozycji
		glScaled( width_ratio, 1.0, 1.0 ); // Skalowanie

		if( align == F_Center ) // Wyrównanie do œrodka
		{
			glBegin( GL_QUADS );
					glTexCoord2d( 0.0, 0.0 ); glVertex3d( text_surface->w / text_surface->h * height / 2.0 *(-1), height/2.0, 0.0 );
					glTexCoord2d( 1.0, 0.0 ); glVertex3d( text_surface->w / text_surface->h * height / 2.0, height/2.0, 0.0 );
					glTexCoord2d( 1.0, 1.0 ); glVertex3d( text_surface->w / text_surface->h * height / 2.0, height/2.0 * (-1), 0.0 );
					glTexCoord2d( 0.0, 1.0 ); glVertex3d( text_surface->w / text_surface->h * height / 2.0 *(-1), height/2.0 * (-1), 0.0 );
			glEnd();
		}
		else
		if( align == F_Left ) // Wyrównanie do lewej
		{
			glBegin( GL_QUADS );
					glTexCoord2d( 0.0, 0.0 ); glVertex3d( 0.0, height, 0.0 );
					glTexCoord2d( 1.0, 0.0 ); glVertex3d( text_surface->w / text_surface->h * height, height, 0.0 );
					glTexCoord2d( 1.0, 1.0 ); glVertex3d( text_surface->w / text_surface->h * height, 0.0, 0.0 );
					glTexCoord2d( 0.0, 1.0 ); glVertex3d( 0.0, 0.0, 0.0 );
			glEnd();
		}
		else
		if( align == F_Right ) // Wyrównanie do prawej
		{
			glBegin( GL_QUADS );
					glTexCoord2d( 0.0, 0.0 ); glVertex3d( -text_surface->w / text_surface->h * height, height, 0.0 );
					glTexCoord2d( 1.0, 0.0 ); glVertex3d( 0.0, height, 0.0 );
					glTexCoord2d( 1.0, 1.0 ); glVertex3d( 0.0, 0.0, 0.0 );
					glTexCoord2d( 0.0, 1.0 ); glVertex3d( -text_surface->w / text_surface->h * height, 0.0, 0.0 );
			glEnd();
		}
		glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}
