
#include <iostream>
#include "Game.h"

struct MouseData {
	int trialCount;
	int blockCount;
	Uint32 timestamp;
	int x, y;
};

struct TrialInformationData {
	int trialCount;
	int blockCount;
	int success;
	Uint32 reactionTime;
	bool currentCongruency;
	bool currentStimulusDirection;
	bool currentStimulusLocation;
	bool previousCongruency;
	bool previousStimulusDirection;
	bool previousStimulusLocation;
};

// define textures and recs for all game objets
SDL_Texture* redBoxTex;
SDL_Rect redBoxDestR;

SDL_Texture* whiteBoxTex;
SDL_Rect whiteBoxLeftDestR;
SDL_Rect whiteBoxRightDestR;

SDL_Texture* arrowLeftTex;
SDL_Rect arrowLeftDestR;

SDL_Texture* arrowRightTex;
SDL_Rect arrowRightDestR;

SDL_Texture* fullscreenTex;
SDL_Rect fullscreenDestR;

SDL_Texture* feedbackTex;
SDL_Texture* backgroundTex;
SDL_Texture* startRealTex;

SDL_Texture* instructionStartTex;
SDL_Texture* instructionStep1Tex;
SDL_Texture* instructionStep2Tex;
SDL_Texture* instructionStep3Tex;
SDL_Texture* instructionEndTex;

SDL_Texture* Block1FinishedTex;
SDL_Texture* Block2FinishedTex;
SDL_Texture* Block3FinishedTex;
SDL_Texture* FinishedTex;

SDL_Texture* ErrorScreenTex;

SDL_Rect latencyTrackingBoxDestR;

std::vector<MouseData> mouse_data;

std::vector<TrialInformationData> trial_information_data;

Game::Game()
{}
Game::~Game()
{}

// check if a point is inside a rectangle
bool isPointInRect(int x, int y, const SDL_Rect& rect) {
	return x >= rect.x && x <= rect.x + rect.w &&
		y >= rect.y && y <= rect.y + rect.h;
}

int getNextMultipleOf16(int number) {
	return (number + 15) / 16;
}

// generate a trial-repition matrix to balance out orders
std::vector<Trial> generateMatrix(int repetitionsPerCombination) {
	std::vector<Trial> trials;
	bool isFirstTrial = true; 

	for (int currentCongruent = 0; currentCongruent <= 1; ++currentCongruent) {
		for (int previousCongruent = 0; previousCongruent <= 1; ++previousCongruent) {
			for (int stimulusDirection = 0; stimulusDirection <= 1; ++stimulusDirection) {
				for (int stimulusPosition = 0; stimulusPosition <= 1; ++stimulusPosition) {
					for (int i = 0; i < repetitionsPerCombination; ++i) {
						trials.push_back({
							static_cast<bool>(currentCongruent),
							static_cast<bool>(stimulusDirection),
							static_cast<bool>(stimulusPosition),
							isFirstTrial 
							});
						isFirstTrial = false;
					}
				}
			}
		}
	}

	return trials;
}

// shuffle matrix for randomization
void shuffleMatrix(std::vector<Trial>& trials) {
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(trials.begin(), trials.end(), g);
}

void printMatrix(const std::vector<Trial>& trials) {
	for (const auto& trial : trials) {
		std::cout << "Congruency: " << (trial.currentCongruent ? "Congruent" : "Incongruent")
			<< ", Direction: " << (trial.stimulusDirection ? "Right" : "Left")
			<< ", Position: " << (trial.stimulusPosition ? "Right" : "Left") << '\n';
	}
}

// check if a given response is correct
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

// save the mouse and trial data
void Game::saveData()
{
	if (mouseDataFile) {
		for (const auto& data : mouse_data) {
			mouseDataFile << data.timestamp << "," << data.trialCount << "," << data.blockCount << "," << data.x << "," << data.y << std::endl;
		}
	}
	else {
		std::cerr << "Error: mousedatafile not open for writing.\n";
	}

	std::cout << "Mouse Data Collected\n";

	if (trialDataFile) {
		for (const auto& data : trial_information_data) {
			trialDataFile << data.trialCount << "," << data.blockCount << "," << data.success << "," << data.reactionTime << "," << data.currentCongruency << "," << data.currentStimulusDirection << "," << data.currentStimulusLocation << "," << data.previousCongruency << "," << data.previousStimulusDirection << "," << data.previousStimulusLocation << std::endl;
		}
	}
	else {
		std::cerr << "Error: trialDatafile not open for writing.\n";
	}

	std::cout << "Trial Data Collected\n";

	mouse_data.clear();
	mouse_data.shrink_to_fit();

	trial_information_data.clear();
	trial_information_data.shrink_to_fit();
}

// init the game 
void Game::init(const char *title, int xpos, int ypos, int width, int height, bool fullscreen)
{
	redBoxDestR.h = 220;
	redBoxDestR.w = 439;
	redBoxDestR.x = 740;
	redBoxDestR.y = 810;

	whiteBoxLeftDestR.h = 220;
	whiteBoxLeftDestR.w = 439;
	whiteBoxLeftDestR.x = 0;
	whiteBoxLeftDestR.y = 0;

	whiteBoxRightDestR.h = 220;
	whiteBoxRightDestR.w = 439;
	whiteBoxRightDestR.x = 1479;
	whiteBoxRightDestR.y = 0;

	arrowLeftDestR.h = 324;
	arrowLeftDestR.w = 300;
	arrowLeftDestR.x = 175;
	arrowLeftDestR.y = 450;

	arrowRightDestR.h = 324;
	arrowRightDestR.w = 300;
	arrowRightDestR.x = 1421;
	arrowRightDestR.y = 450;

	fullscreenDestR.h = 1080;
	fullscreenDestR.w = 1920;
	fullscreenDestR.x = 0;
	fullscreenDestR.y = 0;

	latencyTrackingBoxDestR.h = 100;
	latencyTrackingBoxDestR.w = 100;
	latencyTrackingBoxDestR.x = 0;
	latencyTrackingBoxDestR.y = 980;

	// set latency at the start for condition 0
	if (experimentalCondition == 0) {
		std::string command = "echo \"" + std::to_string(latency) + " " +
			std::to_string(latency) + " " +
			std::to_string(latency) + " " +
			std::to_string(latency) +
			"\" > " + "/tmp/DelayDaemon";

		std::system(command.c_str());
	}

	generateAndShuffleMatrix(getNextMultipleOf16(practiceBlockSize));

	std::string mouseDataFilePath = "data/mouse_data_" + std::to_string(participantId) + ".txt";
	mouseDataFile.open(mouseDataFilePath, std::ios::app);

	if (!mouseDataFile) {
		std::cerr << "Error: Could not open mouse data file for writing.\n";
	}

	mouseDataFile << "timedifference," << "trialnumber," << "blocknumber," << "xpos," << "ypos" << std::endl;

	std::string trialDataFilePath = "data/trial_data_" + std::to_string(participantId) + ".txt";
	trialDataFile.open(trialDataFilePath, std::ios::app);

	if (!trialDataFile) {
		std::cerr << "Error: Could not open trial trial file for writing.\n";
	}

	trialDataFile << "trialnumber," << "blocknumber," << "success," << "reactiontime," << "currentcongruency," << "currentdirection," << "currentlocation," << "previouscongruency," << "previousdirection," << "previouslocation" << std::endl;

	int flags = 0;
	if (fullscreen) 
	{
		flags = SDL_WINDOW_FULLSCREEN;
	}

	if(SDL_Init(SDL_INIT_VIDEO) == 0)
	{
		std::cout << "Subsystems initialized..." << std::endl;

    	//SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
		window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
		if (window)
		{
			std::cout << "Window created!" << std::endl;
		}

		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
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

	SDL_Surface* tmpSurfaceWhiteBox = IMG_Load("assets/Whitebox.png");
	whiteBoxTex = SDL_CreateTextureFromSurface(renderer, tmpSurfaceWhiteBox);
	SDL_FreeSurface(tmpSurfaceWhiteBox);

	SDL_Surface* tmpSurfaceArrowLeft = IMG_Load("assets/ArrowLeft.png");
	arrowLeftTex = SDL_CreateTextureFromSurface(renderer, tmpSurfaceArrowLeft);
	SDL_FreeSurface(tmpSurfaceArrowLeft);

	SDL_Surface* tmpSurfaceArrowRight = IMG_Load("assets/ArrowRight.png");
	arrowRightTex = SDL_CreateTextureFromSurface(renderer, tmpSurfaceArrowRight);
	SDL_FreeSurface(tmpSurfaceArrowRight);

	SDL_Surface* tmpSurfaceFeedback = IMG_Load("assets/feedback.png");
	feedbackTex = SDL_CreateTextureFromSurface(renderer, tmpSurfaceFeedback);
	SDL_FreeSurface(tmpSurfaceFeedback);

	SDL_Surface* tmpSurfaceBackground = IMG_Load("assets/Blackbox.png");
	backgroundTex = SDL_CreateTextureFromSurface(renderer, tmpSurfaceBackground);
	SDL_FreeSurface(tmpSurfaceBackground);
	
	SDL_Surface* tmpSurfaceStartReal = IMG_Load("assets/startReal.png");
	startRealTex = SDL_CreateTextureFromSurface(renderer, tmpSurfaceStartReal);
	SDL_FreeSurface(tmpSurfaceStartReal);

	SDL_Surface* tmpSurfaceInstructionsStart = IMG_Load("assets/Instructions-start.png");
	instructionStartTex = SDL_CreateTextureFromSurface(renderer, tmpSurfaceInstructionsStart);
	SDL_FreeSurface(tmpSurfaceInstructionsStart);

	SDL_Surface* tmpSurfaceInstructionsEnd = IMG_Load("assets/Instructions-end.png");
	instructionEndTex = SDL_CreateTextureFromSurface(renderer, tmpSurfaceInstructionsEnd);
	SDL_FreeSurface(tmpSurfaceInstructionsEnd);

	SDL_Surface* tmpSurfaceInstructionsStepOne = IMG_Load("assets/Instructions-1.png");
	instructionStep1Tex = SDL_CreateTextureFromSurface(renderer, tmpSurfaceInstructionsStepOne);
	SDL_FreeSurface(tmpSurfaceInstructionsStepOne);

	SDL_Surface* tmpSurfaceInstructionsStepTwo = IMG_Load("assets/Instructions-2.png");
	instructionStep2Tex = SDL_CreateTextureFromSurface(renderer, tmpSurfaceInstructionsStepTwo);
	SDL_FreeSurface(tmpSurfaceInstructionsStepTwo);

	SDL_Surface* tmpSurfaceInstructionsStepThree = IMG_Load("assets/Instructions-3.png");
	instructionStep3Tex = SDL_CreateTextureFromSurface(renderer, tmpSurfaceInstructionsStepThree);
	SDL_FreeSurface(tmpSurfaceInstructionsStepThree);

	SDL_Surface* tmpSurfaceError = IMG_Load("assets/Error.png");
	ErrorScreenTex = SDL_CreateTextureFromSurface(renderer, tmpSurfaceError);
	SDL_FreeSurface(tmpSurfaceError);

	SDL_Surface* tmpSurfaceBlock1Finished = IMG_Load("assets/Block-1.png");
	Block1FinishedTex = SDL_CreateTextureFromSurface(renderer, tmpSurfaceBlock1Finished);
	SDL_FreeSurface(tmpSurfaceBlock1Finished);

	SDL_Surface* tmpSurfaceBlock2Finished = IMG_Load("assets/Block-2.png");
	Block2FinishedTex = SDL_CreateTextureFromSurface(renderer, tmpSurfaceBlock2Finished);
	SDL_FreeSurface(tmpSurfaceBlock2Finished);

	SDL_Surface* tmpSurfaceBlock3Finished = IMG_Load("assets/Block-3.png");
	Block3FinishedTex = SDL_CreateTextureFromSurface(renderer, tmpSurfaceBlock3Finished);
	SDL_FreeSurface(tmpSurfaceBlock3Finished);

	SDL_Surface* tmpSurfaceFinished = IMG_Load("assets/Finished.png");
	FinishedTex = SDL_CreateTextureFromSurface(renderer, tmpSurfaceFinished);
	SDL_FreeSurface(tmpSurfaceFinished);
}

void Game::generateAndShuffleMatrix(int matrixBlockSize)
{
	auto trials = generateMatrix(matrixBlockSize);
	shuffleMatrix(trials);
	shuffledTrials = trials;
	currentTrial = shuffledTrials[0];
	previousTrial = {};
}

// continue to the next trial
// apply logic and save data
void Game::advanceTrial(int success)
{
	if (!isPracticeBlock) {
		reaction_time += SDL_GetTicks() - last_sample_time;

		if (currentTrial.isFirst) {
			trial_information_data.push_back({ trialCount, experimentalBlockCount, success, reaction_time, currentTrial.currentCongruent, currentTrial.stimulusDirection, currentTrial.stimulusPosition, currentTrial.currentCongruent, currentTrial.stimulusDirection, currentTrial.stimulusPosition });
		}
		trial_information_data.push_back({ trialCount, experimentalBlockCount, success, reaction_time, currentTrial.currentCongruent, currentTrial.stimulusDirection, currentTrial.stimulusPosition, previousTrial.currentCongruent, previousTrial.stimulusDirection, previousTrial.stimulusPosition });
	}

	deadlineTimer = SDL_GetTicks();
	trialCount++;
	trialPhase = 1;

	if (isPracticeBlock) 
	{
		if (success == 0 && hasFeedback) {
			isFeedbackDisplayed = true;
		}

		if (trialCount == practiceBockDeadlineCutoff+1)
		{
			hasDeadline = true;
		}

		if (trialCount == practiceBlockFeedbackCutoff+1)
		{
			hasFeedback = false;
		}

		if (trialCount > practiceBlockSize)
		{
			trialCount = 1;
			isPracticeBlock = false;
			generateAndShuffleMatrix(getNextMultipleOf16(experimentalBlockSize));
			startRealScreen = true;
		}
	}
	else {
		std::cout << trialCount << std::endl;
		if (trialCount > experimentalBlockSize)
		{	
			experimentalBlockCount++;



			if (experimentalBlockCount == 3) {
				if (experimentalCondition == 1) {
					std::string command = "echo \"" + std::to_string(latency) + " " +
						std::to_string(latency) + " " +
						std::to_string(latency) + " " +
						std::to_string(latency) +
						"\" > " + "/tmp/DelayDaemon";

					std::system(command.c_str());
				}
				else {
					std::string command = "echo \"0 0 0 0\" > /tmp/DelayDaemon";
					std::system(command.c_str());
				}
			}

			saveData();

			if (experimentalBlockCount > experimentalBlockNum)
			{
				gameFinished = true;
			}
			else {
				trialCount = 1;
				generateAndShuffleMatrix(getNextMultipleOf16(experimentalBlockSize));
				gamePaused = true;
			}
		}
	}

	if (!gameFinished) {
		previousTrial = currentTrial;
		int currentTrialIdx = trialCount - 1;
		currentTrial = shuffledTrials[currentTrialIdx];
	}

	// potentially: reset mouse position when trial fails/succeeds
	//SDL_WarpMouseInWindow(window, 960, 960);
}

// handle mouse and keyboard events
void Game::handleEvents()	
{
	SDL_Event event;
	SDL_PollEvent(&event);
	sampleMouseData();

	bool realTrial = !isFeedbackDisplayed && !gamePaused && !startRealScreen && !instructions  && !gameFinished && !deadlineError;

	switch (event.type) {
		case SDL_QUIT:
			isRunning = false;
			break;

		case SDL_MOUSEBUTTONDOWN:
			if (!realTrial) {
				break;
			}

			// red button clicked
			if (isPointInRect(event.button.x, event.button.y, redBoxDestR) && trialPhase == 1)
			{
				trialPhase = 2;
				deadlineTimer = SDL_GetTicks();
			}

			break;

		case SDL_KEYDOWN:
			if (gameFinished && event.key.keysym.sym == SDLK_ESCAPE) {
				isRunning = false;
			}

			if (event.key.keysym.sym == SDLK_SPACE) {
				if (instructions && instructionsSlide == 5) {
					instructions = false;
				} else if (gamePaused || startRealScreen) {
					deadlineTimer = SDL_GetTicks();
					reaction_time = 0;
					gamePaused = false;
					startRealScreen = false;
				}
			}

			if (instructions) {
				if (event.key.keysym.sym == SDLK_RIGHT) {
					if (instructionsSlide < 5) {
						instructionsSlide++;
					}
				}

				if (event.key.keysym.sym == SDLK_LEFT) {
					if (instructionsSlide > 1) {
						instructionsSlide--;
					}
				}
			}

			break;
		case SDL_MOUSEMOTION:
			if (trialPhase == 2)
			{

				int currentY = event.motion.y;
				Uint32 currentTime = SDL_GetTicks(); 

				if (initialY == -1) {
					initialY = currentY;
					initialTime = currentTime;
				}

				// Check if upward movement threshold is reached
				int deltaY = currentY - initialY; 
				Uint32 deltaTime = currentTime - initialTime;

				if (!upwardDetected && deltaY <= -50 && deltaTime <= 250) {
					upwardDetected = true;
					trialPhase = 3;
					deadlineTimer = SDL_GetTicks();
					last_sample_time = SDL_GetTicks();
					reaction_time = 0;

					if (!isPracticeBlock) {
						SDL_GetMouseState(&event.motion.x, &event.motion.y);

						mouse_data.push_back({ trialCount, experimentalBlockCount, 0, event.motion.x, event.motion.y });
					}

					initialY = -1;
					initialTime = 0;
				}

				if (deltaTime > 250) {
					initialY = -1;
					initialTime = 0;
				}

				/**
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
						last_sample_time = SDL_GetTicks();

						if (!isPracticeBlock) {
							SDL_GetMouseState(&event.motion.x, &event.motion.y);

							mouse_data.push_back({ trialCount, experimentalBlockCount, 0, event.motion.x, event.motion.y });
						}
					}
				}
				else {
					continousUpwardsMovement = 0;
				}
				*/
			}

			// handle tracking of mouse data in Phase 3
			else if (trialPhase == 3) {
				if (upwardDetected) {
					upwardDetected = false;
				}
				
				if (isPointInRect(event.button.x, event.button.y, whiteBoxLeftDestR))
				{
					if (isCorrectResponse("left", currentTrial))
					{
						advanceTrial(1);
					}
					else {
						advanceTrial(0);
					}
				
				}
				if (isPointInRect(event.button.x, event.button.y, whiteBoxRightDestR))
				{
					if (isCorrectResponse("right", currentTrial))
					{
						advanceTrial(1);
					}
					else {
						advanceTrial(0);
					}
				}
			}

			break;
		default: 
			break;
	}
}

void Game::sampleMouseData()
{
	if (trialPhase == 3 && !isPracticeBlock && !gameFinished && !gamePaused && !instructions && !deadlineError) {
		// Timer-based sampling
		Uint32 currentTime = SDL_GetTicks();
		Uint32 timeDiff = currentTime - last_sample_time;

		if (timeDiff >= sampling_interval_ms) {
			// Update the last sample time
			reaction_time += timeDiff;

			last_sample_time = currentTime;

			// Get mouse position
			int mouseX, mouseY;
			SDL_GetMouseState(&mouseX, &mouseY);

			mouse_data.push_back({ trialCount, experimentalBlockCount, timeDiff, mouseX, mouseY });
		}
	}
}

void Game::update()
{
	if (deadlineError) {
		if (deadlineErrorTimer == 0) {
			deadlineErrorTimer = SDL_GetTicks();
		}
		
		if (SDL_GetTicks() - deadlineErrorTimer > deadlineErrorDuration) {
			last_sample_time = SDL_GetTicks();
			reaction_time = 0;
			advanceTrial(0);
			deadlineError = false;
			deadlineErrorTimer = 0;
		}
	}
	else if (isFeedbackDisplayed)
	{
		deadlineTimer = SDL_GetTicks();
		if (feedbackTimer == 0) {
			feedbackTimer = SDL_GetTicks();
		}
		Uint32 timeDiff = SDL_GetTicks() - feedbackTimer;

		if (timeDiff > 1000) {
			isFeedbackDisplayed = false;
			feedbackTimer = 0;
		}

	}	else if (hasDeadline && !gamePaused && !startRealScreen && !instructions && !gameFinished && !deadlineError)
	{
		sampleMouseData();
		Uint32 timeDiff = SDL_GetTicks() - deadlineTimer;
		
		if (trialPhase == 1 && timeDiff > phase1Deadline || trialPhase == 2 && timeDiff > phase2Deadline || trialPhase == 3 && timeDiff > phase3Deadline)
		{
			deadlineError = true;
		}
	}
}

void Game::render()
{
	sampleMouseData();
	SDL_RenderClear(renderer);

	if (gameFinished) {
		SDL_RenderCopy(renderer, FinishedTex, NULL, &fullscreenDestR);
	}
	else if (deadlineError) {
		SDL_RenderCopy(renderer, ErrorScreenTex, NULL, &fullscreenDestR);
	}
	else if (instructions) {
		switch (instructionsSlide) {
			case 1:
				SDL_RenderCopy(renderer, instructionStartTex, NULL, &fullscreenDestR);
				break;
			case 2:
				SDL_RenderCopy(renderer, instructionStep1Tex, NULL, &fullscreenDestR);
				break;  
			case 3:
				SDL_RenderCopy(renderer, instructionStep2Tex, NULL, &fullscreenDestR);
				break;  
			case 4:
				SDL_RenderCopy(renderer, instructionStep3Tex, NULL, &fullscreenDestR);
				break;  
			case 5:
				SDL_RenderCopy(renderer, instructionEndTex, NULL, &fullscreenDestR);
				break;  
			default:
				break;
		}

	}
	else {
		SDL_RenderCopy(renderer, backgroundTex, NULL, &fullscreenDestR);

		if (isFeedbackDisplayed) {
			SDL_RenderCopy(renderer, feedbackTex, NULL, &fullscreenDestR);
		}
		else if (gamePaused) {
			switch (experimentalBlockCount) {
				case 2:
					SDL_RenderCopy(renderer, Block1FinishedTex, NULL, &fullscreenDestR);
					break;
				case 3:
					SDL_RenderCopy(renderer, Block2FinishedTex, NULL, &fullscreenDestR);
					break;
				case 4:
					SDL_RenderCopy(renderer, Block3FinishedTex, NULL, &fullscreenDestR);
					break;
			}
		}
		else if (startRealScreen) {
			SDL_RenderCopy(renderer, startRealTex, NULL, &fullscreenDestR);
		}
		else if (trialPhase == 1)
		{
			SDL_RenderCopy(renderer, redBoxTex, NULL, &redBoxDestR);
		}
		else if (trialPhase == 2) {
			SDL_RenderCopy(renderer, whiteBoxTex, NULL, &whiteBoxLeftDestR);
			SDL_RenderCopy(renderer, whiteBoxTex, NULL, &whiteBoxRightDestR);
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

			SDL_RenderCopy(renderer, whiteBoxTex, NULL, &whiteBoxLeftDestR);
			SDL_RenderCopy(renderer, whiteBoxTex, NULL, &whiteBoxRightDestR);
		}
	}

	SDL_RenderPresent(renderer);
}

void Game::clean()
{
	std::string command = "echo \"0 0 0 0\" > /tmp/DelayDaemon";
	std::system(command.c_str());
	
	if (mouseDataFile.is_open()) {
		mouseDataFile.close();
		std::cout << "Mousedata file closed.\n";
	}
	
	if (trialDataFile.is_open()) {
		trialDataFile.close(); 
		std::cout << "Trialdata file closed.\n";
	}

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	std::cout << "Game cleaned!" << std::endl;
}
