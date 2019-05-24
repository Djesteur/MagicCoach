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

/*Class utiliser pour parser les message des logs*/

class JsonMessage {
private:
	std::string message;
	Json::Value root;
	Json::Reader reader;
	bool parsingSuccess;
	bool debug;

public:
	JsonMessage(std::string, bool);

	bool getAction(Transmitter &);

	std::string getStep();
	std::string getPhase();

	unsigned int getActivePlayer();

	bool getGameOver();
	bool getMatchOver();
	bool getGameStageStart();

	std::array<std::vector<std::vector<int>>, 3> getGameObjects();
	std::vector<int> getCast();

	std::vector<int> getDeclareAttack();

	int stepToInt(std::string, std::string);

	~JsonMessage();
};
#endif