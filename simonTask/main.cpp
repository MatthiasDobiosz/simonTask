#include "Game.h"

Game* game = nullptr;

int main(int argc, char* argv[])
{
	game = new Game();

	if (argc == 3) {
		game->setExperimentalCondition(std::atoi(argv[1]));  
		game->setParticipantId(std::atoi(argv[2])); 
		game->setLatency(std::atoi(argv[3]));   
		std::cout << "Using sysargs" << std::endl;
	}
	else {
		std::cout << "Using default values" << std::endl;
	}

	game->init("Task", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 1080, true);

	while (game->running()) {
		game->handleEvents();
		game->update();
		game->render();
	}

	game->clean();

	return 0;
}
