
#include <iostream>
#include "Game.h"

struct MouseData {
	Uint32 timestamp;
	int x, y;
};

SDL_Texture* redBoxTex;
SDL_Rect redBoxDestR;

SDL_Texture* blackBoxTex;
SDL_Rect blackBoxLeftDestR;
SDL_Rect blackBoxRightDestR;

SDL_Texture* arrowLeftTex;
SDL_Rect arrowLeftDestR;

SDL_Texture* arrowRightTex;
SDL_Rect arrowRightDestR;

std::vector<MouseData> mouse_data;

Game::Game()
{}
Game::~Game()
{}

bool isPointInRect(int x, int y, const SDL_Rect& rect) {
	return x >= rect.x && x <= rect.x + rect.w &&
		y >= rect.y && y <= rect.y + rect.h;
}

void Game::init(const char *title, int xpos, int ypos, int width, int height, bool fullscreen)
{
	redBoxDestR.h = 240;
	redBoxDestR.w = 480;
	redBoxDestR.x = 720;
	redBoxDestR.y = 800;

	blackBoxLeftDestR.h = 240;
	blackBoxLeftDestR.w = 480;
	blackBoxLeftDestR.x = 0;
	blackBoxLeftDestR.y = 0;

	blackBoxRightDestR.h = 240;
	blackBoxRightDestR.w = 480;
	blackBoxRightDestR.x = 1440;
	blackBoxRightDestR.y = 0;

	arrowLeftDestR.h = 150;
	arrowLeftDestR.w = 300;
	arrowLeftDestR.x = 200;
	arrowLeftDestR.y = 500;

	arrowRightDestR.h = 150;
	arrowRightDestR.w = 300;
	arrowRightDestR.x = 1420;
	arrowRightDestR.y = 500;

	int flags = 0;
	if (fullscreen) 
	{
		flags = SDL_WINDOW_FULLSCREEN;
	}

	if(SDL_Init(SDL_INIT_EVERYTHING) == 0) 
	{
		std::cout << "Subsystems initialized..." << std::endl;

		window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
		if (window)
		{
			std::cout << "Window created!" << std::endl;
		}

		renderer = SDL_CreateRenderer(window, -1, 0);
		if (renderer)
		{
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			std::cout << "Renderer created!" << std::endl;
		}

		isRunning = true;
	}
	else {
		isRunning = false;
	}

	SDL_Surface* tmpSurfaceRedBox = IMG_Load("assets/Redbox.png");
	redBoxTex = SDL_CreateTextureFromSurface(renderer, tmpSurfaceRedBox);
	SDL_FreeSurface(tmpSurfaceRedBox);

	SDL_Surface* tmpSurfaceBlackBox = IMG_Load("assets/Blackbox.png");
	blackBoxTex = SDL_CreateTextureFromSurface(renderer, tmpSurfaceBlackBox);
	SDL_FreeSurface(tmpSurfaceBlackBox);

	SDL_Surface* tmpSurfaceArrowLeft = IMG_Load("assets/ArrowLeft.png");
	arrowLeftTex = SDL_CreateTextureFromSurface(renderer, tmpSurfaceArrowLeft);
	SDL_FreeSurface(tmpSurfaceArrowLeft);

	SDL_Surface* tmpSurfaceArrowRight = IMG_Load("assets/ArrowRight.png");
	arrowRightTex = SDL_CreateTextureFromSurface(renderer, tmpSurfaceArrowRight);
	SDL_FreeSurface(tmpSurfaceArrowRight);
}

void Game::handleEvents()	
{
	SDL_Event event;
	SDL_PollEvent(&event);

	switch (event.type) {
		case SDL_QUIT:
			isRunning = false;
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (isPointInRect(event.button.x, event.button.y, redBoxDestR) && trialPhase == 1)
			{
				std::cout << "Red box clicked" << std::endl;

				trialPhase = 2;
			}
		case SDL_MOUSEMOTION:
			// handle upwards movement when in Phase 2 
			if (trialPhase == 2)
			{
				if (event.motion.yrel < -3)
				{
					if (continousUpwardsMovement == 0)
					{
						continousUpwardsMovement = 1;
					}
					else if (continousUpwardsMovement == 1) {
						continousUpwardsMovement = 0;
						trialPhase = 3;
					}
				}
				else {
					continousUpwardsMovement = 0;
				}
			}
			// handle tracking of mouse data in Phase 3
			else if (trialPhase == 3) {
				Uint32 current_time = SDL_GetTicks();

				if (current_time - last_sample_time >= sampling_interval_ms) {
					SDL_GetMouseState(&event.motion.x, &event.motion.y);

					mouse_data.push_back({ current_time, event.motion.x, event.motion.y });

					last_sample_time = current_time;
				}
			}
		default: 
			break;
	}
}

void Game::update()
{
	// handle timer/deadlines here: check which phase is currently running, set/reset timer accordingly and fail trial if necessary
	// handle overall trial states (trialCount, trialConditions, feedback)
	// handle getting and setting current conditions for single trial (congruent/incongruent etc)
	// handle success of trial

}

void Game::render()
{
	SDL_RenderClear(renderer);
	if (trialPhase == 1)
	{
		SDL_RenderCopy(renderer, redBoxTex, NULL, &redBoxDestR);
	}
	else if (trialPhase == 2) {
		SDL_RenderCopy(renderer, blackBoxTex, NULL, &blackBoxLeftDestR);
		SDL_RenderCopy(renderer, blackBoxTex, NULL, &blackBoxRightDestR);
	}
	else {
		SDL_RenderCopy(renderer, blackBoxTex, NULL, &blackBoxLeftDestR);
		SDL_RenderCopy(renderer, blackBoxTex, NULL, &blackBoxRightDestR);
		SDL_RenderCopy(renderer, arrowLeftTex, NULL, &arrowLeftDestR);
		SDL_RenderCopy(renderer, arrowRightTex, NULL, &arrowRightDestR);
	}
	SDL_RenderPresent(renderer);
}

void Game::clean()
{
	std::cout << "Mouse Data Collected:\n";
	for (const auto& data : mouse_data) {
		std::cout << "Time: " << data.timestamp << " ms, X: " << data.x << ", Y: " << data.y << "\n";
	}

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	std::cout << "Game cleaned!" << std::endl;
}