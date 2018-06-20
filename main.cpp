#include "Engine.h"

int main(int argc, char* args[])
{
	Engine* engine = new Engine();
	engine->initContollers();
	engine->initGame();	
	engine->startGame();
	return 0;
}