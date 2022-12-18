#ifndef __ENEMYV_H__
#define __ENEMYV_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"

struct SDL_Texture;

class EnemyV : public Entity
{
public:

	EnemyV();

	virtual ~EnemyV();

	bool Awake();

	bool Start();

	bool Update();

	bool CleanUp();

	// L07 DONE 6: Define OnCollision function for the player. Check the virtual function on Entity class
	void OnCollision(PhysBody* physA, PhysBody* physB);
	static EnemyV* instance;

public:
	Animation* currentAnimation = nullptr;

	Animation idleanim;
	Animation rightwalk;
	Animation leftwalk;
	Animation jumpingdreta;
	Animation jumpingesquerra;
	Animation muertesita;
	Animation atacacion;

	bool grounded = false;

private:

	//L02: DONE 1: Declare player parameters
	SDL_Texture* texture;
	const char* texturePath;
	SDL_Texture* texturescene3;
	SDL_Texture* texturescene4;
	const char* texturedeath;
	const char* texturewin;
	SDL_Texture* originTileTex = nullptr;
	SDL_Texture* originTex = nullptr;


	// L07 DONE 5: Add physics to the player - declare a Physics body
	PhysBody* pbody;

	int pickCoinFxId;
	int winsound;
	int deathsound;
	int jumpsound;
	int runsound;
	int time = 20;
	bool wincondition = false;
	bool losecondition = false;
	bool ultimatelosecondition = false;
	int godmode = 1;
	bool introactiva = true;
	bool introactiva2 = false;
	bool lose = false;
	bool ganar = false;
	bool enemymuerto = false;
	bool playermuerto = false;
	bool originSelected = false;

	int timer = 50;
	int timer2 = 100;
	int speed = 1;
	int speedy = 1;

	float playerpositionx;
	float playerpositiony;

	float yVel = 0;

};

#endif // __PLAYER_H__

