#include "MtGALogParser.hpp"

using namespace std;

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

bool getJSON(string l1, ifstream & fichier, int & ligneLues) {

	int nbAccolade = 0;
	int nbCrochet = 0;
	bool inJson = false;
	bool inArray = false;
	bool ligne1 = true;
	string line = l1;
	string jsonEnCours = "";

	ofstream tmpJson;
	tmpJson.open("tmpJson.json", ofstream::out);
	if (!tmpJson) {
		cout << "Erreur avec l'ouverture en ecriture de tmpJson.json \n";
			return false;
	}

	do {
		if (ligne1) { //si c'est la premiere ligne on ne getpas la ligne avec getLine et on ne la compte pas car deja compter
			ligne1 = false; 
		} else { 
			getline(fichier, line); 
			ligneLues++; 
		}
		for (char& c : line) {//On lis char par char la ligne

			if (c == '{') {
				nbAccolade++;
				inJson = true;
			} else if (c == '[') {
				nbCrochet++;
				inArray = true;
			}
			//On ecris le Json
			if (inJson) {
				//jsonEnCours += c;
				tmpJson << c;
			}
			//On recher les } si c'etait la derniere du JSON on "sort" du JSON
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

	} while (inJson || inArray); 	//fin pour ligne

	tmpJson.close();

	return true;
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

//bool getLogInformations(string fileName, bool* continuerProgramme) {
bool getLogInformations(string fileName, Transmitter &trans) {

	cout << "\n===========================\n";
	cout << "Parsage par methode double line/chars \n";
	cout << "===========================\n";

	ifstream file;
	file.open(fileName);
	if (!file) {
		cout << "Le fichier est introuvable dans le dossier du programe ou n'a pas pus etre ouvert.\n";
		return false;
	}

	cout << "Parsing launched ! \n";

	int nbJson = 0;
	int ligneLues = 0;
	int sortie = 1;

	for (string line; getline(file, line);) {
		ligneLues++;
	}//fin pour ligne

	ligneLues++; //ca compte 1 en moins surement la ligne de fin de fichier qui n'est pas compter
	cout << ligneLues << " lignes lues !\n";
	cout << "Fin de lecture du debut !\n Passage en ecoute. \n";

	clock_t start = clock();
	clock_t lastTick = start;
	bool onMatch = false;

	Information lastinfo;
	lastinfo.type = InformationType::PlayCard;
	int passe = 0;
	while (lastinfo.type != InformationType::StopListen) {
		if (((clock() - lastTick) / (double)CLOCKS_PER_SEC) == 1) {
			lastTick = clock();
			ifstream file2;
			file2.open(fileName);
			if (!file2) {
				cout << "Le fichier est introuvable dans le dossier du programe ou n'a pas pus etre ouvert.\n";
				return false;
			}
			if (skipLine(ligneLues, file2)) {
				cout << "Passe n°: " << passe <<"\n";
				secondePasse(file2, ligneLues, onMatch);
				cout << "Lignes lu au total: " << ligneLues << "\n";
			}
			file2.close();
		}
		if (trans.waitingInfoForListener() == true) {
			lastinfo = trans.getListenerInfo();
		}
	}

	
	cout << "\n\n===========================\n";
	cout << "Parsage du fichier terminee.\n";
	cout << "===========================\n\n";

	//Fermeture du JSON general
	file.close();

	exit(0); // On fermme comme il faut

	return true;

}

bool secondePasse(ifstream & entree, int & lignesLues, bool & onMatch) {

	for (string line; getline(entree, line);) {
		lignesLues++;
		if (line.find("Event.MatchCreated") != string::npos) {// AVANT JUMP
			onMatch = true;
			line = "{";
			cout << "Debut de match detecter ! Passage en parsing match.\n";
		}

		/*
		Ici autre "[Client" utile
		*/

		//On jump une fois les "[Client" possiblement utile lus.
		if (jumpLine(line)) {
			getline(entree, line); 
			lignesLues++;
		}

		if (line[0] == '{' || (line[0] == '[')) {//(line.find('{')!=string::npos){
			//nbJson++;
			if (!getJSON(line, entree, lignesLues)) {
				return false;
			}

			for (string s : getMessageInJson()) {
				getAction(s);
			}
		} else { 
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

	Json::Value root;
	Json::Reader reader;

	bool parsingSuccessful = reader.parse(json, root);
	if (!parsingSuccessful) {
		cout << "Error parsing the string" << endl;
	} else {
		/*if (getGameStageStart) {
			cout << "Stage start !\n";
		}*/
		string phase = getPhase(root);
		string step = getStep(root);
		if (phase != "NOTHING") {
			cout << "Phase: " << phase << " | ";
			cout << "Step : " << step << "\n";
		}
		if (getGameOver(root)) {
			cout << "Game over \n";
		}
		if (getMatchOver(root)) {
			cout << "Match over \n";
		}
	}

	return "";
}

vector<string> getMessageInJson() {

	ifstream tmpJson;
	tmpJson.open("tmpJson.json");
	if (!tmpJson) {
		cout << "Le fichier tmpJson.json est introuvable dans le dossier du programe ou n'a pas pus etre ouvert.\n";
	}

	vector<string> messages;

	for (string line; getline(tmpJson, line);) {

		size_t pos = line.find("greToClientMessages");

		if (pos != string::npos) {
			int nbMessage = 0;

			string sub = line.substr(pos);
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
	}//Line par line de tmpJson.json
	
	tmpJson.close();
	return messages;
}

string getStep(Json::Value root) {
	if (root.isMember("gameStateMessage")) {
		if (root["gameStateMessage"].isMember("turnInfo")) {
			if (root["gameStateMessage"]["turnInfo"].isMember("step")) {
				return stringCleaner(root["gameStateMessage"]["turnInfo"]["step"].toStyledString());
			}
		}
	}
	return "NOTHING";
}

string getPhase(Json::Value root) {
	if (root.isMember("gameStateMessage")) {
		if (root["gameStateMessage"].isMember("turnInfo")) {
			if (root["gameStateMessage"]["turnInfo"].isMember("phase")) {
				return stringCleaner(root["gameStateMessage"]["turnInfo"]["phase"].toStyledString());
			}
		}
	}
	return "NOTHING";
}

bool getGameOver(Json::Value root) {
	if (root.isMember("gameStateMessage")) {
		if (root["gameStateMessage"].isMember("gameInfo")) {
			if (root["gameStateMessage"]["gameInfo"].isMember("stage")) {
				string end = stringCleaner(root["gameStateMessage"]["gameInfo"]["stage"].toStyledString());
				if (end == "GameStage_GameOver") {
					return true;
				}
			}
		}
	}
	return false;
}

bool getMatchOver(Json::Value root) {
	if (root.isMember("gameStateMessage")) {
		if (root["gameStateMessage"].isMember("gameInfo")) {
			if (root["gameStateMessage"]["gameInfo"].isMember("matchState")) {
				string end = stringCleaner(root["gameStateMessage"]["gameInfo"]["matchState"].toStyledString());
				if (end == "MatchState_GameComplete") {
					return true;
				}
			}
		}
	}
	return false;
}

bool getGameStageStart(Json::Value root) {
	if (root.isMember("gameStateMessage")) {
		if (root["gameStateMessage"].isMember("gameInfo")) {
			if (root["gameStateMessage"]["gameInfo"].isMember("stage")) {
				string end = stringCleaner(root["gameStateMessage"]["gameInfo"]["stage"].toStyledString());
				if (end == "GameStage_Start") {
					return true;
				}
			}
		}
	}
	return false;
}

string stringCleaner(string s) {
	return s.substr(1, s.length()-3); //suprime les " " et \n a la fin des stylestring
}

//Fonction de demarage du parsing.
void startParsing(Transmitter &trans) {
	string userHome = getenv("USERPROFILE");
	string folder = userHome + "/AppData/LocalLow/Wizards Of The Coast/MTGA/output_log.txt";
	getLogInformations(folder, trans); //lis les premier ligne et start l'ecoute
}