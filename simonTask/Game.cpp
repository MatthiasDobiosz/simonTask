
#include <iostream>
#include "Game.h"

struct MouseData {
	int trialCount;
	Uint32 timestamp;
	int x, y;
};

struct TrialData {
	int trialCount;
	int success;
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

std::vector<TrialData> trial_data;

Game::Game()
{}
Game::~Game()
{}

bool isPointInRect(int x, int y, const SDL_Rect& rect) {
	return x >= rect.x && x <= rect.x + rect.w &&
		y >= rect.y && y <= rect.y + rect.h;
}

std::vector<Trial> generateMatrix(int repetitionsPerCombination) {
	std::vector<Trial> trials;

	for (int currentCongruent = 0; currentCongruent <= 1; ++currentCongruent) {
		for (int previousCongruent = 0; previousCongruent <= 1; ++previousCongruent) {
			for (int stimulusDirection = 0; stimulusDirection <= 1; ++stimulusDirection) {
				for (int stimulusPosition = 0; stimulusPosition <= 1; ++stimulusPosition) {
					for (int i = 0; i < repetitionsPerCombination; ++i) {
						trials.push_back({ static_cast<bool>(currentCongruent),
										  static_cast<bool>(previousCongruent),
										  static_cast<bool>(stimulusDirection),
										  static_cast<bool>(stimulusPosition) });
					}
				}
			}
		}
	}

	return trials;
}

void shuffleMatrix(std::vector<Trial>& trials) {
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(trials.begin(), trials.end(), g);
}

void addPreviousConiditonsToMatrix(std::vector<Trial>& trials)
{
	for (int i = 0; i < trials.size(); i++)
	{
		if (i != 0)
		{
			trials[i].previousCongruent = trials[i - 1].currentCongruent;
		}
	}
}

void printMatrix(const std::vector<Trial>& trials) {
	for (const auto& trial : trials) {
		std::cout << "Current: " << (trial.currentCongruent ? "Congruent" : "Incongruent")
			<< ", Previous: " << (trial.previousCongruent ? "Congruent" : "Incongruent")
			<< ", Direction: " << (trial.stimulusDirection ? "Right" : "Left")
			<< ", Position: " << (trial.stimulusPosition ? "Right" : "Left") << '\n';
	}
}

bool isCorrectResponse(std::string direction, Trial trial)
{
	if (direction == "right") {
		if (trial.stimulusDirection)
		{
			return true;
		}
		return false;
	}
	else if (direction == "left")
	{
		if (!trial.stimulusDirection)
		{
			return true;
		}
		return false;
	}

	return false;
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

	auto trials = generateMatrix(1);
	shuffleMatrix(trials);
	addPreviousConiditonsToMatrix(trials);
	shuffledTrials = trials;
	currentTrial = shuffledTrials[0];

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

void Game::advanceTrial(int success)
{
	deadlineTimer = SDL_GetTicks();
	trialCount++;
	trialPhase = 1;

	currentTrial = shuffledTrials[trialCount];

	if (trialCount == 10)
	{
		hasDeadline = true;
		deadlineTimer = SDL_GetTicks();
	}

	if (trialCount == 20)
	{
		hasFeedback = false;
	}

	// potentially: reset mouse position when trial fails/succeeds
	//SDL_WarpMouseInWindow(window, 960, 960);

	trial_data.push_back({ trialCount, success });
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
				deadlineTimer = SDL_GetTicks();
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
						deadlineTimer = SDL_GetTicks();
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

					mouse_data.push_back({ trialCount, current_time, event.motion.x, event.motion.y });

					last_sample_time = current_time;
				}

				// user is in responds box
				if (isPointInRect(event.button.x, event.button.y, blackBoxLeftDestR))
				{
					if (isCorrectResponse("left", currentTrial))
					{
						std::cout << "correct" << std::endl;
						advanceTrial(1);
					}
					else {
						std::cout << "incorrect" << std::endl;
						advanceTrial(0);
					}
				
				}
				if (isPointInRect(event.button.x, event.button.y, blackBoxRightDestR))
				{
					if (isCorrectResponse("right", currentTrial))
					{
						std::cout << "correct" << std::endl;
						advanceTrial(1);
					}
					else {
						std::cout << "incorrect" << std::endl;
						advanceTrial(0);
					}
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

	if (hasFeedback)
	{
		//TODO: implement feedback
		//probably need to stop timer for deadlines etc. when feedback is shown
	}

	// reset timer and save failed trial if user takes too long
	if (hasDeadline)
	{
		Uint32 timeDiff = SDL_GetTicks() - deadlineTimer;
		
		if (trialPhase == 1 && timeDiff > phase1Deadline || trialPhase == 2 && timeDiff > phase2Deadline || trialPhase == 3 && timeDiff > phase3Deadline)
		{
			advanceTrial(0);
		}
	}

	if (trialCount == 400)
	{
		//TODO: End game when all trials done
	}
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
		if (currentTrial.stimulusPosition)
		{
			if (currentTrial.stimulusDirection)
			{
				SDL_RenderCopy(renderer, arrowRightTex, NULL, &arrowRightDestR);
			}
			else {
				SDL_RenderCopy(renderer, arrowLeftTex, NULL, &arrowRightDestR);
			}
		}
		else {
			if (currentTrial.stimulusDirection)
			{
				SDL_RenderCopy(renderer, arrowRightTex, NULL, &arrowLeftDestR);
			}
			else {
				SDL_RenderCopy(renderer, arrowLeftTex, NULL, &arrowLeftDestR);
			}
		}

		SDL_RenderCopy(renderer, blackBoxTex, NULL, &blackBoxLeftDestR);
		SDL_RenderCopy(renderer, blackBoxTex, NULL, &blackBoxRightDestR);
	}
	SDL_RenderPresent(renderer);
}

void Game::clean()
{
	std::cout << "Mouse Data Collected:\n";
	//TODO: Save data at the end

	//printMatrix(shuffledTrials);
	/*
	for (const auto& data : mouse_data) {
		std::cout << "Time: " << data.trialCount << data.timestamp << " ms, X: " << data.x << ", Y: " << data.y << "\n";
	}
	*/


	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	std::cout << "Game cleaned!" << std::endl;
}