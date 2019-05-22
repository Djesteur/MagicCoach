#include "MtGALogParser.hpp"

/*Parser général*/

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
bool getLogInformations(string fileName, Transmitter &trans, bool debug) {

	int lastPosition = 0;
	unsigned int lastLine = getNbLine(fileName, lastPosition);

	if (debug) {
		cout << "Parser: " << lastLine << " lignes Skip ! | File size : " << lastPosition << "\n";
		cout << "Parser: Passage en ecoute. \n";
	}

	clock_t start = clock();
	clock_t lastTick = start;
	bool onMatch = false;

	Information lastinfo;
	lastinfo.type = InformationType::PlayCard;
	int passe = 0;
	int nbClock = 0;
	while (lastinfo.type != InformationType::StopListen) {
		if (((clock() - lastTick) / (double)CLOCKS_PER_SEC) >= 1) { //Mettre 10 sec en temps normal si bug encore en partie
			lastTick = clock();

			//unsigned int nbLineFile = getNbLine(fileName);

			//if (lastLine < nbLineFile) {
			if (lastPosition < getFileSize(fileName)){
				//cout << "Nb line file : " << nbLineFile << "\n";
				if (debug) {
					cout << "Parser: Passe num: " << passe << "\n";
				}
				secondePasse(fileName, lastPosition, lastLine, onMatch, trans, debug);
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

bool secondePasse(string fileName, int & lastPosition ,unsigned int & lastLine, bool & onMatch, Transmitter &trans, bool debug) {

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
			if (debug) {
				cout << "Parser: Debut de match detecter ! Passage en parsing match.\n";
			}
		}

		/*
		Ici autre "[Client" utile
		*/

		//On jump une fois les "[Client" possiblement utile lus.
		if (!jumpLine(line)) {
			//cout << "Debug position befor getJson : " << lastPosition << "\n";

			if (line[0] == '{' || (line[0] == '[')) {//(line.find('{')!=string::npos){
				//nbJson++;
				if (!getJSON(line, file, lastLine, lastPosition)) {
					file.close();
					return false;
				}

				for (string s : getMessageInJson(debug)) {
					JsonMessage m = JsonMessage(s, debug);
					m.getAction(trans);
				}
			} else {
			}
		}
		


	}//fin pour ligne
	file.close();

	int nbLineRead = (lastLine - saveLastLine);
	if (debug) {
		cout << "Parser: Ligne lues pendant la passe : " << nbLineRead << " | File size: " << lastPosition << "\n";
	}
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

vector<string> getMessageInJson(bool debug) {

	ifstream tmpJson;
	tmpJson.open("tmpJson.json");
	if (!tmpJson) {
		cout << "Parser: Le fichier tmpJson.json est introuvable dans le dossier du programe ou n'a pas pus etre ouvert.\n";
	}

	vector<string> messages;

	for (string line; getline(tmpJson, line);) {

		size_t pos = line.find("greToClientMessages"); //on trouve des message
		
		int nbCrochet = 0;

		bool messageTrouver = false;

		if (pos != string::npos || messageTrouver) { //si on viens de trouver ou si on n'a déjà trouver alors c'est ok

			int nbMessage = 0;
			messageTrouver = true;

			nbCrochet = 1;
			
			bool inArray = true;
			int nbAccolade = 0;
			bool inJson = false;
			string jsonEnCours = "";

			do {
				getline(tmpJson, line); //on prend la ligne suivante au moins une fois
				for (char& c : line) {//On lis char par char la ligne
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
					if (c == '}') {
						nbAccolade--;
						if (nbAccolade <= 0) {
							nbAccolade = 0;
							messages.push_back(jsonEnCours);
							jsonEnCours = "";
							inJson = false;
						}
					} else if (c == ']') {
						nbCrochet--;
						if (nbCrochet <= 0) { //si on arrive a 0 c'est que on a lus tous les messages du tableau donc messageTrouver a false car on est plus dans des message
							nbCrochet = 0;
							messageTrouver = false;
						}
					}

				}//fin pour char

			} while (inJson || nbCrochet); 	//fin pour ligne
			
		}
	}//Line par line de tmpJson.json
	
	tmpJson.close();
	return messages;
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
void startParsing(Transmitter &trans, bool debug) {
	string userHome = getenv("USERPROFILE");
	string folder = userHome + "/AppData/LocalLow/Wizards Of The Coast/MTGA/output_log.txt";
	getLogInformations(folder, trans, debug); //lis les premier ligne et start l'ecoute
}