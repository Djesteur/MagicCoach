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

bool getJSON(string l1, ifstream & fichier, unsigned int & ligneLues, int & lastPosition) {

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
			lastPosition = fichier.tellg();
			tmpJson << "\n"; //Evite la ligne de 300Km qui fait surement buger
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

	int lastPosition = 0;
	unsigned int lastLine = getNbLine(fileName, lastPosition);

	cout << "Parser: " << lastLine << " lignes Skip ! | File size : " << lastPosition << "\n";

	cout << "Parser: Passage en ecoute. \n";

	clock_t start = clock();
	clock_t lastTick = start;
	bool onMatch = false;

	Information lastinfo;
	lastinfo.type = InformationType::PlayCard;
	int passe = 0;
	int nbClock = 0;
	while (lastinfo.type != InformationType::StopListen) {
		if (((clock() - lastTick) / (double)CLOCKS_PER_SEC) == 5) { //Mettre 10 sec en temps normal si bug encore en partie
			lastTick = clock();

			//unsigned int nbLineFile = getNbLine(fileName);

			//if (lastLine < nbLineFile) {
			if (lastPosition < getFileSize(fileName)){
				//cout << "Nb line file : " << nbLineFile << "\n";
				cout << "Parser: Passe num: " << passe <<"\n";
				secondePasse(fileName, lastPosition, lastLine, onMatch, trans);
				passe++;
			}

			nbClock++;
		}
		if (trans.waitingInfoForListener() == true) {
			lastinfo = trans.getListenerInfo();
		}
	}

	exit(0); // On fermme comme il faut

	return true;

}

bool secondePasse(string fileName, int & lastPosition ,unsigned int & lastLine, bool & onMatch, Transmitter &trans) {

	unsigned int saveLastLine = lastLine;

	ifstream file;
	file.open(fileName);
	if (!file) {
		cout << "Parser: Le fichier est introuvable dans le dossier du programe ou n'a pas pus etre ouvert.\n";
		return false;
	}

	file.seekg(lastPosition); //On remet a l'ancienne position du fichier.
	//cout << "Debug position : " << file.tellg() << "\n";

	//skipLine(lastLine, file);
	for (string line; getline(file, line);) {
		lastLine++;
		lastPosition = file.tellg();
		//cout << "Debug position at getline : " << file.tellg() << "\n";
		if (line.find("Event.MatchCreated") != string::npos) {// AVANT JUMP
			onMatch = true;
			line = "{";
			cout << "Parser: Debut de match detecter ! Passage en parsing match.\n";
		}

		/*
		Ici autre "[Client" utile
		*/

		//On jump une fois les "[Client" possiblement utile lus.
		if (jumpLine(line)) {
			getline(file, line); 
			lastLine++;
			lastPosition = file.tellg();
		}
		
		//cout << "Debug position befor getJson : " << lastPosition << "\n";

		if (line[0] == '{' || (line[0] == '[')) {//(line.find('{')!=string::npos){
			//nbJson++;
			if (!getJSON(line, file, lastLine, lastPosition)) {
				file.close();
				return false;
			}

			for (string s : getMessageInJson()) {
				trans.addInfoForCoach(getAction(s));
			}
		} else { 
		}

	}//fin pour ligne
	file.close();

	int nbLineRead = (lastLine - saveLastLine);
	cout << "Parser: Ligne lues pendant la passe : " << nbLineRead << " | File size: " << lastPosition <<"\n";
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

Information getAction(string json) {

	Json::Value root;
	Json::Reader reader;

	Information info = Information();

	bool parsingSuccessful = reader.parse(json, root);
	if (!parsingSuccessful) {
		cout << "Parser: Error parsing the string" << endl;
	} else {

		info.player = getActivePlayer(root);
		/*if (getGameStageStart) {
			cout << "Stage start !\n";
		}*/
		string phase = getPhase(root);
		string step = getStep(root);
		if (phase != "NOTHING") {
			info.type = InformationType::CurrentStep;
			info.values.push_back(stepToInt(step, phase));
			cout << "Phase: " << phase << " | ";
			cout << "Step : " << step << "\n";
		}
		if (getGameOver(root)) {
			info.type = InformationType::GameOver;
			cout << "Game over \n";
		}
		if (getMatchOver(root)) {
			info.type = InformationType::MatchOver;
			cout << "Match over \n";
		}
	}

	return info;
}

vector<string> getMessageInJson() {

	ifstream tmpJson;
	tmpJson.open("tmpJson.json");
	if (!tmpJson) {
		cout << "Parser: Le fichier tmpJson.json est introuvable dans le dossier du programe ou n'a pas pus etre ouvert.\n";
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

unsigned int getActivePlayer(Json::Value root) {
	if (root.isMember("gameStateMessage") && root["gameStateMessage"].isMember("turnInfo") && root["gameStateMessage"]["turnInfo"].isMember("activePlayer")) {
		return (unsigned int) root["gameStateMessage"]["turnInfo"]["activePlayer"].isInt();
	}
	return 0;
}

string stringCleaner(string s) {
	return s.substr(1, s.length()-3); //suprime les " " et \n a la fin des stylestring
}

int stepToInt(string step, string phase) {
	if (step == "Step_BeginCombat") {
		return 4;
	} else if (step == "Step_Draw") {
		return 2;
	} else if (step == "Step_DeclareAttack") {
		return 5;
	} else if (step == "Step_DeclareBlock") {
		return 6;
	} else if (step == "Step_EndCombat") {
		return 8;
	} else if (step == "Step_CombatDamage") {
		return 7;
	} else if (step == "Step_Cleanup") {
		return 11;
	} else if (step == "Step_End") {
		return 10;
	} else if (step == "Step_Upkeep") {
		return 1;
	} else if (step == "NOTHING" && phase == "Phase_Main1") {
		return 3;
	} else if (step == "NOTHING" && phase == "Phase_Main2") {
		return 9;
	} else {
		return -1;
	}
}

unsigned int getNbLine(string fileName) {
	std::ifstream file(fileName);
	file.unsetf(std::ios_base::skipws);
	unsigned line_count = std::count(std::istream_iterator<char>(file),std::istream_iterator<char>(),'\n');
	return line_count;
}

unsigned int getNbLine(string fileName, int & fileSize) { //version je fait les deux en un
	std::ifstream file(fileName);

	file.seekg(0, file.end);
	fileSize = file.tellg();
	file.seekg(0, file.beg);
	
	file.unsetf(std::ios_base::skipws);
	unsigned line_count = std::count(std::istream_iterator<char>(file),std::istream_iterator<char>(),'\n');
	return line_count;
}

int getFileSize(string fileName) {
	std::ifstream file(fileName);
	file.seekg(0, file.end);
	int fileSize = file.tellg();
	file.seekg(0, file.beg);
	return  fileSize;
}

//Fonction de demarage du parsing.
void startParsing(Transmitter &trans) {
	string userHome = getenv("USERPROFILE");
	string folder = userHome + "/AppData/LocalLow/Wizards Of The Coast/MTGA/output_log.txt";
	getLogInformations(folder, trans); //lis les premier ligne et start l'ecoute
}