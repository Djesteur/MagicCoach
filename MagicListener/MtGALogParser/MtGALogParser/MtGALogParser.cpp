#include "pch.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <ctime>
#include <json/json.h>

#pragma warning(disable : 4996)

using namespace std;

bool getLogInformations(string, bool*);
bool skipLine(int, ifstream &);
string getJSON(string, ifstream &, ofstream &, int&);
bool jumpLine(string);
bool secondePasse(ifstream &, ofstream*, int&, bool&);
int choixSortie(string);
string getAction(string);
vector<string> getMessageInJson(string);


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

bool skipLine(int n, ifstream & fichier) {
	string line;
	bool nAtteind = false;
	for (int i = 0; i < n && !fichier.eof(); i++) {
		getline(fichier, line);
		if (i == n - 1) { nAtteind = true; };
	}
	//return (nAtteind);
	return !(fichier.eof()); // retourne vrai si fichier n'a pas atteind la fin.
}

string getJSON(string l1, ifstream & fichier, ofstream & sortie, int & ligneLues) {

	int nbAccolade = 0;
	int nbCrochet = 0;
	bool inJson = false;
	bool inArray = false;
	bool ligne1 = true;
	string line = l1;
	string jsonEnCours = "";

	do {
		if (ligne1) { ligne1 = false; } else { getline(fichier, line); ligneLues++; }
		for (char& c : line) {//On lis char par char la ligne

			if (c == '{') {
				nbAccolade++;
				inJson = true;
			} else if (c == '[') {
				nbCrochet++;
				inArray = true;
			}
			//On ecrit dans le fichier des JSON sinon dans autre ligne
			if (inJson) {
				sortie << c;
				jsonEnCours += c;
			}
			//On recher les } si c'était la dernière du JSON on "sort" du JSON
			if (c == '}') {
				nbAccolade--;
				if (nbAccolade <= 0) {
					nbAccolade = 0;
					inJson = false;
				}
			} else if (c == ']') {
				nbCrochet--;
				if (nbCrochet <= 0) {
					nbCrochet = 0;
					inArray = false;
					//onUtile = -1; //On reset onUtile
				}
			}

		}//fin pour char
	} while (inJson || inArray);
	//fin pour ligne
	return jsonEnCours;
}

bool jumpLine(string line) {

	int const tailleList = 6;
	string listJump[tailleList];
	listJump[0] = (string)"[Unity";
	listJump[1] = (string)"[Store";
	listJump[2] = (string)"[Discord";
	listJump[3] = (string)"[Account";
	listJump[4] = (string)"[Social";
	listJump[5] = (string)"[Client";

	for (string l : listJump) {
		size_t foundJump = line.find(l);
		if (foundJump != string::npos) {
			return true;
		}
	}

	return false;
}

bool getLogInformations(string fileName, bool* continuerProgramme) {

	cout << "\n===========================\n";
	cout << "Parsage par methode double line/chars \n";
	cout << "===========================\n";

	ifstream file;
	file.open(fileName);

	if (!file) {
		cout << "Le fichier est introuvable dans le dossier du programe ou n'a pas pus etre ouvert.\n";
		return false;
	}

	ofstream sorties[5];
	sorties[0].open("Parsed_Files/discard.txt", ofstream::out);
	if (!sorties[0]) {
		cout << "Le fichier 'discard.txt' n'a pas pus être créé.\n";
		return false;
	}
	sorties[1].open("Parsed_Files/log_JSON.json", ofstream::out); //JSON en bordel
	if (!sorties[1]) {
		cout << "Le fichier 'log_JSON.json' n'a pas pus être créé.\n";
		return false;
	}
	sorties[2].open("Parsed_Files/deck.json", ofstream::out);
	if (!sorties[2]) {
		cout << "Le fichier 'deck.json' n'a pas pus être créé.\n";
		return false;
	}
	sorties[3].open("Parsed_Files/carte.json", ofstream::out);
	if (!sorties[3]) {
		cout << "Le fichier 'carte.json' n'a pas pus être créé.\n";
		return false;
	}
	sorties[4].open("Parsed_Files/match.json", ofstream::out);
	if (!sorties[4]) {
		cout << "Le fichier 'match.json' n'a pas pus être créé.\n";
		return false;
	}

	/*jsonFile*/sorties[1] << "{\n \"jsonList\" : [\n";
	sorties[2] << "{\n \"jsonList\" : [\n";
	sorties[3] << "{\n \"jsonList\" : [\n";
	sorties[4] << "{\n \"jsonList\" : [\n";

	cout << "Parsing launched ! \n";

	int nbJson = 0;
	int ligneLues = 0;
	int sortie = 1;

	for (string line; getline(file, line);) {
		ligneLues++;
		if (jumpLine(line)) { sorties[0] << line; getline(file, line); ligneLues++; }

		sortie = choixSortie(line);
		if (sortie != 1) { sorties[0] << line; getline(file, line); ligneLues++; }
		//if (line.find("Event.MatchCreated") != string::npos) { sorties[0] << line; sortie = 4; line = "{"; }

		if (line[0] == '{' || (line[0] == '[')) {//(line.find('{')!=string::npos){
			nbJson++;
			string jsonSorti = getJSON(line, file, sorties[sortie], ligneLues); //On récupère le JSON complet en changant le nombre de ligne lue (retourn aussi ce json dans un string)
			
			for (string s : getMessageInJson(jsonSorti)) {
				getAction(s);
			}
			
			//getAction(jsonSorti);
			sorties[sortie] << ",\n";
		} else { sorties[0] << line; }
		sortie = 1;

	}//fin pour ligne

	ligneLues++; //ca compte 1 en moins !?
	cout << ligneLues << " lignes lues !\n";
	cout << "Fin de lecture du début !\n Passage en écoute. \n";

	clock_t start = clock();
	clock_t lastTick = start;
	bool onMatch = false;
	while (*continuerProgramme) {
		if (((clock() - lastTick) / (double)CLOCKS_PER_SEC) == 5) {
			lastTick = clock();
			//cout << "POKE ! \n";
			ifstream file2;
			file2.open(fileName);
			if (!file2) {
				cout << "Le fichier est introuvable dans le dossier du programe ou n'a pas pus etre ouvert.\n";
				return false;
			}
			if (skipLine(ligneLues, file2)) {
				cout << "seconde passe \n";
				secondePasse(file2, sorties, ligneLues, onMatch);
				cout << "Lignes lu au total: " << ligneLues << "\n";
			}
			file2.close();
		}
	}

	//Fermeture du JSON général
	sorties[1] << "\n{\"NbJson\" : " << nbJson << " }\n]\n}";

	sorties[2] << "\n{\"Jsondeck\" : " << 0 << " }\n]\n}";
	sorties[3] << "\n{\"Jsoncarte\" : " << 0 << " }\n]\n}";
	sorties[4] << "\n{\"Jsonmatch\" : " << 0 << " }\n]\n}";

	cout << "\n\n===========================\n";
	cout << "Parsage du fichier terminee.\n";
	cout << "===========================\n\n";

	file.close();

	sorties[0].close();
	sorties[1].close();
	sorties[2].close();
	sorties[3].close();
	sorties[4].close();

	exit(0); // On fermme comme il faut

	return true;

}

bool secondePasse(ifstream & entree, ofstream* sorties, int & lignesLues, bool & onMatch) {
	int sortie = 1;
	for (string line; getline(entree, line);) {
		lignesLues++;
		if (line.find("Event.MatchCreated") != string::npos) {// AVANT JUMP
			sorties[0] << line;
			onMatch = true;
			line = "{";
			cout << "Début de match détécter ! Passage en parsing match.\n";
		}
		if (onMatch) {
			sortie = 4;
		}
		/*
		Ici autre "[Client" utile
		*/

		//On jump une fois les "[Client" possiblement utile lus.
		if (jumpLine(line)) {
			sorties[0] << line; 
			getline(entree, line); 
			lignesLues++;
		}

		if (line[0] == '{' || (line[0] == '[')) {//(line.find('{')!=string::npos){
			//nbJson++;
			getJSON(line, entree, sorties[sortie], lignesLues);
			sorties[sortie] << ",\n";
		} else { 
			sorties[0] << line;
		}

	}//fin pour ligne
	return true;

}

int choixSortie(string line) {
	int const tailleList = 3;
	string utilityList[tailleList];// A OPTIMISER
	utilityList[0] = (string)"<== Deck.GetDeckLists(";
	utilityList[1] = (string)"<== PlayerInventory.GetPlayerCardsV3(";
	utilityList[2] = (string)"[Client GRE]";

	int i = 0;
	for (string l : utilityList) {
		size_t foundUtile = line.find(l);
		if (foundUtile != string::npos) {
			return i + 2;
		}
		i++;
	}

	return 1;//pas a trier
}

string getAction(string json) {
	cout << "Action : ";
	int const tailleList = 5;
	string listAction[tailleList][3];
	listAction[0][0] = (string)"MatchState_MatchComplete"; listAction[0][1] = (string)"FinDeMatch"; listAction[0][2] = (string)"Match terminer !";
	listAction[1][0] = (string)"DuelScene.GameStart"; listAction[1][1] = (string)"DebutDeDuel"; listAction[1][2] = (string)"Game start ! Debut du duel.";
	listAction[2][0] = (string)"Event.MatchCreated"; listAction[2][1] = (string)"MatchCreate"; listAction[2][2] = (string)"Debut d'un match !";
	listAction[3][0] = (string)"Step_BeginCombat"; listAction[3][1] = (string)"DebutCombat"; listAction[3][2] = (string)"Debut d'un combat.";
	listAction[4][0] = (string)"step\": \"Step_Draw"; listAction[4][1] = (string)"Pioche"; listAction[4][2] = (string)"Pioche";

	for (int i = 0; i < tailleList; i++) {
		size_t foundAction = json.find(listAction[i][0]);
		if (foundAction != string::npos) {
			cout << listAction[i][2];
			//return listAction[i][1];
		}
	}

	cout << "\n";

	Json::Value root;
	Json::Reader reader;

	bool parsingSuccessful = reader.parse(json, root);
	if (!parsingSuccessful) {
		cout << "Error parsing the string" << endl;
	} else {

	}

	return "";
}

vector<string> getMessageInJson(string json) {
	vector<string> messages;

	size_t pos = json.find("greToClientMessages");

	if(pos != string::npos) {
		int nbMessage = 0;

		string sub = json.substr(pos);
		int nbCrochet = 0;//le premier crocher serra trouver dans le for
		bool inArray = true;
		int nbAccolade = 0;
		bool inJson = false;
		string jsonEnCours = "";

		for (char& c : sub) {//On lis char par char la ligne
			if (c == '{') {
				nbAccolade++;
				if (!inJson) {
					nbMessage++;
					inJson = true;
				}
			} else if (c == '[') {
				nbCrochet++;
			}
			if (inJson) {
				jsonEnCours += c;
			}
			if (c == '}' && nbAccolade > 0) {
				nbAccolade--;
				if (nbAccolade <= 0) {
					nbAccolade = 0;
					messages.push_back(jsonEnCours);
					jsonEnCours = "";
					inJson = false;
				}
			} else if (c == ']' && nbCrochet > 0) {
				nbCrochet--;
				if (nbCrochet <= 0) {
					nbCrochet = 0;
					inArray = false;
				}
			}

		}//fin pour char
	}
	return messages;
}


int main() {

	bool continuerProgramme = true;

	//thread parsing(getLogInformations, "output_log_1game.txt", &continuerProgramme);
	thread parsing(getLogInformations, "C:/Users/cleme/AppData/LocalLow/Wizards Of The Coast/MTGA/output_log.txt", &continuerProgramme);
	thread comm(readingCommande, &continuerProgramme);
	parsing.join();
	comm.join();

	return EXIT_SUCCESS;
}