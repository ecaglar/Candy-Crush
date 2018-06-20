#include "AnimationController.h"


AnimationController::AnimationController()
{
	animationQueue = new std::vector<Animation*>();
}
AnimationController::~AnimationController()
{
}
void AnimationController::tick()
{
	std::vector<Animation*>::iterator i = animationQueue->begin();
	while (i != animationQueue->end())
	{
		(*i)->tick();		
		i++;
		
	}
	std::vector<Animation*>::iterator it = animationQueue->begin();

	for (it = animationQueue->begin(); it != animationQueue->end(); )
	{

		if ((*it)->isCompleted())
		{
			it = animationQueue->erase(it);
		}
		else{
			++it;
		}
	}
}
void AnimationController::addAnimation(Animation* anim)
{
	Uint32 t = SDL_GetTicks();
	anim->setAnimationStartTime(t);
	animationQueue->push_back(anim);

}
bool AnimationController::isAnimating(DrawableEntity* ent)
{
	std::vector<Animation*>::iterator i = animationQueue->begin();
	for (; i != animationQueue->end(); i++)
	{
		if ((*i)->getSourceObj() == ent)
			return true;
	}
	return false;
}
bool AnimationController::checkAnyAnimation()
{
	return (animationQueue->size() != 0);
}

int AnimationController::getAnimationCount()
{
	return animationQueue->size();
}

void AnimationController::addAnimation(std::vector<Animation*>* anims)
{
	std::vector<Animation*>::iterator i = anims->begin();
	for (; i != anims->end(); i++)
	{
		addAnimation(*i);
	}
}