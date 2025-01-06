#pragma once
#include "SDL.h"
#include "SDL_image.h"	
#include <vector>
#include <iostream>

class Game
{
public:
	Game();
	~Game();

	void init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);

	void handleEvents();
	void advanceTrial(int success);
	void update();
	void render();
	void clean();

	bool running() { return isRunning; }

private:
	// interval for saving mouse data 
	// 10ms = 100Hz
	int sampling_interval_ms = 10;
	Uint32 last_sample_time = -1;

	Uint32 deadlineTimer = 0;

	// track the upwards movement
	int continousUpwardsMovement = 0;

	// track the phase for a single trial
	int trialPhase = 1;
	Uint32 phase1Deadline = 1500;
	Uint32 phase2Deadline = 1500;
	Uint32 phase3Deadline = 2000;

	//Information about trialCount and trial conditions
	bool isPracticeTrial = true;
	bool hasDeadline = true;
	bool hasFeedback = true;
	int trialCount = 1;

	//information about state of the stimulus
	bool isLocationLeft = true;
	bool isArrowLeft = true;

	bool isRunning;
	SDL_Window *window;
	SDL_Renderer *renderer;
};

