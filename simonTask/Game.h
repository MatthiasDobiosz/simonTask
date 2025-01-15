#pragma once
#include "SDL.h"
#include "SDL_image.h"	
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <random>
#include <string>

struct Trial {
	bool currentCongruent;
	bool stimulusDirection;
	bool stimulusPosition;
	bool isFirst = false;
};

class Game
{
public:
	Game();
	~Game();

	void init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);

	void handleEvents();
	void generateAndShuffleMatrix(int matrixBlockSize);
	void advanceTrial(int success);
	void update();
	void render();
	void clean();
	void saveData();

	bool running() { return isRunning; }

private:
	// interval for saving mouse data 
	// 10ms = 100Hz
	int sampling_interval_ms = 10;
	Uint32 last_sample_time = -1;
	Uint32 reaction_time = 0;

	Uint32 deadlineTimer = 0;
	Uint32 feedbackTimer = 0;

	std::vector<Trial> shuffledTrials;

	// track the upwards movement
	int continousUpwardsMovement = 0;

	// track the phase for a single trial
	int trialPhase = 1;
	Uint32 phase1Deadline = 1500;
	Uint32 phase2Deadline = 1500;
	Uint32 phase3Deadline = 2000;

	//Information about trialCount and trial conditions
	bool hasDeadline = false;
	bool hasFeedback = true;
	int trialCount = 1;
	bool isFeedbackDisplayed = false;

	bool isPracticeBlock = true;
	int practiceBlockSize = 4;
	int practiceBockDeadlineCutoff = 2;
	int practiceBlockFeedbackCutoff = 3;

	int experimentalBlockCount = 1;
	int experimentalBlockSize = 10;
	int experimentalBlockNum = 1;

	int lastSave = 1;
	std::ofstream mouseDataFile;	
	std::ofstream trialDataFile;


	//information about state of the stimulus
	Trial currentTrial;
	Trial previousTrial;

	bool isRunning;
	SDL_Window *window;
	SDL_Renderer *renderer;
};

