#pragma once
#include "GameEntity.h"
#include "SDL.h"
#include "Common.h"

/*
	Special kinds of entity  which can be drawn on screen
	It has a texture and position on screen.
*/
class DrawableEntity :
	public GameEntity
{
public:
	DrawableEntity();
	DrawableEntity(GAME_TEX tex);				//we only create with texture type.No position information.Can be calculated later.
	DrawableEntity(int x, int y, GAME_TEX tex);	//creates a game entity in given coordinates with given texture
	DrawableEntity(int x, int y, GAME_TEX tex, bool isDraggable);
	int x;	//x coordinate of the entity on grid
	int y; //y coordinate of the entity on grid
	GAME_TEX tex; //textrue type of game entity (RED, BLUE, BACKGROUND etc)
	 
	virtual  ~DrawableEntity(){};
	virtual void render(SDL_Renderer*) ;	//renders entity on screen
	void setTargetRectX(int x);
	void setTargetRectY(int y);
	void setTargetRectW(int w);
	void setTargetRectH(int h);
	void recalculatePosition();				//considering its x and y coordinates, it recalculates the position on screeen
	SDL_Rect* const getTargetRect() const;
	bool isDraggable;

private:	
	SDL_Rect* targetRect = NULL;		//x,y,w,h for the textrue to be drawn on screen
	SDL_Texture* entityTex = NULL;		//original texture for the game obj
	
};

