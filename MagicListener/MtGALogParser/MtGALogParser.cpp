#include "pch.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>


void parsingUtileFile(std::string fileName, std::vector<std::string> &tabL, std::vector<std::ofstream> &tabF){

	std::ifstream file;
	file.open(fileName);

	if (!file) {
		std::cout << "Le fichier : "<< fileName << " est introuvable dans le dossier du programe ou n'a pas pus etre ouvert.\n";
	}

	for (std::string line; getline(file, line);) {
		int spacePos = line.find("|");
		tabL.push_back(line.substr(0, spacePos));

		//un tableau de fichier.... tordu ?
		std::ofstream f;
		f.open(line.substr(spacePos + 1, line.size()));
		tabF.push_back(f);
	}

	file.close();

}

void closeTabFile(std::vector<std::ofstream> &tabF) {
	for (size_t i = 0; i < tabF.size(); i++) {
		tabF[i].close();
	}
}

bool checkLigneSkip(std::string line) {

	int const tailleList = 1;
	std::string listInutile[tailleList];// A OPTIMISER !!!!!!! Merci
	listInutile[0] = (std::string)"| Response:";

	for (std::string l: listInutile) {
		std::size_t foundInutile = line.find(l);
		if (foundInutile != std::string::npos) {
			return true;
		}
	}

	return false;
}

int checkLigneUtility(std::string line, std::vector<std::string> utilityList) {

	for (size_t i = 0; i < utilityList.size(); i++) {
		std::size_t foundUtile = line.find(utilityList[i]);
		if (foundUtile != std::string::npos) {
			return i;
		}
	}

	return -1;//pas a trier
}


bool getLogInformations(std::string fileName, std::vector<std::string> utilityList, std::vector<std::ofstream> tabFile) {

	std::cout << "\n~~~~~~~~~~~~~~~~~~~~~~~~\n";
	std::cout << "Parsage par methode double line/chars \n";
	std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~\n";

	std::ifstream file;
	file.open(fileName);

	if (!file) {
		std::cout << "Le fichier est introuvable dans le dossier du programe ou n'a pas pus etre ouvert.\n";
		return false;
	}

	std::ofstream jsonFile("Parsed_Files/log_JSON.json", std::ofstream::out);
	if (!jsonFile) {
		std::cout << "Le fichier 'log_JSON.json' n'a pas être créer.\n";
		return false;
	}


	jsonFile << "{\n \"jsonList\" : [\n";

	int nbAcolade = 0;
	int nbCrocher = 0;
	bool inJson = false;
	int nbJson = 0;
	int onUtile = -1;

	for (std::string line; getline(file, line);) {

		bool skipLine = checkLigneSkip(line);

		int utile = checkLigneUtility(line, utilityList);
		if (utile != -1) {
			std::cout << "Json Utile trouver ! \n";
			onUtile = utile;
			skipLine = true; //on skipe la ligne car pas besoins ce la première
		}

		if (!skipLine) {//on skip on non la ligne si un truck inutile est trouver ou non

			for (char& c : line) {//On lis char par char la ligne

				//std::cout << chars;

				//Si on trouve une { on vérifie que l'on était pas déjà dans un JSON
				if (c == '{') {
					if (!inJson) {
						nbJson++;
						std::cout << "Nb JSON find: " << nbJson << "\n";
						inJson = true;
					}
					nbAcolade++;
				}else if (onUtile != -1) {
					if (c == '[') {
						nbCrocher++;
					}
				}

				//On ecrit dans le fichier des JSON sinon dans autre ligne
				if (inJson || onUtile != -1) {

					switch (onUtile) {
					case -1:
						jsonFile << c;
						break;
					default:
						tabFile[onUtile] << c; 
						break;
					}
				}
				//On recher les } si c'était la dernière du JSON on "sort" du JSON
				if (c == '}') {
					nbAcolade--;
					if (nbAcolade <= 0) {
						nbAcolade = 0;
						//Ajout de , a la fin de l'objet pour la liste de JSON
						jsonFile << ",\n";
						inJson = false;
					}
				}else if(onUtile != -1) {
					if (c == ']') {
						nbCrocher--;
						if (nbCrocher <= 0) {
							nbCrocher = 0;
							onUtile = -1; //On reset onUtile
						}
					}
				}

			}//fin pour char

		}

	}//fin pour ligne

	//Fermeture du JSON général
	jsonFile << "\n{\"NbJson\" : " << nbJson << " }\n]\n}";

	file.close();
	jsonFile.close();

	return true;
}


int main(){

	std::vector<std::string> utilityList;
	std::vector<std::ofstream> tabFile;

	parsingUtileFile("utile.txt", utilityList, tabFile);

	std::vector<std::string, std::string> notUitlityList;


    std::cout << "Log file:\n";
	getLogInformations("output_log.txt", utilityList, tabFile);

	std::cout << "\n\n===========================\n";
	std::cout << "Parsage du fichier terminee.\n";
	std::cout << "===========================\n\n";

	closeTabFile(tabFile);

	return EXIT_SUCCESS;
}
