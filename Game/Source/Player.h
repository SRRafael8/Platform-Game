#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"

struct SDL_Texture;

class Player : public Entity
{
public:

	Player();
	
	virtual ~Player();

	bool Awake();

	bool Start();

	bool Update();

	bool CleanUp();

	// L07 TODO 6: Define OnCollision function for the player. Check the virtual function on Entity class

public:

private:

	//L02: DONE 1: Declare player parameters
	SDL_Texture* texture;
	const char* texturePath;

	//Audio fx
	int pickCoinFxId;

	// L07 TODO 5: Add physics to the player - declare a Physics body


};

#endif // __PLAYER_H__