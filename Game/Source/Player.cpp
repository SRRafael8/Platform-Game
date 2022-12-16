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
	for (int i = 5; i > 0; i--) {
		atacacion.PushBack({ (i * 56), 57, 56, 56 });
	}
	atacacion.loop = true;
	atacacion.speedx = 0.1f;

	//Walking esquerra
	for (int i = 7; i > 1; i--) {
		leftwalk.PushBack({ 900 - (i * 56), 57 + 57, 56, 56 });
	}
	leftwalk.loop = true;
	leftwalk.speedx = 0.1f;

	//saltimbanquis dretísssssssima
	for (int i = 7; i > 0; i--) {
		jumpingesquerra.PushBack({(i * 56), 57 * 3, 56, 56 });
	}
	/*jumpingesquerra.PushBack({ (5 * 56), 57 * 4, 56, 56 });
	jumpingesquerra.PushBack({ (6 * 56), 57 * 4, 56, 56 });
	jumpingesquerra.PushBack({ (7 * 56), 57 * 4, 56, 56 });*/
	jumpingesquerra.loop = false;
	jumpingesquerra.speedx = 0.1f;

	//mortïsssssimo
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
	texturedeath= "Assets/Scenes/deathscreen.png";
	texturewin= "Assets/Scenes/winscreen.png";
	

	//L02: DONE 5: Get Player parameters from XML
	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();
	
	return true;
}

bool Player::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);
	texturescene3 = app->tex->Load(texturewin);
	texturescene4 = app->tex->Load(texturedeath);

	// L07 DONE 5: Add physics to the player - initialize physics body
	pbody = app->physics->CreateCircle(position.x-50, position.y-276, 12, bodyType::DYNAMIC);

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
	
	int speed = 3; 
	int currentspeed = 0;
	
	b2Vec2 vel = b2Vec2(0, -GRAVITY_Y); 

	//L02: DONE 4: modify the position of the player using arrow keys and render the texture
	if (introactiva == false) {
		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT && time > 0 && ultimatelosecondition == false) {
			vel = b2Vec2(0, +2 * GRAVITY_Y);
			time--;
			app->audio->PlayFx(jumpsound);
			if (grounded) {
				yVel = 0.85 * GRAVITY_Y;
				grounded = false;
				currentAnimation = &jumpingesquerra;
			}
		}
		else if (ultimatelosecondition == false && grounded == true) {
			b2Vec2(0, -GRAVITY_Y);
			currentAnimation = &idleanim;
		}

		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && ultimatelosecondition == false) {
			currentspeed = -speed;

			if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT && time > 0) {
				if (grounded) {
					yVel = 0.85 * GRAVITY_Y;
					grounded = false;
				}
				app->audio->PlayFx(jumpsound);
				time--;
			}
			else if (position.x < 133 * 23) {
				app->audio->PlayFx(runsound);
			}
			if (position.x > 23 * 5 && position.x < 107 * 23) {
				app->render->camera.x = -position.x + 100;
			}
			currentAnimation = &leftwalk;
		}

		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && ultimatelosecondition == false) {
			currentspeed = speed;

			if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT && time > 0) {
				if (grounded) {
					yVel = 0.85 * GRAVITY_Y;
					grounded = false;
				}
				time--;
				app->audio->PlayFx(jumpsound);
			}
			else if (position.x < 133 * 23) {
				app->audio->PlayFx(runsound);
			}
			if (position.x > 23 * 5 && position.x < 107 * 23) {
				app->render->camera.x = -position.x + 100;
			}
			currentAnimation = &rightwalk;
		}
		
		if (opciontimer2 == true) {
			timer2--;
		}

		if (timer2 <= 0) {
			grounded = true;
			opciontimer2 = false;
			timer2 = 50;
		}

		if (app->input->GetKey(SDL_SCANCODE_K)==KEY_DOWN && ultimatelosecondition == false){
			pbody2=app->physics->CreateRectangle(position.x + 25, position.y+10, 15,6, bodyType::STATIC);
			currentAnimation = &atacacion;
			grounded = false;
			opciontimer2 = true;
		}
	}
	if (!grounded) {
		yVel -= GRAVITY_Y * 0.02;
	}

	if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_UP && ultimatelosecondition == false) {
		time = 0;
	}
	if(position.x > 133 * 23 && wincondition == false && app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_IDLE && app->input->GetKey(SDL_SCANCODE_A) == KEY_IDLE && app->input->GetKey(SDL_SCANCODE_D) == KEY_IDLE && ultimatelosecondition == false){
		app->audio->PlayFx(winsound);
		wincondition = true;
	}
	if (losecondition == true && godmode == 1) {
		ultimatelosecondition = true;
		currentAnimation = &muertesita;
		app->audio->PlayFx(deathsound);
	}

	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) {
		godmode * -1;
	}


	//Set the velocity of the pbody of the player
	vel = b2Vec2(currentspeed, yVel);
	pbody->body->SetLinearVelocity(vel);

	//Update player position in pixels
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 15;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 14;
	SDL_Rect rect = currentAnimation->GetCurrentFrame();
	if (app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN) {
		introactiva = false;
	}
	if (introactiva == false) {
		app->render->DrawTexture(texture, position.x - 12, position.y - 28, &rect);
	}
	//app->render->DrawTexture(texture, position.x , position.y);
	currentAnimation->Update();

	if (ganar == true) {
		app->render->DrawTexture(texturescene3, 2360, 0);
	}

	if (lose == true) {
		timer--;;
		if (position.x < 23 * 5 && timer <= 0) {
			app->render->DrawTexture(texturescene4, 0, 0);
		}
		if (position.x > 23 * 5 && position.x < 107 * 23 && timer <= 0) {
			app->render->DrawTexture(texturescene4, position.x - 106, 0);
		}
		if (position.x > 107 * 23 && timer <= 0) {
			app->render->DrawTexture(texturescene4, 2360, 0);
		}
	}

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
			yVel = 3;
			grounded = true;
			currentAnimation->Reset();
			break;
		case ColliderType::UNKNOWN:
			LOG("Collision UNKNOWN");
			break;
		case ColliderType::WIN:
			LOG("Collision WIN");
			app->audio->PlayFx(winsound);
			//PASAMOS A PANTALLA GANADORA
			ganar = true;
			break;
		case ColliderType::LOSE:
			LOG("Collision LOSE");
			//PASAMOS A PANTALLA PERDEDORA
			losecondition = true;
			lose = true;
			break;
		case ColliderType::ENEMY:
			LOG("Collision ENEMY");
			losecondition = true;
			lose = true;
			break;

		default: grounded = false;
			
	}
	


}
