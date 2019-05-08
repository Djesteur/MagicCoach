#ifndef JSONMESSAGE_HPP
#define JSONMESSAGE_HPP

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <ctime>
#include <json.h>
#include <iterator>
#include <algorithm>
#include <array>

#include "Transmitter.hpp"

class JsonMessage {
private:
	std::string message;
	Json::Value root;
	Json::Reader reader;
	bool parsingSuccess;

public:
	JsonMessage(std::string);

	bool getAction(Transmitter &);

	std::string getStep();
	std::string getPhase();

	unsigned int getActivePlayer();

	bool getGameOver();
	bool getMatchOver();
	bool getGameStageStart();

	std::vector<std::array<int, 4>> getGameObjects();
	std::vector<int> getCast();

	int stepToInt(std::string, std::string);

	~JsonMessage();
};
#endif