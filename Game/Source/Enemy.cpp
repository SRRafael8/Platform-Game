#include "Enemy.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"
#include "Player.h"
#include "Map.h"
#include "Pathfinding.h"

Enemy::Enemy() : Entity(EntityType::ENEMY)
{
	name.Create("Enemy");

	//idle Animation
	for (int i = 0; i < 4; i++) {
		idleanim.PushBack({ 13 + (i * 150), 415, 25, 39 });
	}
	idleanim.loop = true;
	idleanim.speedx = 0.07f;

	//Walking dreta
	for (int i = 0; i < 8; i++) {
		rightwalk.PushBack({ 13 + (i * 150), 0, 25, 39 });
	}
	rightwalk.loop = true;
	rightwalk.speedx = 0.1f;

	//atacacion sin moricion
	for (int i = 0; i < 6; i++) {
		atacacion.PushBack({ (i * 56), 57, 56, 56 });
	}
	atacacion.loop = false;
	atacacion.speedx = 0.1f;

	//Walking esquerra
	for (int i = 7; i > 1; i--) {
		leftwalk.PushBack({ 2327 + 13 - (i * 150), 0, 25, 39 });
	}
	leftwalk.loop = true;
	leftwalk.speedx = 0.1f;

	//mort�sssssimo
	for (int i = 4; i > 0; i--) {
		muertesita.PushBack({ 13 +(i * 151), 131, 25, 39 });
	}
	muertesita.loop = true;
	muertesita.speedx = 0.05f;


}

Enemy::~Enemy() {

}

bool Enemy::Awake() {

	//L02: DONE 1: Initialize Player parameters
	//pos = position;
	//texturePath = "Assets/Textures/player/idle1.png";
	texturePath = "Assets/Textures/Mushroom.png";


	//L02: DONE 5: Get Player parameters from XML
	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	return true;
}

bool Enemy::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);

	// L07 DONE 5: Add physics to the player - initialize physics body
	//pbody = app->physics->CreateCircle(position.x + 80, position.y - 276, 18, bodyType::DYNAMIC);
	pbody = app->physics->CreateRectangle(position.x + 80 + (70 * 23), position.y - 253, 18, 36, bodyType::DYNAMIC);

	// L07 DONE 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;

	// L07 DONE 7: Assign collider type
	pbody->ctype = ColliderType::ENEMY;

	//initialize audio effect - !! Path is hardcoded, should be loaded from config.xml
	pickCoinFxId = app->audio->LoadFx("Assets/Audio/Fx/retro-video-game-coin-pickup-38299.ogg");
	runsound = app->audio->LoadFx("Assets/Audio/Fx/grassrunsupershort.wav");
	deathsound = app->audio->LoadFx("Assets/Audio/Fx/enemydeath.wav");
	winsound = app->audio->LoadFx("Assets/Audio/Fx/Winsound.wav");
	jumpsound = app->audio->LoadFx("Assets/Audio/Fx/jumpyjump.wav");

	currentAnimation = &idleanim;

	originTileTex = app->tex->Load("Assets/Maps/path_square.png");

	originTex = app->tex->Load("Assets/Maps/x_square.png");

	return true;
}

bool Enemy::Update()
{
	playerpositionx = app->scene->player->position.x+10;
	playerpositiony = app->scene->player->position.y;
	app->pathfinding->ClearLastPath();
	iPoint playerTile = iPoint(0, 0);
	playerTile = app->map->WorldToMap(playerpositionx+5,playerpositiony+23);
	iPoint origin = iPoint(app->map->WorldToMap(app->scene->enemy->position.x, app->scene->enemy->position.y+23));
	app->pathfinding->CreatePath(origin, playerTile);

	/*if (originSelected == true)
	{
		app->pathfinding->CreatePath(origin, playerTile);
		originSelected = false;
	}
	else
	{
		origin = playerTile;
		originSelected = true;
		
	}*/
	
	const DynArray<iPoint>* path = app->pathfinding->GetLastPath();
	for (uint i = 0; i < path->Count(); ++i)
	{
		iPoint pos = app->map->MapToWorld(path->At(i)->x, path->At(i)->y);
		app->render->DrawTexture(originTileTex, pos.x, pos.y);
	}

	iPoint originScreen = app->map->MapToWorld(origin.x, origin.y);
	app->render->DrawTexture(originTex, originScreen.x, originScreen.y);

	
	int currentspeed = 0;

	b2Vec2 vel = b2Vec2(0, -GRAVITY_Y);


	//L02: DONE 4: modify the position of the player using arrow keys and render the texture
	if (introactiva == false) {
		//if (enemymuerto == false && grounded == true) {
		//	b2Vec2(0, -GRAVITY_Y);
		//	currentAnimation = &idleanim;
		//}
		const DynArray<iPoint>* pather = app->pathfinding->GetLastPath();
		if (pather->At(1) == nullptr) {	
			currentAnimation = &idleanim;
		}
		else {
			
		/*	if (pather->At(1)->x < app->scene->enemy->position.x +10 && enemymuerto == false) {
				currentspeed = -speed;
				app->audio->PlayFx(runsound);
				currentAnimation = &leftwalk;
			}
			if (pather->At(1)->x >= app->scene->enemy->position.x +10 && enemymuerto == false) {
				currentspeed = speed;
				app->audio->PlayFx(runsound);
				currentAnimation = &rightwalk;
			}*/
			if (app->map->WorldToMap(app->scene->enemy->position.x, app->scene->enemy->position.y + 23).x > app->map->WorldToMap(app->scene->player->position.x, app->scene->player->position.y + 23).x && enemymuerto==false){
				currentspeed = -speed;
				app->audio->PlayFx(runsound);
				currentAnimation = &leftwalk;
				}
			if (app->map->WorldToMap(app->scene->enemy->position.x, app->scene->enemy->position.y + 23).x <= app->map->WorldToMap(app->scene->player->position.x, app->scene->player->position.y + 23).x && enemymuerto == false) {
				currentspeed = speed;
				app->audio->PlayFx(runsound);
				currentAnimation = &rightwalk;
			}
		}

	}
	if (!grounded) {
		yVel -= GRAVITY_Y * 0.02;
	}
	if (timer<=0) {
		
		
		SDL_DestroyTexture(texture);
		pbody->body->SetActive(false);
		enemymuerto = false;
		grounded = true;
	}

	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) {
		godmode * -1;
	}

	if (enemymuerto == true) {
		timer--;
	}
	
	if (playermuerto == true) {
		timer2--;
	}

	if (timer2 <= 0) {
		SDL_DestroyTexture(texture);
	}

	//Set the velocity of the pbody of the player
	vel = b2Vec2(currentspeed, yVel);
	pbody->body->SetLinearVelocity(vel);

	//Update player position in pixels
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 15;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 14;
	SDL_Rect rect = currentAnimation->GetCurrentFrame();
	if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
		introactiva = false;
	}
	if (introactiva == false) {
		app->render->DrawTexture(texture, position.x, position.y - 7, &rect);
	}
	//app->render->DrawTexture(texture, position.x , position.y);
	currentAnimation->Update();

	if (ganar == true) {
		app->render->DrawTexture(texturescene3, 2360, 0);
	}

	if (lose == true) {
		if (position.x < 23 * 5) {
			app->render->DrawTexture(texturescene4, 0, 0);
		}
		if (position.x > 23 * 5 && position.x < 107 * 23) {
			app->render->DrawTexture(texturescene4, position.x - 106, 0);
		}
		if (position.x > 107 * 23) {
			app->render->DrawTexture(texturescene4, 2360, 0);
		}
	}

	/*if (reviveenemy == true) {
		texture = app->tex->Load(texturePath);
		pbody = app->physics->CreateRectangle(position.x + 80 + (70 * 23), position.y - 253, 18, 36, bodyType::DYNAMIC);
		reviveenemy = false;
	}*/

	return true;
}

bool Enemy::CleanUp()
{
	return true;
}


// L07 DONE 6: Define OnCollision function for the player. Check the virtual function on Entity class
void Enemy::OnCollision(PhysBody* physA, PhysBody* physB) {

	// L07 DONE 7: Detect the type of collision

	switch (physB->ctype)
	{
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		app->audio->PlayFx(pickCoinFxId);
		break;
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		time = 20;
		yVel = 3;
		grounded = true;
		currentAnimation->Reset();
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	case ColliderType::ATTACK:
		LOG("Collision ATTACK");
		enemymuerto = true;
		grounded = false;
		currentAnimation = &muertesita;
		app->audio->PlayFx(deathsound);
		break;
	case ColliderType::PLAYER:
		playermuerto = true;
		LOG("Collision PLAYER");
		break;

	case ColliderType::ENEMYV:
		playermuerto = true;
		LOG("Collision PLAYER");
		break;
		
	default: grounded = false;

	}



}
