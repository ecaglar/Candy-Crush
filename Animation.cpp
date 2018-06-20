#include "Animation.h"
#include "Common.h"
#include <iostream>

Animation::~Animation(){}
Animation::Animation(DrawableEntity* obj, ANIMATION_TYPE type)
{
	source = obj;
	this->type = type;
	calculateTargetState();
	completed = false;

}
Animation::Animation(DrawableEntity* obj, ANIMATION_TYPE type, int target_x, int target_y)
{
	source = obj;
	this->type = type;
	this->target_x_coor = target_x;
	this->target_y_coor = target_y;
	completed = false;
	calculateTargetState();
}

void Animation::calculateTargetState()
{
	calculateRenderPositionFromCoordinate(target_x_coor, target_y_coor, &targetRect);
	//if it is destroy animation then it will be dissepear on screen so that its w and h wll go to 0 over time
	if (type == DESTROY)
	{
		targetRect.x = source->getTargetRect()->x + STONE_WIDTH / 2;
		targetRect.y = source->getTargetRect()->y + STONE_HEIGHT / 2;
		targetRect.h = 0;
		targetRect.w = 0;
	}
	else if (type == MOVE)
	{
		SDL_Rect r;
		calculateRenderPositionFromCoordinate(target_x_coor, target_y_coor, &r);
		targetRect.x = r.x;
		targetRect.y = r.y;
		targetRect.h = source->getTargetRect()->h;
		targetRect.w = source->getTargetRect()->w;

	}
	//save the original position of the objects so that they will be calculated for calculations
	originalRect.x = source->getTargetRect()->x;
	originalRect.y = source->getTargetRect()->y;
	originalRect.w = source->getTargetRect()->w;
	originalRect.h = source->getTargetRect()->h;	
}
/*
	Updates the animation.
*/
void Animation::tick()
{
	int new_x = 0;
	int new_y = 0;
	int new_w = 0;
	int new_h = 0;
	int animDuration = 0;

	//get the animation duration according to animation type
	if (type == DESTROY) animDuration = DESTROY_ANIMATION_TIME;
	if (type == MOVE) animDuration = MOVE_ANIMATION_TIME;

	//calculate elapsed time
	Uint32	elapsedTime = SDL_GetTicks() - startTime;

	//if elapsed time bigger then animation duration then we should finish the aniamtion
	if (elapsedTime >= animDuration) //time is up!
	{
		new_x = targetRect.x;
		new_y = targetRect.y;
		new_w = targetRect.w;
		new_h = targetRect.h;
	}
	else{
		//calculate the positions according to time passed 		
		float dt = (float)elapsedTime / (float)animDuration;
		new_x = originalRect.x + ((targetRect.x -  originalRect.x) * dt);
		new_y = originalRect.y + ((targetRect.y - originalRect.y) * dt);
		new_h = originalRect.h + ((targetRect.h - originalRect.h) * dt);
		new_w = originalRect.w + ((targetRect.w - originalRect.w) * dt);
	}
	//update object positions with calculated ones
	source->setTargetRectX(new_x);
	source->setTargetRectY(new_y);
	source->setTargetRectW(new_w);
	source->setTargetRectH(new_h);

}
/*
	check if object reached to target position which means aniamtion completed
*/
bool Animation::isCompleted(){
	return (source->getTargetRect()->x == targetRect.x  && source->getTargetRect()->y == targetRect.y &&
			source->getTargetRect()->w == targetRect.w && source->getTargetRect()->h == targetRect.h);
}

DrawableEntity* Animation::getSourceObj()
{
	return source;
}
void Animation::setAnimationStartTime(Uint32 t)
{
	startTime = t;
}