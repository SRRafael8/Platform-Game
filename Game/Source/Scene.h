#ifndef __SCENE_H__
#define __SCENE_H__

#include "Module.h"
#include "Player.h"
#include "Item.h"
#include "Enemy.h"

struct SDL_Texture;

class Scene : public Module
{
public:

	Scene();

	// Destructor
	virtual ~Scene();

	// Called before render is available
	bool Awake(pugi::xml_node& config);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();


public:

	//L02: DONE 3: Declare a Player attribute 
	Player* player;
	Enemy* enemy;

private:
	SDL_Texture* img;
	SDL_Texture* texturescene;
	SDL_Texture* texturescene2;
	SDL_Texture* texturescreen3;
	SDL_Texture* texturescreen4;
	SDL_Texture* mouseTileTex = nullptr;
	SDL_Texture* originTex = nullptr;
	const char* textureintro;
	const char* texturelobby;
	const char* texturedeath;
	const char* texturewin;
	bool introactiva = true;
	bool introactiva2 = true;
	int timer = 200;

	iPoint origin;
	bool originSelected = false;


};

#endif // __SCENE_H__