#include "DrawableEntity.h"
#include "ResourceController.h"

DrawableEntity::DrawableEntity()
{
	x = 0;
	y = 0;
	targetRect = new SDL_Rect();
	entityTex = NULL;
	isDraggable = true;
}
DrawableEntity::DrawableEntity(GAME_TEX obj)
{
	x = 0;
	y = 0;
	targetRect = new SDL_Rect();
	SDL_Texture* t = ResourceController::getResource(obj);
	entityTex = t;
	tex = obj;
	isDraggable = true;
}
DrawableEntity::DrawableEntity(int x, int y, GAME_TEX obj)
{
	this->x = x;
	this->y = y;
	targetRect = new SDL_Rect();
	calculateRenderPositionFromCoordinate(x, y, targetRect);
	SDL_Texture* t = ResourceController::getResource(obj);
	entityTex = t;
	tex = obj;
	isDraggable = true;
}
DrawableEntity::DrawableEntity(int x, int y, GAME_TEX obj, bool isDraggable)
{
	this->x = x;
	this->y = y;
	this->isDraggable = isDraggable;
	targetRect = new SDL_Rect();
	calculateRenderPositionFromCoordinate(x, y, targetRect);
	SDL_Texture* t = ResourceController::getResource(obj);
	entityTex = t;
	tex = obj;
	isDraggable = true;
}

void DrawableEntity::render(SDL_Renderer* renderer)
{
	if (entityTex != NULL)
	{
		SDL_RenderCopyEx(renderer, entityTex, NULL, isDraggable ? targetRect : NULL, 0.0, NULL, SDL_FLIP_NONE);
	}
}

SDL_Rect* const DrawableEntity::getTargetRect() const { return targetRect; } 

void DrawableEntity::recalculatePosition()
{
	calculateRenderPositionFromCoordinate(x, y, targetRect);
}

void DrawableEntity::setTargetRectW(int w)
{
	targetRect->w = w;
}
void DrawableEntity::setTargetRectH(int h)
{
	targetRect->h = h;
}
void DrawableEntity::setTargetRectX(int x){
	if (targetRect != NULL)
	{
		targetRect->x = x;
	}
}
void DrawableEntity::setTargetRectY(int y){
	if (targetRect != NULL)
	{
		targetRect->y = y;
	}
}