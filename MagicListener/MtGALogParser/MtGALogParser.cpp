#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <ctime>

using namespace std;

/*void parsingUtileFile(string fileName, vector<string> tabL, vector<shared_ptr<ofstream>> tabF) {

	ifstream file;
	file.open(fileName);

	if (!file) {
		cout << "Le fichier : " << fileName << " est introuvable dans le dossier du programe ou n'a pas pus etre ouvert.\n";
	}

	int i = 0;
	for (string line; getline(file, line);) {
		int spacePos = line.find("|");
		tabL.push_back(line.substr(0, spacePos));

		ofstream f(line.substr(spacePos + 1, line.size()));
		tabF.push_back(make_shared<ofstream>(f));
		i++;
	}

	file.close();

}

void closeTabFile(vector<shared_ptr<ofstream>> tabF) {
	for (size_t i = 0; i < tabF.size(); i++) {
		tabF[i]->close();
	}
}*/

bool checkLigneSkip(string line) {

	int const tailleList = 1;
	string listInutile[tailleList];// A OPTIMISER
	listInutile[0] = (string)"| Response:";

	for (string l: listInutile) {
		size_t foundInutile = line.find(l);
		if (foundInutile != string::npos) {
			return true;
		}
	}

	return false;
}

int checkLigneUtility(string line, vector<string> utilityList) {

	for (size_t i = 0; i < utilityList.size(); i++) {
		size_t foundUtile = line.find(utilityList[i]);
		if (foundUtile != string::npos) {
			return i;
		}
	}

	return -1;//pas a trier
}

int checkLigneUtility(string line) {

	int const tailleList = 2;
	string utilityList[tailleList];// A OPTIMISER
	utilityList[0] = (string)"<== Deck.GetDeckLists(";
	utilityList[1] = (string)"<== PlayerInventory.GetPlayerCardsV3(";

	int i = 0;
	for (string l : utilityList) {
		size_t foundUtile = line.find(l);
		if (foundUtile != string::npos) {
			return i;
		}
		i++;
	}

	return -1;//pas a trier
}


//bool getLogInformations(string fileName, vector<string> utilityList, vector<shared_ptr<ofstream>> tabFile) {
bool getLogInformations(string fileName, bool *continuerProgramme) {

	cout << "\n===========================\n";
	cout << "Parsage par methode double line/chars \n";
	cout << "===========================\n";

	ifstream file;
	file.open(fileName);

	if (!file) {
		cout << "Le fichier est introuvable dans le dossier du programe ou n'a pas pus etre ouvert.\n";
		return false;
	}

	ofstream jsonFile("Parsed_Files/log_JSON.json", ofstream::out);
	if (!jsonFile) {
		cout << "Le fichier 'log_JSON.json' n'a pas pus être créer.\n";
		return false;
	}

	ofstream deckFile("Parsed_Files/deck.json", ofstream::out);
	if (!jsonFile) {
		cout << "Le fichier 'deck.json' n'a pas pus être créer.\n";
		return false;
	}

	ofstream carteFile("Parsed_Files/carte.json", ofstream::out);
	if (!jsonFile) {
		cout << "Le fichier 'carte.json' n'a pas pus être créer.\n";
		return false;
	}

	jsonFile << "{\n \"jsonList\" : [\n";

	int nbAcolade = 0;
	int nbCrocher = 0;
	bool inJson = false;
	int nbJson = 0;
	int onUtile = -1;

	cout << "Parsing launched ! \n";

	//istream_iterator = 

	for (string line; getline(file, line);) {

		bool skipLine = checkLigneSkip(line);

		//int utile = checkLigneUtility(line, utilityList);
		int utile = checkLigneUtility(line);
		if (utile != -1) {
			//cout << "Json Utile trouver ! \n";
			onUtile = utile;
			skipLine = true; //on skipe la ligne car pas besoins ce la première
		}

		if (!skipLine) {//on skip on non la ligne si un truck inutile est trouver ou non

			for (char& c : line) {//On lis char par char la ligne

				//cout << chars;

				//Si on trouve une { on vérifie que l'on était pas déjà dans un JSON
				if (c == '{') {
					if (!inJson) {
						nbJson++;
						//cout << "Nb JSON find: " << nbJson << "\n";
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

					/*switch (onUtile) {
					case -1:
						jsonFile << c;
						break;
					default:
						tabFile[onUtile]->operator<<(c); // Oo wtf
						break;
					}*/

					switch (onUtile) {
					case 0:
						deckFile << c;
						break;
					case 1:
						carteFile << c;
						break;
					default:
						jsonFile << c; // Oo wtf
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

	cout << "Fin de lecture du début !\n Passage en écoute. \n";

	clock_t start = clock();
	clock_t lastTick = start;
	while (*continuerProgramme) { //AJOUTER LECTURE LIGNE PAR LIGNE APRES PREMIRE LECTURE ! ... Je sais pas comment faire pour le moment
		if (((clock() - lastTick) / (double)CLOCKS_PER_SEC) == 2) {
			lastTick = clock();
			cout << "POKE ! \n";
		}
	}

	//Fermeture du JSON général
	jsonFile << "\n{\"NbJson\" : " << nbJson << " }\n]\n}";

	cout << "\n\n===========================\n";
	cout << "Parsage du fichier terminee.\n";
	cout << "===========================\n\n";

	file.close();
	jsonFile.close();

	deckFile.close();
	carteFile.close();

	exit(0); // On fermme comme il faut

	return true;
}

void readingCommande(bool *continuerProgramme) {
	string commande;
	while (*continuerProgramme) {
		cout << "Commande:";
		getline(cin, commande); //getline pour avoir toutes la ligne propre
		if (commande == "quit") {
			cout << "Le programme vas ce terminer correctement dans quelque instant. \n";
			*continuerProgramme = false;
		}
		cout << "\n";
	}
}


int main(){

	/*vector<string> utilityList;
	vector<shared_ptr<ofstream>> tabFile;

	parsingUtileFile("utile.txt", utilityList, tabFile);

    cout << "Log file:\n";
	getLogInformations("output_log.txt", utilityList, tabFile);*/

	bool continuerProgramme = true;

	thread parsing(getLogInformations, "output_log.txt", &continuerProgramme);
	thread comm(readingCommande, &continuerProgramme);
	parsing.join();
	comm.join();


	//closeTabFile(tabFile);

	return EXIT_SUCCESS;
}
