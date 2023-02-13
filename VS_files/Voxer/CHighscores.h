/////////////////////////////////////////////////////////////////////////////////////////////
// Name: CHighscores.h
// Desc.: Ekran z najwy¿szymi wynikami
// Author: Gerard Wypych
/////////////////////////////////////////////////////////////////////////////////////////////


#ifndef CHighscores_h
#define CHighscores_h

#include "declarations.h"
#include "CFont.h"

// Deklaracje zapowiadaj¹ce
class CCursor;
class CButton;
class CMesh;

extern char home_highscores[128];

class CHighscores
{
private:
	CMesh *background; // T³o
	CButton *close; // Przycisk close
	std::ostringstream convert; // Zmienna tymczasowa do konwertowania tekstu
	std::string name_txt[10]; // Lista nazw graczy
	int score_txt[10]; // Lista puntków
	CFont name[10]; // Lista napisów nazw graczy
	CFont score[10]; // Lista napisów punktów
public:
	CHighscores();
	~CHighscores();

	bool init(); // Inicjalizacja
	void draw(); // Rysowanie
	bool refresh(); // Odœwie¿enie wyników
	bool logic( CCursor *cursor ); // Czêœæ logiczna
};

#endif
