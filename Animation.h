#pragma once
#include "DrawableEntity.h"

enum ANIMATION_TYPE { MOVE , DESTROY };

/*
	Animation object for a drawable entity.
	Currently two kinds of aniamtion supported which are destroy and move
	Animation is time based which means each animation type has a time in which the have to be completed
	and those times are defiend in common.h header.
	So for example considering move, if a game entity has more way to go then it goes faster to be compeleted in time.
*/
class Animation
{
public:
	Animation();
	Animation(DrawableEntity* obj, ANIMATION_TYPE type);
	Animation(DrawableEntity* obj, ANIMATION_TYPE type, int target_x, int target_y);
	~Animation();

	void tick();					//update animation state
	bool isCompleted();				//checks if animation completed
	DrawableEntity* getSourceObj();	
	void setAnimationStartTime(Uint32);

private:
	DrawableEntity*		source;			//source object for the animation
	ANIMATION_TYPE		type;			//animation type
	int					target_x_coor;	//target x position for animation
	int					target_y_coor;	//target y position for animation
	SDL_Rect			targetRect;		
	SDL_Rect			originalRect;	//original position of the object 
	bool				completed;
	Uint32				startTime;

	void calculateTargetState();		//calculate the target position using animation type
};

