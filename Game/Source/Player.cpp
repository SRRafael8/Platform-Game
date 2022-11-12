#include "Player.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"

Player::Player() : Entity(EntityType::PLAYER)
{
	name.Create("Player");

	//idle Animation
	for (int i = 0; i < 6; i++) {
		idleanim.PushBack({(i * 56), 0, 56, 56 });
	}
	idleanim.loop = true;
	idleanim.speedx = 0.1f;

	//Walking dreta
	for (int i = 0; i < 8; i++) {
		rightwalk.PushBack({ (i * 56), 57 + 57, 56, 56 });
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
		leftwalk.PushBack({ 900 - (i * 56), 57 + 57, 56, 56 });
	}
	leftwalk.loop = true;
	leftwalk.speedx = 0.1f;

	//saltimbanquis dretÝsssssssima
	for (int i = 7; i > 1; i--) {
		jumpingesquerra.PushBack({ 900 - (i * 56), 57 * 3, 56, 56 });
	}
	for (int i = 7; i > 1; i--) {
		jumpingesquerra.PushBack({ 900 - (i * 56), 57 * 4, 56, 56 });
	}
	jumpingesquerra.loop = true;
	jumpingesquerra.speedx = 0.1f;

	//mort´sssssimo bb
	
	for (int i = 2; i > 0; i--) {
		muertesita.PushBack({ (i * 56), 57 * 6, 56, 56 });
	}
	for (int i = 7; i > 0; i--) {
		muertesita.PushBack({ (i * 56), 57 * 5, 56, 56 });
	}
	muertesita.loop = false;
	muertesita.speedx = 0.1f;


}

Player::~Player() {

}

bool Player::Awake() {

	//L02: DONE 1: Initialize Player parameters
	//pos = position;
	//texturePath = "Assets/Textures/player/idle1.png";
	texturePath = "Assets/Textures/Player0.png";

	//L02: DONE 5: Get Player parameters from XML
	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();
	

	return true;
}

bool Player::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);

	// L07 DONE 5: Add physics to the player - initialize physics body
	pbody = app->physics->CreateCircle(position.x-50, position.y-276, 14, bodyType::DYNAMIC);

	// L07 DONE 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this; 
	
	// L07 DONE 7: Assign collider type
	pbody->ctype = ColliderType::PLAYER;

	//initialize audio effect - !! Path is hardcoded, should be loaded from config.xml
	pickCoinFxId = app->audio->LoadFx("Assets/Audio/Fx/retro-video-game-coin-pickup-38299.ogg");
	runsound = app->audio->LoadFx("Assets/Audio/Fx/grassrunsupershort.wav");
	deathsound = app->audio->LoadFx("Assets/Audio/Fx/Deathsound.wav");
	winsound = app->audio->LoadFx("Assets/Audio/Fx/Winsound.wav");
	jumpsound = app->audio->LoadFx("Assets/Audio/Fx/jumpyjump.wav");

	currentAnimation = &idleanim;

	return true;
}

bool Player::Update()
{

	// L07 DONE 5: Add physics to the player - updated player position using physics

	int speed = 5; 
	
	b2Vec2 vel = b2Vec2(0, -GRAVITY_Y); 

	//L02: DONE 4: modify the position of the player using arrow keys and render the texture
	
	if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT && time>0 && losecondition == false) {
		vel = b2Vec2(0, +2*GRAVITY_Y);
		time--;
		app->audio->PlayFx(jumpsound);
		//Animacion saltar normal
	}
	else if(losecondition == false) {
		b2Vec2(0, -GRAVITY_Y);
		currentAnimation = &idleanim;
	}
		
	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && losecondition == false) {
		vel = b2Vec2(-speed, -GRAVITY_Y);
		
		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT && time > 0) {
			vel = b2Vec2(-speed, +2*GRAVITY_Y);
			app->audio->PlayFx(jumpsound);
			time--;
		}
		else if (position.x < 133 * 23) {
			app->audio->PlayFx(runsound);
		}
		if (position.x > 23 * 5 && position.x < 107 * 23) {
			app->render->camera.x = -position.x + 100;
		}
		//Animacion caminar izquierda
		currentAnimation = &leftwalk;
	}

	if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && losecondition == false) {
		vel = b2Vec2(speed, -GRAVITY_Y);
		
		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT && time > 0) {
			vel = b2Vec2(speed, +2*GRAVITY_Y);
			time--;
			app->audio->PlayFx(jumpsound);
			//Animacion saltar derecha
		}
		else if(position.x < 133 * 23) {
			app->audio->PlayFx(runsound);
		}
		if (position.x > 23 * 5 && position.x < 107 * 23) {
			app->render->camera.x = -position.x +100;
		}
		currentAnimation = &rightwalk;
	}

	if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_UP && losecondition == false) {
		time = 0;
	}
	if(position.x > 133 * 23 && wincondition == false && app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_IDLE && app->input->GetKey(SDL_SCANCODE_A) == KEY_IDLE && app->input->GetKey(SDL_SCANCODE_D) == KEY_IDLE && losecondition == false){
		app->audio->PlayFx(winsound);
		wincondition = true;
	}
	if (losecondition == true) {
		currentAnimation = &muertesita;
	}

	//Set the velocity of the pbody of the player
	pbody->body->SetLinearVelocity(vel);

	//Update player position in pixels
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 15;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 14;
	SDL_Rect rect = currentAnimation->GetCurrentFrame();
	app->render->DrawTexture(texture, position.x - 12, position.y - 28, &rect);
	//app->render->DrawTexture(texture, position.x , position.y);
	currentAnimation->Update();


	return true;
}

bool Player::CleanUp()
{
	return true;
}

// L07 DONE 6: Define OnCollision function for the player. Check the virtual function on Entity class
void Player::OnCollision(PhysBody* physA, PhysBody* physB) {

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
			break;
		case ColliderType::UNKNOWN:
			LOG("Collision UNKNOWN");
			break;
		case ColliderType::WIN:
			LOG("Collision WIN");
			app->audio->PlayFx(winsound);
			//PASAMOS A PANTALLA GANADORA
			break;
		case ColliderType::LOSE:
			LOG("Collision LOSE");
			app->audio->PlayFx(deathsound);
			//PASAMOS A PANTALLA PERDEDORA
			losecondition = true;
			break;
	}
	


}
