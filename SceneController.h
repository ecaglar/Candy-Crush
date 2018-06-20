#pragma once
#include "DrawableEntity.h"
#include "AnimationController.h"
#include <vector>
#include <set>
#include <queue>
#include <iostream>
#include <fstream>
#include "SDL_ttf.h"
using namespace std;

/*
	Main controller responsible for game scene.Handles input, updates game state, uses animation manager etc.
	Game objects those are wanted to be rendered on screen, must be added to scene controller first.Scene controller
	is responsible for updating and rendering all game entities as well as starts animations.
*/
class SceneController
{

public:
	SceneController();
	~SceneController();

	void addToScene(DrawableEntity* entity);	//adds given drawable entity to the scene so that it can be rendered
	void renderScene();							//render scene 
	void update();								//updates scene according to user input and animation state
	bool init();								//inits scene controller
	SDL_Renderer* getRenderer();
	DrawableEntity* pickEntity(SDL_Point&);		//picks game entity from a given point (game screen coordinates)
	void setBackground(SDL_Texture*);
	void setAnimationController(AnimationController* animController);	//aniamtion controller injected from engine.
	void handleEvent(SDL_Event*);			//handles user inputs

	void generateLevel();			//generate level 
	bool checkIfValidMove(DrawableEntity* const from, DrawableEntity* const to); //when user wants to make a swap it checks if it results with a combo at least 3 items
	void performMove(DrawableEntity*from, DrawableEntity* to);	//if it is a valid swap then performs the swap
	void checkPossipleCombosOnBoard();							//checks all possible combos at that state.
	void removeComboItems(std::set<DrawableEntity*>*);			//when combo found, it removes combo objects from board
	void collapseBoard();										//after a successful removal, collapse the objects to fill empty slots
	void fillEmptySlots();										//after collapsing, there will be empty slots at the top of the board so it detects and fills those slots
	void cleanup();								
	void setGameStartTime(Uint32);
	bool checkIfTimeIsUp();	
	bool quit = false;

private:
	SDL_Window*		gameWindow = 0;
	SDL_Surface*	gameSurface = 0;

	SDL_Renderer*	renderer = NULL;
	DrawableEntity* selectedEntity = NULL;			//picked entity
	SDL_Texture*	background = NULL;
	DrawableEntity* gameMatrix[GAME_MATRIX_SIZE_Y][GAME_MATRIX_SIZE_X];	//game matrix that holds game objects
	bool			collapseFlag = false;
	bool			moveFlag = false;
	bool			fillFlag = false;
	bool			gameOver = false;
	bool			handleInput = true;
	AnimationController* animationController;		//animation controller reference

	void onMouseButtonDown(SDL_Event*);
	void onMouseButtonUp(SDL_Event*);
	void onMouseMotion(SDL_Event*);

	//to transfer data to update after waiting animation completed
	std::set<DrawableEntity*> comboItems;	 

	//shows which column has how many empty slot such as if [1] = 5 means 1st column has 5 empty slot
	int coordinatesToNull[GAME_MATRIX_SIZE_X];
	std::queue<SwapData> swaps;

	//font related
	TTF_Font *      gFont  = NULL;
	SDL_Surface*    scoreSurface = NULL;
	SDL_Surface*    timeSurface = NULL;

	DrawableEntity*	gameoverEnt = NULL;

	SDL_Texture*	scoreTexture = NULL;
	SDL_Texture*	timeTexture = NULL;
	SDL_Rect		timeTextRect;
	SDL_Rect		scoreTextRect;
	Uint32			gameStartTime;

	int score = 0;
	bool initText();
	bool updateScore(int, SDL_Color textColor);
	bool updateScore(int);
	bool updateTime();
	bool updateTime(SDL_Color textColor);
	bool displayGameOver();
};

