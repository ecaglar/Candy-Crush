#pragma once
#include <vector>
#include "Animation.h"

/*
	Controller class for animation.Responsible for starting and updating animations.
	Only aniamtions added to animation controller can be updated.On every update it updates
	all its animations as well.Also we can query if there is any ongoing aniamtion etc.
*/
class AnimationController
{
public:
	AnimationController();
	~AnimationController();

	void tick();								//global tick for animations.It updates all aniamtions
	void addAnimation(Animation*);				//add animation to controller.When an animation is added it starts immediately
	void addAnimation(std::vector<Animation*>*);//to support multiple animation adding at once.Useful for block animations like combos
	bool isAnimating(DrawableEntity*);			//checks if given object is animating
	bool checkAnyAnimation();					//check if there is any ongoing aniamtion on screen
	int getAnimationCount();					//returns how many animations currently playing
private:
	std::vector<Animation*> * animationQueue;	//animation queue. 

};

