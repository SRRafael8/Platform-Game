
#include "App.h"
#include "Render.h"
#include "Textures.h"
#include "Map.h"
#include "Physics.h"

#include "Defs.h"
#include "Log.h"

#include <math.h>
#include "SDL_image/include/SDL_image.h"

Map::Map() : Module(), mapLoaded(false)
{
    name.Create("map");
}

// Destructor
Map::~Map()
{}

// Called before render is available
bool Map::Awake(pugi::xml_node& config)
{
    LOG("Loading Map Parser");
    bool ret = true;

    mapFileName = config.child("mapfile").attribute("path").as_string();
    mapFolder = config.child("mapfolder").attribute("path").as_string();

    return ret;
}

bool Map::CreateWalkabilityMap(int& width, int& height, uchar** buffer) const
{
    bool ret = false;
    ListItem<MapLayer*>* item;
    item = mapData.maplayers.start;

    for (item = mapData.maplayers.start; item != NULL; item = item->next)
    {
        MapLayer* layer = item->data;

        if (layer->properties.GetProperty("Navigation") != NULL && !layer->properties.GetProperty("Navigation")->value)
            continue;

        uchar* map = new uchar[layer->width * layer->height];
        memset(map, 1, layer->width * layer->height);

        for (int y = 0; y < mapData.height; ++y)
        {
            for (int x = 0; x < mapData.width; ++x)
            {
                int i = (y * layer->width) + x;

                int tileId = layer->Get(x, y);
                TileSet* tileset = (tileId > 0) ? GetTilesetFromTileId(tileId) : NULL;

                if (tileset != NULL)
                {
                    //According to the mapType use the ID of the tile to set the walkability value
                    if (tileId == 318) {
                        map[i] = 1;
                   }
                       
                   else map[i] = 0;
                }
            }
        }

        *buffer = map;
        width = mapData.width;
        height = mapData.height;
        ret = true;

        break;
    }

    return ret;
}


void Map::Draw()
{
    if(mapLoaded == false)
        return;

    /*
    // L04: DONE 6: Iterate all tilesets and draw all their 
    // images in 0,0 (you should have only one tileset for now)

    ListItem<TileSet*>* tileset;
    tileset = mapData.tilesets.start;

    while (tileset != NULL) {
        app->render->DrawTexture(tileset->data->texture,0,0);
        tileset = tileset->next;
    }
    */

    // L05: DONE 5: Prepare the loop to draw all tiles in a layer + DrawTexture()

    ListItem<MapLayer*>* mapLayerItem;
    mapLayerItem = mapData.maplayers.start;

    while (mapLayerItem != NULL) {

        //L06: DONE 7: use GetProperty method to ask each layer if your �Draw� property is true.
        if (mapLayerItem->data->properties.GetProperty("Draw") != NULL && mapLayerItem->data->properties.GetProperty("Draw")->value) {

            for (int x = 0; x < mapLayerItem->data->width; x++)
            {
                for (int y = 0; y < mapLayerItem->data->height; y++)
                {
                    // L05: DONE 9: Complete the draw function
                    int gid = mapLayerItem->data->Get(x, y);

                    //L06: DONE 3: Obtain the tile set using GetTilesetFromTileId
                    TileSet* tileset = GetTilesetFromTileId(gid);

                    SDL_Rect r = tileset->GetTileRect(gid);
                    iPoint pos = MapToWorld(x, y);

                    app->render->DrawTexture(tileset->texture,
                        pos.x,
                        pos.y,
                        &r);
                }
            }
        }
        mapLayerItem = mapLayerItem->next;

    }
}

// L05: DONE 8: Create a method that translates x,y coordinates from map positions to world positions
iPoint Map::MapToWorld(int x, int y) const
{
    iPoint ret;

    ret.x = x * mapData.tileWidth;
    ret.y = y * mapData.tileHeight;

    return ret;
}

iPoint Map::WorldToMap(int x, int y)
{
    iPoint ret(0, 0);

    if (mapData.type == MAPTYPE_ORTHOGONAL)
    {
        ret.x = x / mapData.tileWidth;
        ret.y = y / mapData.tileHeight;
    }
    else
    {
        LOG("Unknown map type");
        ret.x = x; ret.y = y;
    }

    return ret;
}

// Get relative Tile rectangle
SDL_Rect TileSet::GetTileRect(int gid) const
{
    SDL_Rect rect = { 0 };
    int relativeIndex = gid - firstgid;

    // L05: DONE 7: Get relative Tile rectangle
    rect.w = tileWidth;
    rect.h = tileHeight;
    rect.x = margin + (tileWidth + spacing) * (relativeIndex % columns);
    rect.y = margin + (tileWidth + spacing) * (relativeIndex / columns);

    return rect;
}


// L06: DONE 2: Pick the right Tileset based on a tile id
TileSet* Map::GetTilesetFromTileId(int gid) const
{
    ListItem<TileSet*>* item = mapData.tilesets.start;
    TileSet* set = NULL;

    while (item)
    {
        set = item->data;
        if (gid < (item->data->firstgid + item->data->tilecount))
        {
            break;
        }
        item = item->next;
    }

    return set;
}

// Called before quitting
bool Map::CleanUp()
{
    LOG("Unloading map");

    // L04: DONE 2: Make sure you clean up any memory allocated from tilesets/map
	ListItem<TileSet*>* item;
	item = mapData.tilesets.start;

	while (item != NULL)
	{
		RELEASE(item->data);
		item = item->next;
	}
	mapData.tilesets.Clear();

    // L05: DONE 2: clean up all layer data
    // Remove all layers
    ListItem<MapLayer*>* layerItem;
    layerItem = mapData.maplayers.start;

    while (layerItem != NULL)
    {
        RELEASE(layerItem->data);
        layerItem = layerItem->next;
    }

    return true;
}

// Load new map
bool Map::Load()
{
    bool ret = true;

    pugi::xml_document mapFileXML;
    pugi::xml_parse_result result = mapFileXML.load_file(mapFileName.GetString());

    if(result == NULL)
    {
        LOG("Could not load map xml file %s. pugi error: %s", mapFileName, result.description());
        ret = false;
    }

    if(ret == true)
    {
        ret = LoadMap(mapFileXML);
    }

    if (ret == true)
    {
        ret = LoadTileSet(mapFileXML);
    }

    // L05: DONE 4: Iterate all layers and load each of them
    if (ret == true)
    {
        ret = LoadAllLayers(mapFileXML.child("map"));
    }
    
    // L07 DONE 3: Create colliders
    // Later you can create a function here to load and create the colliders from the map


    //Plataforma 1
    PhysBody* c3 = app->physics->CreateRectangle(195, 704 + 32 - 206, 23 * 17, 4 * 23, STATIC);
    // L07 DONE 7: Assign collider type
    c3->ctype = ColliderType::PLATFORM;

    //plataforma 2
    PhysBody* c2 = app->physics->CreateRectangle(540, 704 + 32 - 206, 23 * 7, 4 * 23, STATIC);
    c2->ctype = ColliderType::PLATFORM;

    //plataforma3
    PhysBody* c1 = app->physics->CreateRectangle(23*29+57, 704 + 32 - 206,23*5, 4 * 23, STATIC);
    c1->ctype = ColliderType::PLATFORM;
    PhysBody* c4 = app->physics->CreateRectangle(23 * 35, 704 + 32 - 206-12, 23 * 2, 5 * 23, STATIC);
    c4->ctype = ColliderType::PLATFORM;
    PhysBody* c5 = app->physics->CreateRectangle(23 * 37, 704 + 32 - 206 - 24, 23 * 2, 6 * 23, STATIC);
    c5->ctype = ColliderType::PLATFORM;
    PhysBody* c6 = app->physics->CreateRectangle(23 * 41 +12, 704 + 32 - 206 - 35, 23 * 7, 7 * 23, STATIC);
    c6->ctype = ColliderType::PLATFORM;

    //plataforma 4
    PhysBody* c7 = app->physics->CreateRectangle(23 * 49,16*23+12, 23 * 4,23, STATIC);
    c7->ctype = ColliderType::PLATFORM;

    //plataforma 5
    PhysBody* c8 = app->physics->CreateRectangle(23 * 58, 15 * 23 + 12, 23 * 8, 23, STATIC);
    c8->ctype = ColliderType::PLATFORM;

    //plataforma 6
    PhysBody* c9 = app->physics->CreateRectangle( 23*75-12 , 704 + 32 - 195, 23 * 19, 3 * 23, STATIC);
    c9->ctype = ColliderType::PLATFORM;

    //plataforma 7
    PhysBody* c10 = app->physics->CreateRectangle(23 * 75 - 12, 704 + 32 - 195, 23 * 19, 3 * 23, STATIC);
    c10->ctype = ColliderType::PLATFORM;
    
    //plataforma 8
    PhysBody* c11 = app->physics->CreateRectangle(23 * 90 - 12, 704 + 32 - 195, 23 * 5, 3 * 23, STATIC);
    c11->ctype = ColliderType::PLATFORM;
    
    //plataforma 9
    PhysBody* c12 = app->physics->CreateRectangle(23 * 96, 704 + 32 - 218, 23 * 8, 5 * 23, STATIC);
    c12->ctype = ColliderType::PLATFORM;
    
    //plataforma 10
    PhysBody* c13 = app->physics->CreateRectangle(23 * 101 + 12, 704 + 32 - 287, 23 * 7, 3 * 23, STATIC);
    c13->ctype = ColliderType::PLATFORM;
    
    //plataforma 11
    PhysBody* c14 = app->physics->CreateRectangle(23 * 101 + 12 + 7*23, 5*23 + 704 + 32 - 287, 23 * 17, 1 * 23, STATIC);
    c14->ctype = ColliderType::PLATFORM;
    
    //plataforma 12
    PhysBody* c15 = app->physics->CreateRectangle(23 * 113 + 12 + 7 * 23, 4 * 23 + 704 + 32 - 287, 23 * 3, 1 * 23, STATIC);
    c15->ctype = ColliderType::PLATFORM;
    
    //plataforma 13
    PhysBody* c16 = app->physics->CreateRectangle(23 * 125, 3 * 23 + 704 + 32 - 287, 23 * 2, 1 * 23, STATIC);
    c16->ctype = ColliderType::PLATFORM;

    //plataforma 14
    PhysBody* c17 = app->physics->CreateRectangle(23 * 129, 2 * 23 + 704 + 32 - 287, 23 * 2, 1 * 23, STATIC);
    c17->ctype = ColliderType::PLATFORM;

    //plataforma 15
    PhysBody* c18 = app->physics->CreateRectangle(23 * 133+12, 1 * 23 + 704 + 32 - 287, 23 * 1, 1 * 23, STATIC);
    c18->ctype = ColliderType::PLATFORM;

    //plataforma 16
    PhysBody* c19 = app->physics->CreateRectangle(23 * 136 + 12, -1 * 23 + 704 + 32 - 287, 23 * 1, 1 * 23, STATIC);
    c19->ctype = ColliderType::PLATFORM;

    //plataforma 17
    PhysBody* c20 = app->physics->CreateRectangle(23 * 139 + 12, 0 * 23 + 704 + 32 - 287, 23 * 1, 1 * 23, STATIC);
    c20->ctype = ColliderType::PLATFORM;

    //plataforma 18
    PhysBody* c21 = app->physics->CreateRectangle(23 * 146, 1 * 23 + 704 + 32 - 275, 23 * 8, 8 * 23, STATIC);
    c21->ctype = ColliderType::PLATFORM;

    //plataforma portal
    PhysBody* cmoneda = app->physics->CreateRectangle(117*23+12, 5*23, 23 * 9, 23 * 2, STATIC);
    cmoneda->ctype = ColliderType::PLATFORM;

    //plataforma 19 dead
    PhysBody* c22 = app->physics->CreateRectangle(23 * 75, 25 * 23 + 12, 23 * 155, 1 * 23, STATIC);
    c22->ctype = ColliderType::LOSE;

    //plataforma 20 wall
    PhysBody* c23 = app->physics->CreateRectangle(-12, 12 * 23 + 12, 23 * 1, 25 * 23, STATIC);
    c23->ctype = ColliderType::PLATFORM;

    //plataforma 20 wall
    PhysBody* c24 = app->physics->CreateRectangle(150 * 23 + 12, 12 * 23 + 12, 23 * 1, 25 * 23, STATIC);
    c24->ctype = ColliderType::WIN;

    //spikes
    PhysBody* c25 = app->physics->CreateRectangle(518, 480, 15, 10, STATIC);
    c25->ctype = ColliderType::LOSE;
    PhysBody* c26 = app->physics->CreateRectangle(772, 480, 15, 10, STATIC);
    c26->ctype = ColliderType::LOSE;
    PhysBody* c27 = app->physics->CreateRectangle(1553, 502, 15, 10, STATIC);
    c27->ctype = ColliderType::LOSE;
    PhysBody* c28 = app->physics->CreateRectangle(2336, 411, 15, 10, STATIC);
    c28->ctype = ColliderType::LOSE;
    PhysBody* c29 = app->physics->CreateRectangle(2356, 411, 20, 10, STATIC);
    c29->ctype = ColliderType::LOSE;
    PhysBody* c30 = app->physics->CreateRectangle(2772, 525, 15, 10, STATIC);
    c30->ctype = ColliderType::LOSE;

    //PORTAL
    PhysBody* cportalup = app->physics->CreateRectangle(2602, 70, 5, 22*2, STATIC);
    cportalup->ctype = ColliderType::TPDOWN;
    PhysBody* cportaldown = app->physics->CreateRectangle(2307, 526, 5, 22 * 2, STATIC);
    cportaldown->ctype = ColliderType::TPUP;

    //COINS
    PhysBody* ccoin1 = app->physics->CreateCircle(2671, 59, 8, STATIC);
    ccoin1->ctype = ColliderType::ITEM;
    PhysBody* ccoin2 = app->physics->CreateCircle(2701, 59, 8, STATIC);
    ccoin2->ctype = ColliderType::ITEM;
    PhysBody* ccoin3 = app->physics->CreateCircle(2731, 59, 8, STATIC);
    ccoin3->ctype = ColliderType::ITEM;
    PhysBody* ccoin4 = app->physics->CreateCircle(2761, 59, 8, STATIC);
    ccoin4->ctype = ColliderType::ITEM;
    PhysBody* ccoin5 = app->physics->CreateCircle(2791, 59, 8, STATIC);
    ccoin5->ctype = ColliderType::ITEM;
    PhysBody* ccoin6 = app->physics->CreateCircle(2671, 79, 8, STATIC);
    ccoin6->ctype = ColliderType::ITEM;
    PhysBody* ccoin7 = app->physics->CreateCircle(2701, 79, 8, STATIC);
    ccoin7->ctype = ColliderType::ITEM;
    PhysBody* ccoin8 = app->physics->CreateCircle(2731, 79, 8, STATIC);
    ccoin8->ctype = ColliderType::ITEM;
    PhysBody* ccoin9 = app->physics->CreateCircle(2761, 79, 8, STATIC);
    ccoin9->ctype = ColliderType::ITEM;
    PhysBody* ccoin10 = app->physics->CreateCircle(2791, 79, 8, STATIC);
    ccoin10->ctype = ColliderType::ITEM;

    if(ret == true) 
    {
        // L04: DONE 5: LOG all the data loaded iterate all tilesets and LOG everything
       
        LOG("Successfully parsed map XML file :%s", mapFileName.GetString());
        LOG("width : %d height : %d",mapData.width,mapData.height);
        LOG("tile_width : %d tile_height : %d",mapData.tileWidth, mapData.tileHeight);
        
        LOG("Tilesets----");

        ListItem<TileSet*>* tileset;
        tileset = mapData.tilesets.start;

        while (tileset != NULL) {
            LOG("name : %s firstgid : %d",tileset->data->name.GetString(), tileset->data->firstgid);
            LOG("tile width : %d tile height : %d", tileset->data->tileWidth, tileset->data->tileHeight);
            LOG("spacing : %d margin : %d", tileset->data->spacing, tileset->data->margin);
            tileset = tileset->next;
        }

        // L05: DONE 4: LOG the info for each loaded layer
        ListItem<MapLayer*>* mapLayer;
        mapLayer = mapData.maplayers.start;

        while (mapLayer != NULL) {
            LOG("id : %d name : %s", mapLayer->data->id, mapLayer->data->name.GetString());
            LOG("Layer width : %d Layer height : %d", mapLayer->data->width, mapLayer->data->height);
            mapLayer = mapLayer->next;
        }
    }

    if(mapFileXML) mapFileXML.reset();

    mapLoaded = ret;

    return ret;
}

// L04: DONE 3: Implement LoadMap to load the map properties
bool Map::LoadMap(pugi::xml_node mapFile)
{
    bool ret = true;
    pugi::xml_node map = mapFile.child("map");

    if (map == NULL)
    {
        LOG("Error parsing map xml file: Cannot find 'map' tag.");
        ret = false;
    }
    else
    {
        //Load map general properties
        mapData.height = map.attribute("height").as_int();
        mapData.width = map.attribute("width").as_int();
        mapData.tileHeight = map.attribute("tileheight").as_int();
        mapData.tileWidth = map.attribute("tilewidth").as_int();
        mapData.type = MAPTYPE_ORTHOGONAL;
        mapData.type = MAPTYPE_ORTHOGONAL;
    }

    return ret;
}

// L04: DONE 4: Implement the LoadTileSet function to load the tileset properties
bool Map::LoadTileSet(pugi::xml_node mapFile){

    bool ret = true; 

    pugi::xml_node tileset;
    for (tileset = mapFile.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
    {
        TileSet* set = new TileSet();

        // L04: DONE 4: Load Tileset attributes
        set->name = tileset.attribute("name").as_string();
        set->firstgid = tileset.attribute("firstgid").as_int();
        set->margin = tileset.attribute("margin").as_int();
        set->spacing = tileset.attribute("spacing").as_int();
        set->tileWidth = tileset.attribute("tilewidth").as_int();
        set->tileHeight = tileset.attribute("tileheight").as_int();
        set->columns = tileset.attribute("columns").as_int();
        set->tilecount = tileset.attribute("tilecount").as_int();

        // L04: DONE 4: Load Tileset image
        SString tmp("%s%s", mapFolder.GetString(), tileset.child("image").attribute("source").as_string());
        set->texture = app->tex->Load(tmp.GetString());

        mapData.tilesets.Add(set);
    }

    return ret;
}

// L05: DONE 3: Implement a function that loads a single layer layer
bool Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
    bool ret = true;

    //Load the attributes
    layer->id = node.attribute("id").as_int();
    layer->name = node.attribute("name").as_string();
    layer->width = node.attribute("width").as_int();
    layer->height = node.attribute("height").as_int();

    //L06: DONE 6 Call Load Propoerties
    LoadProperties(node, layer->properties);

    //Reserve the memory for the data 
    layer->data = new uint[layer->width * layer->height];
    memset(layer->data, 0, layer->width * layer->height);

    //Iterate over all the tiles and assign the values
    pugi::xml_node tile;
    int i = 0;
    for (tile = node.child("data").child("tile"); tile && ret; tile = tile.next_sibling("tile"))
    {
        layer->data[i] = tile.attribute("gid").as_int();
        i++;
    }

    return ret;
}

// L05: DONE 4: Iterate all layers and load each of them
bool Map::LoadAllLayers(pugi::xml_node mapNode) {
    bool ret = true;

    for (pugi::xml_node layerNode = mapNode.child("layer"); layerNode && ret; layerNode = layerNode.next_sibling("layer"))
    {
        //Load the layer
        MapLayer* mapLayer = new MapLayer();
        ret = LoadLayer(layerNode, mapLayer);

        //add the layer to the map
        mapData.maplayers.Add(mapLayer);
    }

    return ret;
}

// L06: DONE 6: Load a group of properties from a node and fill a list with it
bool Map::LoadProperties(pugi::xml_node& node, Properties& properties)
{
    bool ret = false;

    for (pugi::xml_node propertieNode = node.child("properties").child("property"); propertieNode; propertieNode = propertieNode.next_sibling("property"))
    {
        Properties::Property* p = new Properties::Property();
        p->name = propertieNode.attribute("name").as_string();
        p->value = propertieNode.attribute("value").as_bool(); // (!!) I'm assuming that all values are bool !!

        properties.list.Add(p);
    }

    return ret;
}


// L06: DONE 7: Ask for the value of a custom property
Properties::Property* Properties::GetProperty(const char* name)
{
    ListItem<Property*>* item = list.start;
    Property* p = NULL;

    while (item)
    {
        if (item->data->name == name) {
            p = item->data;
            break;
        }
        item = item->next;
    }

    return p;
}


