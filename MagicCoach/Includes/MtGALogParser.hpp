#ifndef MTGALOGPARSER_HPP
#define MTGALOGPARSER_HPP

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

#include "Transmitter.hpp"

bool getLogInformations(std::string, bool*);
bool skipLine(int, std::ifstream &);
bool getJSON(std::string, std::ifstream &, unsigned int &, int &);
bool jumpLine(std::string);
bool secondePasse(std::string, int &, unsigned int &, bool&, Transmitter&);
int choixSortie(std::string);
Information getAction(std::string);
std::vector<std::string> getMessageInJson();
std::string getStep(Json::Value);
std::string getPhase(Json::Value);
std::string stringCleaner(std::string);
void startParsing(Transmitter&);
bool getGameOver(Json::Value);
bool getMatchOver(Json::Value);
bool getGameStageStart(Json::Value);
unsigned int getActivePlayer(Json::Value);
int stepToInt(std::string, std::string);
unsigned int getNbLine(std::string);
unsigned int getNbLine(std::string, int &);
int getFileSize(std::string);

#endif