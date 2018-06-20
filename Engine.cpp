#include "Engine.h"
#include "SDL.h"
#include <iostream>
using namespace std;
Engine::Engine(){}

Engine::~Engine(){}

void Engine::destroyGame(){}
void Engine::initContollers()
{
	sceneController = new SceneController();
	resourceController = new ResourceController();
	animationController = new AnimationController();
	sceneController->init();
	resourceController->init(sceneController->getRenderer());
	sceneController->setAnimationController(animationController);	
}
bool Engine::initGame()
{
	bool success = false;	
	srand(time(NULL));
	if (resourceController->loadResources() == false)
	{
		success = false;
	}
	
	SDL_Texture* bg = resourceController->getResource(BACKGROUND);
	sceneController->setBackground(bg);
	sceneController->generateLevel();
	return success;
}
void Engine::startGame(){

	//gameloop
	bool quit = false;
	double previous = SDL_GetTicks();
	double lag = 0.0f;
	sceneController->setGameStartTime(previous);
	while (!quit)
	{		
		if (sceneController->quit)
		{
			quit = true;
		}
		double current = SDL_GetTicks();
		double elapsed = current - previous;
		previous = current;
		lag += elapsed;
		while (lag >= MS_60_MS)
		{
			SDL_Event event;
			while (SDL_PollEvent(&event)) {
				if (event.type == SDL_WINDOWEVENT) {
					if (event.window.type == SDL_WINDOWEVENT_CLOSE)
					{
						quit = true;
					}
				}
				if (event.type == SDL_QUIT) {
					
						quit = true;					
				}					
				sceneController->handleEvent(&event);
			}
			sceneController->update();
			lag -= MS_60_MS;
		}
		sceneController->renderScene();		
	}

	sceneController->cleanup();
	resourceController->cleanup();
	delete animationController;
	delete sceneController;
	delete resourceController;
}
