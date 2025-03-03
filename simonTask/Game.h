#pragma once
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"	
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
	// Change These per participant
	// condition: 0 = with latency in 1 & 2 Block; 1 = with latency in 3 & 4 Block
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
	void sampleMouseData();

	// Setters
	void setExperimentalCondition(int condition) { experimentalCondition = condition; }
	void setParticipantId(int participantId_arg) { participantId = participantId_arg; }
	void setLatency(int latency_arg) { latency = latency_arg; }

	bool running() { return isRunning; }

private:
	bool gamePaused = false;
	bool startRealScreen = false;

	int experimentalCondition = 1;
	int participantId = 14;
	int latency = 80;

	// interval for saving mouse data 
	// 10ms = 100Hz
	int sampling_interval_ms = 5;
	Uint32 last_sample_time = -1;
	Uint32 reaction_time = 0;

	Uint32 deadlineTimer = 0;
	Uint32 feedbackTimer = 0;

	std::vector<Trial> shuffledTrials;

	// track the upwards movement
	int continousUpwardsMovement = 0;
	int initialY = -1;
	Uint32 initialTime = 0;
	bool upwardDetected = false;

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
	int practiceBlockSize = 40;
	int practiceBockDeadlineCutoff = 10;
	int practiceBlockFeedbackCutoff = 20;

	// which block is it currently
	int experimentalBlockCount = 1;
	// number of trials per block
	int experimentalBlockSize = 320;
	// how many blocks are the max
	int experimentalBlockNum = 4;

	bool gameFinished = false;

	bool deadlineError = false;
	Uint32 deadlineErrorDuration = 100;
	Uint32 deadlineErrorTimer = 0;

	bool instructions = true;
	int instructionsSlide = 1;

	std::ofstream mouseDataFile;	
	std::ofstream trialDataFile;


	//information about state of the stimulus
	Trial currentTrial;
	Trial previousTrial;

	bool showLatencyTrackingBox = false;

	bool isRunning;
	SDL_Window *window;
	SDL_Renderer *renderer;
};

