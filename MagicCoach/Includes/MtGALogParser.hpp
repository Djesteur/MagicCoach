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

#include "Transmitter.hpp"

bool getLogInformations(std::string, bool*);
bool skipLine(int, std::ifstream &);
std::string getJSON(std::string, std::ifstream &, int&);
bool jumpLine(std::string);
bool secondePasse(std::ifstream &,int&, bool&);
int choixSortie(std::string);
std::string getAction(std::string);
std::vector<std::string> getMessageInJson(std::string);
std::string getStep(Json::Value);
std::string getPhase(Json::Value);
std::string stringCleaner(std::string);
void startParsing(Transmitter&);
bool getGameOver(Json::Value);
bool getMatchOver(Json::Value);
bool getGameStageStart(Json::Value);

#endif