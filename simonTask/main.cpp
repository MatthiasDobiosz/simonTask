#include "Game.h"

Game* game = nullptr;

int main(int argc, char* argv[])
{
	game = new Game();
	std::cout << argc << std::endl;

	//check for sysargs and use them for condition values if given
	if (argc == 4) {
		game->setExperimentalCondition(std::atoi(argv[1]));  
		game->setParticipantId(std::atoi(argv[2])); 
		game->setLatency(std::atoi(argv[3]));   
		std::cout << std::atoi((argv[3])) << std::endl;
		std::cout << "Using sysargs" << std::endl;
	}
	else {
		std::cout << "Using default values" << std::endl;
	}

	game->init("Task", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 1080, true);

	// run the game loop
	while (game->running()) {
		game->handleEvents();
		game->update();
		game->render();
	}

	game->clean();

	return 0;
}
