/////////////////////////////////////////////////////////////////////////////////////////////
// Name: CIngame.h
// Desc.: Klasa zawieraj¹ca rozgrywkê
// Author: Gerard Wypych
/////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CIngame_h
#define CIngame_h

#include <SDL_mixer.h>
#include <vector>
#include <sstream>
#include "CFont.h"

// Deklaracje zapowiadaj¹ce
class CCursor;
class CMesh;
class CMeteor;
class CTurret;
class CBullet;
class CLevel;
class CBonus;
class CShop;
class CGame;
class CApp;
class CGameover;

class CIngame
{
private:
	CLevel *lvl; // Obiekt level'a

	// Napisy interfejsu
	CFont Scores_Text, ReloadSpeed_Text, BulletSpeed_Text, TrackingSpeed_Text, Level_Text, Money_Text;
	std::ostringstream convert; // Zmienna tymczasowa do konwerotwania tekstu

	CMesh *background; // T³o

	Mix_Chunk *hit; // DŸwiêk trafienia w meteoroid
	Mix_Chunk *coin_gain; // DŸwiêk trafienia w bonus pieniê¿ny
	
	std::vector<CBullet> Bullets; // Pociski
	std::vector<CMeteor> meteors; // Meteoroidy
	
	int m_time; // Czas po jakim pojawia siê kolejny obiekt (w milisekundach)
	int dm_time; // Up³yw czasu od chwili pojawienia siê ostatniego obiektu
	CMesh *shield; // Model tarczy
	
public:
	CIngame();
	~CIngame();

	bool init( CApp *App ); CApp *app;
	void draw();
	void logic( CCursor *cursor, int DeltaTime );

	CShop *shop; // Sklep
	CGameover *gameover; // Ekran koñca gry
		
	unsigned int scores; // Liczba punktów
	unsigned int level; // Numer level'a

	double BulletSpeed; // Szybkoœæ bazowa pocisku
	double BulletSpeedRatio; // Mno¿nik szybkoœci pocisku
	int shields; // Liczba tarcz
	unsigned int money; // Iloœæ pieniêdzy

	CMesh *BulletMesh; // Model pocisku
	CMesh *moneybonus_mesh; // Model monety
	CMesh *meteor_mesh; // Model meteoroidu

	std::vector<CBonus> bonuses; // Bonusy
	CTurret *turret; // Wie¿yczka
};

#endif
