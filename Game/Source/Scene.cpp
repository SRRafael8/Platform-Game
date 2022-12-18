#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "EntityManager.h"
#include "Map.h"
#include "PathFinding.h"

#include "Defs.h"
#include "Log.h"

Scene::Scene() : Module()
{
	name.Create("scene");
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");
	bool ret = true;

	// iterate all objects in the scene
	// Check https://pugixml.org/docs/quickstart.html#access
	for (pugi::xml_node itemNode = config.child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
	{
		Item* item = (Item*)app->entityManager->CreateEntity(EntityType::ITEM);
		item->parameters = itemNode;
	}

	//L02: DONE 3: Instantiate the player using the entity manager
	player = (Player*)app->entityManager->CreateEntity(EntityType::PLAYER);
	player->parameters = config.child("player");

	enemy = (Enemy*)app->entityManager->CreateEntity(EntityType::ENEMY);
	enemy->parameters = config.child("enemy");

	enemyV = (EnemyV*)app->entityManager->CreateEntity(EntityType::ENEMYV);
	enemyV->parameters = config.child("enemyV");

	textureintro = "Assets/Scenes/startscene.png";
	texturelobby = "Assets/Scenes/lobbyscreen.png";
	texturedeath = "Assets/Scenes/deathscreen.png";
	texturewin = "Assets/Scenes/winscreen.png";

	return ret;

}

// Called before the first frame
bool Scene::Start()
{
	//img = app->tex->Load("Assets/Textures/test.png");
	app->audio->PlayMusic("Assets/Audio/Music/medievalsong.mp3");
	texturescene = app->tex->Load(textureintro);
	texturescene2 = app->tex->Load(texturelobby);
	// L03: DONE: Load map
	bool retLoad = app->map->Load();

	if (retLoad) {
		int w, h;
		uchar* data = NULL;

		bool retWalkMap = app->map->CreateWalkabilityMap(w, h, &data);
		if (retWalkMap) app->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);

	}

	// L04: DONE 7: Set the window title with map/tileset info
	SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d",
		app->map->mapData.width,
		app->map->mapData.height,
		app->map->mapData.tileWidth,
		app->map->mapData.tileHeight,
		app->map->mapData.tilesets.Count());

	app->win->SetTitle(title.GetString());

	if (app->map->mapData.type == MapTypes::MAPTYPE_ORTHOGONAL) {

		// Texture to highligh mouse position 
		mouseTileTex = app->tex->Load("Assets/Maps/path_square.png");

		// Texture to show path origin 
		originTex = app->tex->Load("Assets/Maps/x_square.png");
	}
	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	

	// L03: DONE 3: Request App to Load / Save when pressing the keys F5 (save) / F6 (load)
	if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		app->SaveGameRequest();

	if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		app->LoadGameRequest();

	if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		app->render->camera.y += 20;

	if (app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		app->render->camera.y -= 20;

	if (app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		app->render->camera.x += 20;

	if (app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		app->render->camera.x -= 20;
	
	//app->render->DrawTexture(img, 380, 100); // Placeholder not needed any more

	// Draw map
	app->map->Draw();

	int mouseX, mouseY;
	app->input->GetMousePosition(mouseX, mouseY);

	iPoint mouseTile = iPoint(0, 0);

	mouseTile = app->map->WorldToMap(	mouseX - app->render->camera.x,
										mouseY - app->render->camera.y);
	

	if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		if (originSelected == true)
		{
			app->pathfinding->CreatePath(origin, mouseTile);
			originSelected = false;
		}
		else
		{
			origin = mouseTile;
			originSelected = true;
			app->pathfinding->ClearLastPath();
		}
	}

	// L12: Get the latest calculated path and draw
	const DynArray<iPoint>* path = app->pathfinding->GetLastPath();
	for (uint i = 0; i < path->Count(); ++i)
	{
		iPoint pos = app->map->MapToWorld(path->At(i)->x, path->At(i)->y);
		app->render->DrawTexture(mouseTileTex, pos.x, pos.y);
	}

	// L12: Debug pathfinding
	iPoint originScreen = app->map->MapToWorld(origin.x, origin.y);
	app->render->DrawTexture(originTex, originScreen.x, originScreen.y);

	// L04: DONE 7: Set the window title with map/tileset info
	SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d Tile:[%d,%d]",
		app->map->mapData.width,
		app->map->mapData.height,
		app->map->mapData.tileWidth,
		app->map->mapData.tileHeight,
		app->map->mapData.tilesets.Count(),
		mouseTile.x,
		mouseTile.y);

	app->win->SetTitle(title.GetString());

	if (introactiva == true) {
		app->render->DrawTexture(texturescene, 0, 0);
		timer--;
	}
	if (timer == 0) {
		introactiva = false;
		app->render->DrawTexture(texturescene2, 0, 0);
	}
	if (app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN) {
		SDL_DestroyTexture(texturescene);
		SDL_DestroyTexture(texturescene2);
	}


	return true;
}



// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if(app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}
