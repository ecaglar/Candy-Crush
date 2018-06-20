#include "SceneController.h"
#include "SDL_image.h"

#include <iostream>
#include <cmath>
#include <map>
#include <queue>
#include <sstream>
#include <memory>
using namespace std;

SceneController::SceneController(){ }
SceneController::~SceneController(){ }

/*
	Inits all scene and render related modules as well as SDL modules.
*/
bool SceneController::init()
{
	bool success = true;
	
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		cout << "Error on init SDL" << endl;
		success = false;

	}else{

		gameWindow = SDL_CreateWindow("KingCrash", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, GAME_WITDH, GAME_HEIGHT, SDL_WINDOW_SHOWN);
		if (gameWindow == NULL)
		{
			cout << "Game window could not be created" << endl;
			success = false;
		}else{
			renderer = SDL_CreateRenderer(gameWindow, -1, SDL_RENDERER_ACCELERATED);
			if (renderer == NULL)
			{
				cout << "cannot create renderer" << endl;
				success = false;
			}
			else
			{
				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}
				else
				{
					//Get window surface
					gameSurface = SDL_GetWindowSurface(gameWindow);
					//Initialize SDL_ttf
					if (TTF_Init() == -1)
					{
						printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
						success = false;
					}
				}
			}
		}
	}

	bool textResult = initText();
	return success && textResult;
}

void SceneController::setBackground(SDL_Texture* bg)
{
	background = bg;
}

void SceneController::addToScene(DrawableEntity* entity)
{
	gameMatrix[entity->x][entity->y] = entity;
}

SDL_Renderer* SceneController::getRenderer()
{
	return renderer;
}

/*
	Updates the animation and scene.
*/
void SceneController::update()
{	
	//if there is ongoing animations then disable user input
	if (animationController->checkAnyAnimation() == false)
	{
		handleInput = true;
	}
	else{
		handleInput = false;
	}
	//If time is up then display game over screen
	if (checkIfTimeIsUp())
	{
		if (!gameOver)
		{
			displayGameOver(); //create it just one time not for each update
		}		
		gameOver = true;
		return;
	}
	
	updateTime(); //calculates remaining time for game and creates text

	animationController->tick(); //update the animation controller which updates all animations inside.
	
	//after colalpsing wait for aniamtions finish then empty top slots depending the collapse and fill them after
	if (animationController->checkAnyAnimation() == false && fillFlag)
	{
		fillFlag = false;
		for (int i = 0; i < GAME_MATRIX_SIZE_X; i++)
		{
			int count = coordinatesToNull[i];
			if (count > 0)
			{
				for (int j = 0; j < count; j++)
				{
					delete gameMatrix[i][j];
					gameMatrix[i][j] = NULL;
				}
			}

		}
		fillEmptySlots();
	}

	//after remove aniamtion completed, remove items and initiate a collapse 
	if (animationController->checkAnyAnimation() == false && collapseFlag)
	{
		std::set<DrawableEntity*>::iterator i = comboItems.begin();
		for (; i != comboItems.end(); i++)
		{
			gameMatrix[(*i)->x][(*i)->y] = NULL;
			delete *i;
			
		}
		collapseFlag = false;
		collapseBoard();
	}
	if (animationController->checkAnyAnimation() == false && moveFlag)
	{
		moveFlag = false;
		
		while (!swaps.empty())
		{
			SwapData d = swaps.front();
			swaps.pop();

			gameMatrix[d.to.x][d.to.y] = gameMatrix[d.from.x][d.from.y];
			gameMatrix[d.to.x][d.to.y]->x = d.to.x;
			gameMatrix[d.to.x][d.to.y]->y = d.to.y;
			gameMatrix[d.to.x][d.to.y]->recalculatePosition();
		}
		for (int i = 0; i < GAME_MATRIX_SIZE_X; i++)
		{
			int count = coordinatesToNull[i];
			if (count > 0)
			{
				for (int j = 0; j < count; j++)
				{
					gameMatrix[i][j] = NULL;
				}
			}

		}
		fillEmptySlots();
	}
}
/*
	handle user inputs.
*/
void SceneController::handleEvent(SDL_Event* event)
{
	//if game over wait for user inputs to pres enter or escape
	if (gameOver)
	{
		if (event->type == SDL_KEYDOWN)
		{
			if (event->key.keysym.sym == SDLK_RETURN) //if return pressed then initiate a new game
			{
				generateLevel();
				handleInput = true;
				gameOver = false;
				gameStartTime = SDL_GetTicks();
			}
			else if (event->key.keysym.sym == SDLK_ESCAPE) //if escape pressed then wuit game.Note that quit flag is checked by game loop to end game
			{
				quit = true;
			}
		}
		
	}

	//if time is up or handleInput flag is set then do not handle events
	if (checkIfTimeIsUp() || handleInput == false) return;

		switch (event->type)
		{
			case SDL_MOUSEBUTTONDOWN:
				onMouseButtonDown(event);
				break;
			case SDL_MOUSEBUTTONUP:
				onMouseButtonUp(event);
				break;
			case SDL_MOUSEMOTION:
				onMouseMotion(event);
				break;
			default:
				break;
		}
}
/*
	picks an entity from game using window coordinates.
	Only draggable entites can be selected.	
*/
DrawableEntity* SceneController::pickEntity(SDL_Point& p)
{
	SDL_Point coordinate;

	//calculate grid positions from window position
	calculateCoordinateFromMousePositions(p.x, p.y, coordinate);

	//if coordinates are outside of our game render area then do nothing
	if (coordinate.x < 0 || coordinate.y < 0)
	{
		return NULL;
	}
	DrawableEntity* ent = gameMatrix[coordinate.x][coordinate.y];
	if (ent != NULL && ent->isDraggable) //if an entity found and it is draggable
	{
		return ent;
	}
	return NULL;
}

/*
	Check if a swap is valid or not.Only swaps those cause a combo can be valid.
	Moreover only user can swap objects with their neighboors means around only 1 unit.

*/
bool SceneController::checkIfValidMove(DrawableEntity* const from, DrawableEntity* const to)
{
	int horizontalCount	= 1;
	int verticalCount	= 1;

	int x = to->x;
	int y = to->y;

	int from_color	= gameMatrix[from->x][from->y]->tex;
	int to_color	= gameMatrix[to->x][to->y]->tex;

	//check horizontal combos to the left side of the object
	for (int i = x - 1; i >= 0; i--)
	{
		if (i != from->x  && gameMatrix[i][y] != NULL && gameMatrix[i][y]->tex == from_color){
			horizontalCount++;			
		}else{
			break;
		}
	}
	//check horizontal combos to the right side of the object
	for (int i = x + 1; i < GAME_MATRIX_SIZE_X; i++)
	{
		if (i != from->x && gameMatrix[i][y] != NULL && gameMatrix[i][y]->tex == from_color){
			horizontalCount++;
		}else{
			break;
		}
	}
	if (horizontalCount >= 3) //horizondal combo found.Enough to make a valid move
	{
		return true;
	}

	//no horizontal move at this point so check if there is any vertical combo.
	// check vertical combo to up side
	for (int i = y - 1; i >= 0; i--)
	{
		if ( i != from->y && gameMatrix[x][i] != NULL && gameMatrix[x][i]->tex == from_color){
			verticalCount++;
		}else{
			break;
		}
	}
	//check vertical combo to down side
	for (int i = y + 1; i < GAME_MATRIX_SIZE_Y; i++)
	{
		if ( i != from->y && gameMatrix[x][i] != NULL && gameMatrix[x][i]->tex == from_color){
			verticalCount++;			
		}else{
			break;
		}
	}
	//vertical combo found
	if (verticalCount >= 3)
	{
		return true;
	}
	return false;
}

/*
	Performs actual swap by updating coordinates.Then board is checked for any possible combos
*/
void SceneController::performMove(DrawableEntity*from, DrawableEntity* to)
{
	//update object positions
	int tempx = from->x;
	from->x = to->x;
	to->x = tempx;

	int tempy = from->y;
	from->y = to->y;
	to->y = tempy;

	//update game board
	gameMatrix[from->x][from->y] = from;
	gameMatrix[to->x][to->y] = to;

	//fit their position according to their coordinates so that they position perfectly to their location
	from->recalculatePosition();
	to->recalculatePosition();

	//check if swap causes a combo
	checkPossipleCombosOnBoard();
}

/*
	Check any possible combos on board.It checks for horizontal and vertical.
*/
void SceneController::checkPossipleCombosOnBoard()
{
	//BE sure that board has no gap
	std::set<DrawableEntity*>* possibleCombos = new std::set<DrawableEntity*>();
	DrawableEntity* entity = NULL;

	for (int y = 0; y < GAME_MATRIX_SIZE_Y; y++)
	{
		for (int x = 0; x < GAME_MATRIX_SIZE_X - 2; x++)
		{
			entity = gameMatrix[x][y];

			if (entity == NULL || gameMatrix[x + 1][y] == NULL || gameMatrix[x + 2][y] == NULL) continue;

			//next object and next to next object have same colors to insert them as combo and check if there are additional objects.
			if (gameMatrix[x + 1][y]->tex == entity->tex && gameMatrix[x + 2][y]->tex == entity->tex)
			{
				possibleCombos->insert(gameMatrix[x][y]);
				possibleCombos->insert(gameMatrix[x + 1][y]);
				possibleCombos->insert(gameMatrix[x + 2][y]);

				if (x + 3 >= GAME_MATRIX_SIZE_X) //check limits 
					break;

				int nextIndex = x + 3;
				if (gameMatrix[nextIndex][y] == NULL)  continue; 
				while (nextIndex < GAME_MATRIX_SIZE_X &&  gameMatrix[nextIndex][y]->tex == entity->tex) //check if there are other same colors
				{
					possibleCombos->insert(gameMatrix[nextIndex][y]);
					nextIndex++;
				}
				x = nextIndex;
			}
		}
	}
	//check for vertical a well
	for (int x = 0; x < GAME_MATRIX_SIZE_X; x++)
	{
		for (int y = 0; y < GAME_MATRIX_SIZE_Y - 2; y++)
		{
			entity = gameMatrix[x][y];

			if (entity == NULL || gameMatrix[x][y + 1] == NULL || gameMatrix[x][y + 2] == NULL) continue;


			if (gameMatrix[x][y + 1]->tex == entity->tex && gameMatrix[x][y + 2]->tex == entity->tex)
			{
				possibleCombos->insert(gameMatrix[x][y]);
				possibleCombos->insert(gameMatrix[x][y + 1]);
				possibleCombos->insert(gameMatrix[x][y + 2]);

				if (y + 3 >= GAME_MATRIX_SIZE_Y)
					break;

				int next = y + 3;
				while (next < GAME_MATRIX_SIZE_Y && gameMatrix[x][next]->tex == entity->tex)
				{
					possibleCombos->insert(gameMatrix[x][next]);
					next++;
				}
				y = next;
			}
		}
	}

	if (possibleCombos->size() > 0)
	{
		removeComboItems(possibleCombos);
	}	
}

/*
 Detects empty slots and initiate move animations for all board
*/
void SceneController::collapseBoard()
{
	//to hold a coordinate which is empty but still it has animation
	//as when we start to animate a object as moving its coordinates shows still old ones
	map<string , int> tempEmtpyCoordinate;
	vector<Animation*> animations; //animations for collapse
	static int t = 0;

	//first empty the array which holds how many empty slots will be filled from top after collapse
	for (int temp = 0; temp < GAME_MATRIX_SIZE_X; temp++)
	{
		coordinatesToNull[temp] = 0;
	}

	//check all empty slots and decide which column will have how many gaps at top
	for (int i = 0; i < GAME_MATRIX_SIZE_X; i++)
	{		
		for (int j = 0; j < GAME_MATRIX_SIZE_Y; j++)
		{
			if (gameMatrix[i][j] == NULL)
			{
				coordinatesToNull[i] += 1;
			}
		}
	}

	for (int y = GAME_MATRIX_SIZE_Y - 1; y >= 0; --y)
	{
		for (int x = 0; x < GAME_MATRIX_SIZE_X; x++)
		{
			ostringstream tx;  //to convert from integer to str
			ostringstream ty; //to convert from integer to str
			tx << x;
			ty << y;
			string key = tx.str() + ty.str();
			if (gameMatrix[x][y] == NULL || tempEmtpyCoordinate.find(key) != tempEmtpyCoordinate.end())
			{ 
				//go up and find a object to collapse there
				int index = y -1;
				while (index >= 0)
				{
					ostringstream target_y;
					target_y << index;
					if (gameMatrix[x][index] != NULL && tempEmtpyCoordinate.find(tx.str()+target_y.str()) == tempEmtpyCoordinate.end())
					{
						Animation* collapse_anim = new Animation(gameMatrix[x][index], MOVE, x, y);
						animations.push_back(collapse_anim);						
						tempEmtpyCoordinate.insert(std::pair<string, int>(tx.str() + target_y.str(), 1));
						SDL_Point from = { x, index };
						SDL_Point to = { x, y };
						SwapData data = { from, to };
						swaps.push(data);
						break;
					}
					index--;
				} 
			}
		}
	}

	if (animations.size() > 0)
	{
		animationController->addAnimation(&animations);
		moveFlag = true;
		t++;
	}
	else{
		//check if there was combo on top as it cause no animation to collapse but still need to be filled
		for (int i = 0 ; i < GAME_MATRIX_SIZE_X; i++)
		{
			if (coordinatesToNull[i] != 0)
			{
				fillFlag = true;
			}
		}
	}
}
/*

	fills empty slots from top
*/
void SceneController::fillEmptySlots()
{
	for (int y = 0; y < GAME_MATRIX_SIZE_Y; y++)
	{
		for (int x = 0; x < GAME_MATRIX_SIZE_X; x++)
		{
			if (gameMatrix[x][y] == NULL)
			{
				GAME_TEX color = getRandomObj();
				DrawableEntity* e = new DrawableEntity(x, y, color, true);
				gameMatrix[x][y] = e;
			}
		}
	}
	//after filling we can still have possible combos with our new added items to top so check them
	checkPossipleCombosOnBoard();	
}

void  SceneController::onMouseMotion(SDL_Event*event)
{
	if ((event->motion.state & SDL_BUTTON_LMASK) && selectedEntity != NULL)
	{
		SDL_Rect* r = selectedEntity->getTargetRect();
		int new_x = r->x + event->motion.xrel;
		int new_y = r->y + event->motion.yrel;
		selectedEntity->setTargetRectX(new_x);
		selectedEntity->setTargetRectY(new_y);
	}
}

void SceneController::onMouseButtonDown(SDL_Event* event)
{
	SDL_Point p;
	p.x = event->button.x;
	p.y = event->button.y;
	selectedEntity = pickEntity(p);
}

void SceneController::onMouseButtonUp(SDL_Event* event)
{
	if (selectedEntity == NULL) return;
	SDL_Point t;
	t.x = event->button.x;
	t.y = event->button.y;

	//pick the target entity
	DrawableEntity* targetEntity = pickEntity(t);
	
	/**
	*	1 -  If no target entity or target entity is the same with selected entity then it is invalid
	*	2 -  If target entity is far more than 1 unit to selected entity it is invalid
	*	3 -  If move does not result with a combo then it is invalid
	*/
	if (targetEntity == NULL || targetEntity != NULL && targetEntity == selectedEntity ||
		(abs(targetEntity->x - selectedEntity->x) > 1 || abs(targetEntity->y - selectedEntity->y) > 1) ||
		checkIfValidMove(selectedEntity, targetEntity) == false)
	{
		selectedEntity->recalculatePosition(); //calculate its original position to return back.
		selectedEntity = NULL;
		targetEntity = NULL;
		return;
	}

	//it is a valid move so perform it
	performMove(selectedEntity, targetEntity);
}

void SceneController::renderScene()
{	
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(renderer);	
	
	SDL_RenderCopyEx(renderer, background, NULL, NULL, 0.0, NULL, SDL_FLIP_NONE);
	SDL_RenderCopyEx(renderer, scoreTexture, NULL, &scoreTextRect, 0.0, NULL, SDL_FLIP_NONE);
	SDL_RenderCopyEx(renderer, timeTexture, NULL, &timeTextRect, 0.0, NULL, SDL_FLIP_NONE);
	
	for (int y = 0; y < GAME_MATRIX_SIZE_Y; y++)
	{
		for (int x = 0; x < GAME_MATRIX_SIZE_X; x++)
		{
			if (gameMatrix[x][y] != NULL)
			{
				gameMatrix[x][y]->render(renderer);
			}
		}
	}
	if (gameOver)
	{
		gameoverEnt->render(renderer);
	}
	SDL_RenderPresent(renderer);

}

void SceneController::setAnimationController(AnimationController* ac)
{
	animationController = ac;
}
/*
	Generate levels.Create such objects that wont cause any combo at the beginning
*/
void SceneController::generateLevel()
{
	GAME_TEX obj;
	for (int i = 0; i < GAME_MATRIX_SIZE_X; i++)
	{
		for (int y = 0; y < GAME_MATRIX_SIZE_Y; y++)
		{
			int color;
			do{
				//color = rand() % 5 + 1; //exclude background
				obj = getRandomObj();
			} while ((i >= 2 && gameMatrix[i - 1][y]->tex == obj && gameMatrix[i - 2][y]->tex == obj)
				|| (y >= 2 && gameMatrix[i][y - 1]->tex == obj && gameMatrix[i][y - 2]->tex == obj));

			//GAME_TEX c = static_cast<GAME_TEX>(color);
			DrawableEntity* ent = new DrawableEntity(i, y, obj, true);
			//DrawableEntity* ent = new DrawableEntity(i, y, obj, true);
			gameMatrix[i][y] = ent;
			addToScene(ent);
		}
	}
}
/*
	Starts remove animations for the entities will be removed from board
*/
void SceneController::removeComboItems(std::set<DrawableEntity*>* combos)
{
	if (comboItems.size() > 0)
	{
		comboItems.clear();
	}
	std::set<DrawableEntity*>::iterator it = combos->begin();
	for (; it != combos->end(); it++)
	{
		updateScore(1);
		DrawableEntity* ent = (*it);
		Animation* anim = new Animation(ent, DESTROY);
		animationController->addAnimation(anim);
		comboItems.insert(ent); //save them to make them null after animation completed		
	}
	if (comboItems.size() > 0) //in case called somewhere else
	{
		collapseFlag = true;		
	}
}

void SceneController::cleanup()
{
	for (int i = 0; i < GAME_MATRIX_SIZE_X; i++)
	{
		for (int y = 0; y < GAME_MATRIX_SIZE_Y; y++)
		{
			if (gameMatrix[i][y])
			{
				delete gameMatrix[i][y];
			}
		}
	}

	//Destroy window
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(gameWindow);
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();
}

//install fonts etc to write on screem
bool SceneController::initText()
{
	//Loading success flag
	bool success = true;

	//Open the font
	gFont = TTF_OpenFont("Aller_Rg.ttf", 28);
	if (gFont == NULL)
	{
		printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
		success = false;
	}
	else
	{
		SDL_Color textColor = { 0, 0, 0 };
		updateScore(0, textColor);
		updateTime();
		success = true;
	}
	return success;
}

//updates game score.Increment game score as much as incScore parameter
bool SceneController::updateScore(int incScore){
	SDL_Color textColor = { 0, 0, 0 };
	return updateScore(incScore, textColor);
}
bool SceneController::updateScore(int incScore, SDL_Color textColor)
{
	ostringstream scr;
	score += incScore;
	scr << score;
	std::string scor = "Score:" + scr.str();
	scoreSurface = TTF_RenderText_Solid(gFont, scor.c_str(), textColor);
	if (scoreSurface == NULL)
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}
	else
	{
		//Create texture from surface pixels
		scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
		if (scoreTexture == NULL)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			//Get image dimensions
			scoreTextRect.w = scoreSurface->w;
			scoreTextRect.h = scoreSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface(scoreSurface);
	}

	//Return success

	return scoreTexture != NULL;
}
bool SceneController::updateTime(){
	SDL_Color textColor = { 0, 0, 0 };
	return updateTime(textColor);
}
bool SceneController::updateTime( SDL_Color color){

	Uint32 passed = (SDL_GetTicks() - gameStartTime) / 1000;
	Uint32 elapsed = GAME_DURATION - passed;
	ostringstream t;
	t << elapsed;
	std::string prefix = "";
	if (elapsed < 10)
	{
		prefix += "00:0";
		color = {255,0,0};
	}
	else
	{
		prefix += "00:";
	}
	std::string scor = "Time Left:" + prefix + t.str();
	timeSurface = TTF_RenderText_Solid(gFont, scor.c_str(), color);
	if (timeSurface == NULL)
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}
	else
	{
		//Create texture from surface pixels
		timeTexture = SDL_CreateTextureFromSurface(renderer, timeSurface);
		if (timeTexture == NULL)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			//Get image dimensions
			timeTextRect.w = timeSurface->w;
			timeTextRect.h = timeSurface->h;
			timeTextRect.x = GAME_WITDH - timeSurface->w;
			timeTextRect.y = 0;
		}

		//Get rid of old surface
		SDL_FreeSurface(timeSurface);
	}

	//Return success

	return timeTexture != NULL;
}
void SceneController::setGameStartTime(Uint32 t ){
	gameStartTime = t;
}

bool SceneController::checkIfTimeIsUp()
{
	Uint32  current = SDL_GetTicks();
	Uint32 passed_seconds = (current - gameStartTime) / 1000;
	return (passed_seconds > GAME_DURATION);
}

bool SceneController::displayGameOver(){
	
	gameoverEnt = new DrawableEntity(GAMEOVER);

	gameoverEnt->setTargetRectX((GAME_WITDH - GAMEOVER_WIDTH) / 2);
	gameoverEnt->setTargetRectY((GAME_HEIGHT - GAMEOVER_HEIGHT) / 2);
	gameoverEnt->setTargetRectW(GAMEOVER_WIDTH);
	gameoverEnt->setTargetRectH(GAMEOVER_HEIGHT);
	return true;
}