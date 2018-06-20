#include "ResourceController.h"
#include "SDL_image.h"
#include <iostream>
using namespace std;
ResourceController::ResourceController(){}
ResourceController::~ResourceController(){}

map<GAME_TEX, SDL_Texture*>* ResourceController::resourceMap = new map<GAME_TEX, SDL_Texture*>();
bool ResourceController::init(SDL_Renderer* r)
{
	renderer = r;
	return true;
}

bool ResourceController::loadResources()
{
	if (renderer == NULL)
	{
		cout << "Renderer should be initialized via init() method first" << endl;
		return false;
	}
	loadBackground();

	SDL_Surface* over = IMG_Load("./images/gameover.png");
	if (over == NULL) { return false; }
	SDL_Texture* overText = SDL_CreateTextureFromSurface(renderer, over);
	
	SDL_Surface* blue = IMG_Load("./images/Blue.png");
	if (blue == NULL) { return false; }
	SDL_Texture* blueTex = SDL_CreateTextureFromSurface(renderer, blue);
	
	SDL_Surface* green = IMG_Load("./images/Green.png");
	if (green == NULL) { return false; }
	SDL_Texture* greenTex = SDL_CreateTextureFromSurface(renderer, green);

	SDL_Surface* purple = IMG_Load("./images/Purple.png");
	if (purple == NULL) { return false; }
	SDL_Texture* purpleTex = SDL_CreateTextureFromSurface(renderer, purple);

	SDL_Surface* red = IMG_Load("./images/Red.png");
	if (red == NULL) { return false; }
	SDL_Texture* redTex = SDL_CreateTextureFromSurface(renderer, red);
	
	SDL_Surface* yellow = IMG_Load("./images/Yellow.png");
	if (yellow == NULL) { return false; }
	SDL_Texture* yellowTex = SDL_CreateTextureFromSurface(renderer, yellow);
	
	resourceMap->insert(std::pair<GAME_TEX, SDL_Texture*>(BLUE_OBJ, blueTex));
	resourceMap->insert(std::pair<GAME_TEX, SDL_Texture*>(GREEN_OBJ, greenTex));
	resourceMap->insert(std::pair<GAME_TEX, SDL_Texture*>(PURPLE_OBJ, purpleTex));
	resourceMap->insert(std::pair<GAME_TEX, SDL_Texture*>(RED_OBJ, redTex));
	resourceMap->insert(std::pair<GAME_TEX, SDL_Texture*>(YELLOW_OBJ, yellowTex));
	resourceMap->insert(std::pair<GAME_TEX, SDL_Texture*>(GAMEOVER, overText));

	SDL_FreeSurface(over);
	SDL_FreeSurface(blue);
	SDL_FreeSurface(green);
	SDL_FreeSurface(purple);
	SDL_FreeSurface(red);
	SDL_FreeSurface(yellow);
	return true;
}
bool ResourceController::loadBackground()
{
	SDL_Surface* bg = IMG_Load("./images/BackGround.jpg");
	if (bg == NULL)
	{
		return false;
	}
	else
	{
		SDL_Texture* bgTex = SDL_CreateTextureFromSurface(renderer, bg);
		resourceMap->insert(std::pair<GAME_TEX, SDL_Texture*>(BACKGROUND, bgTex));
		return true;
	}	
}

 SDL_Texture* ResourceController::getResource(GAME_TEX obj)
{
	resourceMap->find(obj);
	map<GAME_TEX, SDL_Texture*>::iterator it = resourceMap->find(obj);
	if (it == resourceMap->end())
	{
		return NULL;
	}
	return it->second;
	
}

 void ResourceController::cleanup()
 {
	 map<GAME_TEX, SDL_Texture*>::iterator	i = resourceMap->begin();
	 for (; i != resourceMap->end(); i++)
	 {
		 SDL_Texture* texture = i->second;
		 SDL_DestroyTexture(texture);
	 }
	 delete resourceMap;
 }

