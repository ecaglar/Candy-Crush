#pragma once
#include "SceneController.h"
#include "ResourceController.h"
/*
	root of the game
	creates controllers, initializes them and starts the game loop
*/
class Engine
{
public:
	Engine();
	~Engine();

	bool initGame();		//load resoruces, sets background and generate level
	void initContollers();	//inits controllers
	void startGame();		//starts game and game loop
	void destroyGame();		//destroy game

private:
	//controllers
	SceneController*		sceneController = NULL;	
	ResourceController*		resourceController = NULL;
	AnimationController*	animationController = NULL;

	SDL_Renderer* renderer = NULL;
	Uint32 timeLeft(Uint32);

};

