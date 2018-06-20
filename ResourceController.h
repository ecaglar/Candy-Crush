#pragma once
#include <map>
#include "SDL.h"
#include "Common.h"
using namespace std;

/*
	Responsible for the resoruces and assets related to game
	Resource controller first loads all the resources to memory and
	all instances uses the same texture from memory with referencing them.
	When a game entity is created, it gets its resoruce from resource manager internally.
*/
class ResourceController 
{
private:
	static  map<GAME_TEX, SDL_Texture*> *resourceMap; //map that saves game resources.It maps enums with tecxtures so we can get textures by names.

public:
	ResourceController();
	~ResourceController();

	bool loadResources();		//loads all resoruces
	static SDL_Texture* getResource(GAME_TEX obj);	//gets a spesific texture by its type
	bool init(SDL_Renderer*);	//inits the resoruce controller
	void cleanup();

private:
	bool loadBackground();		//loads backgroun texture.
	SDL_Renderer* renderer = NULL;

};

