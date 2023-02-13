/////////////////////////////////////////////////////////////////////////////////////////////
// Name: CIngame.cpp
// Desc.: Klasa zawieraj¹ca rozgrywkê
// Author: Gerard Wypych
/////////////////////////////////////////////////////////////////////////////////////////////

#include "CIngame.h"
#include "CCursor.h"
#include "CMesh.h"
#include "CMeteor.h"
#include "CTurret.h"
#include "CBullet.h"
#include "CLevel.h"
#include "CBonus.h"
#include "CShop.h"
#include "CGameover.h"
#include "CApp.h"
#include "CGame.h"

#define PI 3.14159265359

CIngame::CIngame()
{
	// Zerowanie wskaŸników
	background = NULL;
	turret = NULL;
	meteor_mesh = NULL;
	BulletMesh = NULL;
	lvl = NULL;
	shop = NULL;
	shield = NULL;
	gameover = NULL;

	// Resetowanie timerów tworzenia obiektów
	m_time = 0;
	dm_time = m_time;
	
	// Zerowanie wskaŸników dŸwiêków
	hit = NULL;
	coin_gain = NULL;

	// Ustawianie domyœlnej szybkoœci pocisku
	BulletSpeed = 2.0;
	BulletSpeedRatio = 1.0;

	convert.str("");
	scores = 0; // Resetowanie punktów
	level = 1; // Pierwszy level

	shields = 4; // 4 tarcze

	money = 0; // Resetowanie pieniêdzy
}

CIngame::~CIngame()
{
	if( background )
		delete background;
	if( turret )
		delete turret;
	if( meteor_mesh )
		delete meteor_mesh;
	if( BulletMesh )
		delete BulletMesh;
	if( lvl )
		delete lvl;
	if( moneybonus_mesh )
		delete moneybonus_mesh;
	if( shop )
		delete shop;
	if( gameover )
		delete gameover;
	if( shield )
		delete shield;

	if( hit )
		Mix_FreeChunk( hit );
	if( coin_gain )
		Mix_FreeChunk( coin_gain );
}

bool CIngame::init( CApp *App)
{
	// Wczytywanie t³a
	background = new CMesh();
	if( !background ) return false;
	if( !background->Load( "Ingame/Graphic/background.obj", 
		"Ingame/Graphic/background.png" ) )
		return false;
	
	// Inicjalizowanie dzia³ka
	turret = new CTurret();
	if( !turret ) return false;
	if( !turret->init() ) return false;

	// Wczytywanie modelu meteoroidu
	meteor_mesh = new CMesh();
	if( !meteor_mesh ) return false;
	if( !meteor_mesh->Load( "Ingame/Graphic/meteor.obj",
		"Ingame/Graphic/meteor.png" ) )
		return false;

	// Wczytywanie modelu pocisku
	BulletMesh = new CMesh();
	if( !BulletMesh ) return false;
	if( !BulletMesh->Load( "Ingame/Graphic/bullet.obj", "Ingame/Graphic/bullet.png" ) )
		return false;
	BulletMesh->z = 0.0975;

	// Wczytywanie modelu bonusu pieniê¿nego
	moneybonus_mesh = new CMesh();
	if( !moneybonus_mesh ) return false;
	if( !moneybonus_mesh->Load( "Ingame/Graphic/MoneyBonus.obj", "Ingame/Graphic/MoneyBonus.png" ) )
		return false;

	// Wczytywanie dŸwiêku trafienia
	hit = Mix_LoadWAV( "Ingame/Sounds/hit.wav" );
	if( !hit )
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, 
			"Initializing Error","Cannot load hit.wav", NULL);
		return false;
	}

	// Wczytywanie dŸwiêku trafienia monetki
	coin_gain = Mix_LoadWAV( "Ingame/Sounds/coingain.wav" );
	if( !coin_gain )
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, 
			"Initializing Error","Cannot load coin_gain.wav", NULL);
		return false;
	}

	// Tworzenie obiektu level'a
	lvl = new CLevel();
	if( !lvl )
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, 
			"Initializing Error","Cannot create lvl object.", NULL);
		return false;
	}

	// Inicjalizacja sklepu
	shop = new CShop(); if( !shop ) return false;
	if( !shop->init( this ) )
		return false;

	// Inicjalizacja ekranu koñca gry
	gameover = new CGameover(); if( !gameover ) return false;
	if( !gameover->init( this ) )
		return false;

	// Wczytywanie modelu tarczy
	shield = new CMesh(); if( !shield ) return false;
	if( !shield->Load( "Ingame/Graphic/shield.obj",
		"Ingame/Graphic/shield.png" ) )
		return false;
	shield->x = 0.761;
	shield->y = -1.48;
	app = App;

	return true;
}

void CIngame::logic( CCursor *cursor, int DeltaTime )
{
	// Wczytywanie nowego level'a, jeœli numer levela jest wiêkszy ni¿ obecny level i status gry ró¿ny od SHOP i GAMEOVER
	if( lvl->level_number != level && app->game->game_status != SHOP && app->game->game_status != GAMEOVER )
	{
		if( level == 0 ) // Jeœli level 0 to zwiêkszyæ level
			++level;
		convert.str("");
		
		// Wczytywanie level'a od 1 do 3 na podstawie obecnego numeru levela
		if( level%3 == 0 )
			convert << "Ingame/Levels/level-" << 3 << ".txt";
		else
			convert << "Ingame/Levels/level-" << level%3 << ".txt";
		lvl->Load( convert.str().c_str() );
		lvl->level_number = level;

		bonuses.clear();
		meteors.clear();

	}

	// Gdy status rózny od SHOP i GAMEOVER
	if( app->game->game_status != SHOP && app->game->game_status != GAMEOVER )
	{
		dm_time += DeltaTime; // Zwiêkszenie iloœci czasu, który up³yn¹³ od chwilu utworzenia ostatniego obieku

		// Tworzenie nowego obiektu
		if( lvl->objects.size() > lvl->object_number && dm_time >= lvl->objects[lvl->object_number].delay  )
		{
				dm_time = 0; // Resetowanie czasu, który up³yn¹³ od chwilu utworzenia ostatniego obieku
				if( lvl->objects[lvl->object_number].type == 0 ) // Jeœli typ 0, utwórz meteoroid
				{
					meteors.resize( meteors.size()+1 );
					meteors[ meteors.size()-1 ].load( meteor_mesh, 
						lvl->objects[lvl->object_number].value + ((level-1)/3), 
						lvl->objects[lvl->object_number].speed,
						lvl->objects[lvl->object_number].size,
						lvl->objects[lvl->object_number].pos_x,
						lvl->objects[lvl->object_number].pos_xk);
				}
				if( lvl->objects[lvl->object_number].type == 1 ) // Jeœli typ 1, utwórz bonus pieniê¿ny
				{
					bonuses.resize( bonuses.size()+1 );
					bonuses[ bonuses.size()-1 ].load( moneybonus_mesh,
						lvl->objects[lvl->object_number].type,
						lvl->objects[lvl->object_number].value, 
						lvl->objects[lvl->object_number].speed,
						lvl->objects[lvl->object_number].size,
						lvl->objects[lvl->object_number].pos_x,
						lvl->objects[lvl->object_number].pos_xk);
				}
				++lvl->object_number; // Zwiêkszenie numeru obiektu
		}

		// Obs³uga logiczna meteoroidów
		for( unsigned int i = 0; i < meteors.size(); ++i )
		{
			// Jeœli meteoroid prosi o usuniêcie
			if( meteors[i].logic( DeltaTime ) )
			{
				// Jeœli liczba ¿yæ nie jest równa 0, meteoroid przekroczy³ margines
				if( meteors[i].lives != 0 )
					--shields; // Zmniejszenie liczby tarcz

				meteors.erase( meteors.begin()+i ); // Usuniêcie meteoroidu
				--i;
			}
		}
	
		// Obs³uga logiczna bonusów
		for( unsigned int i = 0; i < bonuses.size(); ++i )
		{
			// Jeœli bonus prosi o usuniêcie
			if( bonuses[i].logic( DeltaTime ) )
			{
				bonuses.erase( bonuses.begin()+i ); // Usuniêcie bonusu
				--i;
			}
		}
		
		// Obs³uga dzia³ka i pobrania jego statusu
		int TurretStatus = turret->logic( cursor, DeltaTime );
	
		if( TurretStatus == 1 ) // Jeœli strza³ z lufy nr 1
		{
			// Tworzenie pocisku z uwzglêdnieniem obrotu dzia³ka
			Bullets.resize(Bullets.size()+1);
			Bullets[Bullets.size()-1].mesh = BulletMesh;
			Bullets[Bullets.size()-1].x = ( (-0.09256 * 0.65) )*cos(-turret->Rotate * PI/180) + ( (-0.51566 * 0.65) - (-0.8731 * 0.65) )*sin(-turret->Rotate * PI/180);// 0.65 - skala tymczasowa
			Bullets[Bullets.size()-1].y = -( ( (-0.09256 * 0.65) )*sin(-turret->Rotate * PI/180) - ( (-0.51566 * 0.65) - (-0.8731 * 0.65) )*cos(-turret->Rotate * PI/180) - (-0.8731 * 0.65)) - 0.554;// 0.65 - skala tymczasowa, - 0.5 tymczasowe
			Bullets[Bullets.size()-1].Rotate = turret->Rotate;
		}
		else if( TurretStatus == 2 ) // Jeœli strza³ z lufy nr 2
		{
			// Tworzenie pocisku z uwzglêdnieniem obrotu dzia³ka
			Bullets.resize(Bullets.size()+1);
			Bullets[Bullets.size()-1].mesh = BulletMesh;
			Bullets[Bullets.size()-1].x = ( 0.09256  * 0.65)*cos(-turret->Rotate * PI/180) + ( (-0.51566 * 0.65) - (-0.8731 * 0.65) )*sin(-turret->Rotate * PI/180);// 0.65 - skala tymczasowa
			Bullets[Bullets.size()-1].y = -( ( 0.09256 * 0.65 )*sin(-turret->Rotate * PI/180) - ( (-0.51566 * 0.65) - (-0.8731 * 0.65) )*cos(-turret->Rotate * PI/180) - (-0.8731 * 0.65)) - 0.554;// 0.65 - skala tymczasowa, - 0.5 tymczasowe
			Bullets[Bullets.size()-1].Rotate = turret->Rotate;
		}
	}
	else
	{
		// Wyrównywanie dzia³ka za pomoc¹ fa³szywego kursora
		double x = cursor->x;
		double y = cursor->y;
		cursor->x = 0.0;
		cursor->y = 0.0;
		bool clicked = cursor->clicked;
		cursor->clicked = false;
		turret->logic( cursor, DeltaTime );
		cursor->x = x;
		cursor->y = y;
		cursor->clicked = clicked;
	}

	// Obs³uga pocisków
	for( unsigned int i = 0; i < Bullets.size(); ++i )
	{
		// Jeœli pocisk wyszed³ poza planszê
		if( Bullets[i].x > 2.0 || Bullets[i].x < -2.0  || Bullets[i].y < -1.5 || Bullets[i].y > 1.5 )
		{
			Bullets.erase(Bullets.begin()+i); // Usuñ pocisk
			--i;
			continue;
		}

		// Obliczanie nowego po³o¿enia pocisku
		Bullets[i].x += BulletSpeed * BulletSpeedRatio * sin( -Bullets[i].Rotate * PI/180) * DeltaTime/1000.0;
		Bullets[i].y += BulletSpeed * BulletSpeedRatio * cos( -Bullets[i].Rotate * PI/180) * DeltaTime/1000.0;

		// Sprawdzanie kolizji z meteoroidami
		for( unsigned int j = 0; j < meteors.size() && i < Bullets.size(); ++j )
		{
			// Jeœli kolizja
			if( sqrt( pow( (meteors[j].x - Bullets[i].x), 2 ) + pow( (meteors[j].y - Bullets[i].y), 2 ) ) <= meteors[j].radius  && meteors[j].lives > 0)
			{
				Mix_PlayChannel(-1,hit,0); // Odtwórz dŸwiêk trafienia
				Bullets.erase(Bullets.begin()+i); // Usuñ pocisk
				--i;
				if( --meteors[j].lives <= 0 ) // Zmniejsz liczbê ¿yæ meteoroidu
				{
					scores += 10; // Zwiêksz liczbê puntków jeœli meteoroid zniszczony
				}
				break;
			}
		}

		// Sprawdzenie kolizji z bonusami
		for( unsigned int j = 0; j < bonuses.size() && i < Bullets.size(); ++j )
		{
			// Jeœli kolizja
			if( sqrt( pow( (bonuses[j].x - Bullets[i].x), 2 ) + pow( (bonuses[j].y - Bullets[i].y), 2 ) ) <= bonuses[j].radius )
			{
				Mix_PlayChannel(-1,coin_gain,0); // Odtwórz dŸwiêk trafienia
			
				Bullets.erase(Bullets.begin()+i); // Usuñ pocisk
				--i;
				if( bonuses[j].type = 1 ) // Jeœli bonus typu 1
					money += bonuses[j].value; // Zwiêksz liczbê pieniêdzy o wartoœc bonusu
				bonuses.erase( bonuses.begin()+j ); // Usuñ bonus
				break;
			}
		}
	}

	// Sprawdzenie, czy level zosta³ ukoñczony
	if( lvl->objects.size() <= lvl->object_number &&  lvl->level_number == level && meteors.size() == 0 && bonuses.size() == 0 )
	{
		dm_time = 0; // Zeruj iloœc czasu, który up³yn¹³ od chwilu utworzenia ostatniego obieku
		++level; // Zwiêksz numer level'a
		app->game->game_status = SHOP; // Ustaw status gry na SHOP (sklep)
	}
	
	// Sprawdzenie przegranej
	if( shields < 0 && app->game->game_status != GAMEOVER )
	{
		dm_time = 0; // Zeruj iloœc czasu, który up³yn¹³ od chwilu utworzenia ostatniego obieku
		app->game->game_status = GAMEOVER; // Zmiana statusu na GAMEOVER (koniec gry)
		gameover->restart(); // Resetowanie gameover'a
		meteors.clear(); // Resetowanie meteoroidów
		bonuses.clear(); // Resetowanie bonusów
		Bullets.clear(); // Resetowanie pocisków
	}
}

void CIngame::draw()
{
	background->Draw(); // Rysowanie t³a
	turret->draw(); // Rysowanie dzia³ka

	// Rysowanie meteoroidów
	for( unsigned int i = 0; i < meteors.size(); ++i )
		meteors[i].draw();

	// Rysowanie pocisków
	for( unsigned int i = 0; i < Bullets.size(); ++i )
		Bullets[i].draw();

	// Rysowanie bonusów
	for( unsigned int i = 0; i < bonuses.size(); ++i )
		bonuses[i].draw();

	// Rysowanie tarcz
	glPushMatrix();
		for( int i = 0; i < shields; ++i )
		{
			shield->Draw();
			glTranslated( 0.125, 0.0, 0.0 );
		}
	glPopMatrix();

	// Kolor tekstu
	SDL_Color text_color;
	text_color.a = 0; text_color.r = 0; text_color.g = 65; text_color.b = 145;

	// Rysowanie napisów

	// Jeœli prêdkoœc prze³adowywania dzia³ka jest wiêksza ni¿ 100
	if( (turret->ReloadSpeedRatio - 1.0) * 100 > 100 )
	{
		// Zabarwiaj kolor tekstu na czerwono
		text_color.r = Uint8(255.0 * (turret->ReloadSpeedRatio - 2.0)/0.5); 
		text_color.g = 65 - Uint8(65.0 * (turret->ReloadSpeedRatio - 2.0)/0.5); 
		text_color.b = 145 - Uint8(145.0 * (turret->ReloadSpeedRatio - 2.0)/0.5);
	}

	convert.str("");
	convert << "       " << (turret->ReloadSpeedRatio - 1.0) * 100 << "       ";
	ReloadSpeed_Text.RenderText( convert.str().c_str(), text_color, F_Center, 0.125, 1.20, -1.685, -1.45, 0.0001 );

	// Resetowanie koloru tekstu
	text_color.a = 0; text_color.r = 0; text_color.g = 65; text_color.b = 145;

	// Scores
	convert.str("");
	convert << "       " << scores << "       ";
	Scores_Text.RenderText( convert.str().c_str(), text_color, F_Center, 0.125, 1.20, 0.0, 1.3, 0.0001 );

	// Rotate Speed
	convert.str("");
	convert << "       " << (turret->RotateSpeedRatio - 1.0) * 100 << "       ";
	TrackingSpeed_Text.RenderText( convert.str().c_str(), text_color, F_Center, 0.125, 1.20, -0.935, -1.45, 0.0001 );

	// Bullet Speed
	convert.str("");
	convert << "       " << (BulletSpeedRatio - 1.0) * 100 << "       ";
	BulletSpeed_Text.RenderText( convert.str().c_str(), text_color, F_Center, 0.125, 1.20, -1.31, -1.45, 0.0001 );

	// Level
	convert.str("");
	convert << "       " << level << "       ";
	Level_Text.RenderText( convert.str().c_str(), text_color, F_Center, 0.125, 1.20, 1.75, -1.45, 0.0001 );

	text_color.a = 0; text_color.r = 255; text_color.g = 190; text_color.b = 0;

	// Money
	convert.str("");
	convert << "      $" << money << "       ";
	Money_Text.RenderText( convert.str().c_str(), text_color, F_Center, 0.125, 1.20, 1.44, -1.45, 0.0001 );
}
