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
#include <array>

#include "Transmitter.hpp"
#include "JsonMessage.hpp"

bool getLogInformations(std::string, bool*);
bool skipLine(int, std::ifstream &);
bool getJSON(std::string, std::ifstream &, unsigned int &, int &);
bool jumpLine(std::string);
bool secondePasse(std::string, int &, unsigned int &, bool&, Transmitter&);
int choixSortie(std::string);
std::vector<std::string> getMessageInJson();
void startParsing(Transmitter&);
unsigned int getNbLine(std::string);
unsigned int getNbLine(std::string, int &);
int getFileSize(std::string);

#endif