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

bool getLogInformations(string, bool*);
bool skipLine(int, ifstream &);
bool getJSON(string, ifstream &, ofstream &,int&);
bool jumpLine(string);
bool secondePasse(ifstream &, ofstream*,int&);
int choixSortie(string);

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

	int const tailleList = 3;
	string utilityList[tailleList];// A OPTIMISER
	utilityList[0] = (string)"<== Deck.GetDeckLists(";
	utilityList[1] = (string)"<== PlayerInventory.GetPlayerCardsV3(";
    utilityList[2] = (string)"[Client GRE]";

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
bool getLogInformations__old(string fileName, bool *continuerProgramme) {

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
		cout << "Le fichier 'log_JSON.json' n'a pas pus être créé.\n";
		return false;
	}

	ofstream deckFile("Parsed_Files/deck.json", ofstream::out);
	if (!jsonFile) {
		cout << "Le fichier 'deck.json' n'a pas pus être créé.\n";
		return false;
	}

	ofstream carteFile("Parsed_Files/carte.json", ofstream::out);
	if (!jsonFile) {
		cout << "Le fichier 'carte.json' n'a pas pus être créé.\n";
		return false;
	}

	ofstream matchFile("Parsed_Files/match.json", ofstream::out);
	if (!jsonFile) {
		cout << "Le fichier 'match.json' n'a pas pus être créé.\n";
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
                    case 2:
                        matchFile << c;
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
	matchFile.close();

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

	thread parsing(getLogInformations, "output_log_1game.txt", &continuerProgramme);
	thread comm(readingCommande, &continuerProgramme);
	parsing.join();
	comm.join();


	//closeTabFile(tabFile);

	return EXIT_SUCCESS;
}


bool skipLine(int n, ifstream & fichier){
	string line;
	bool nAtteind= false;
	for ( int i = 0; i<n && !fichier.eof() ; i++){
		getline(fichier ,line);
		if (i==n-1){nAtteind=true;};
	}
	//return (nAtteind);
	return !(fichier.eof()); // retourne vrai si fichier n'a pas atteind la fin.
}

bool getJSON(string l1,ifstream & fichier, ofstream & sortie, int & ligneLues){

	int nbAccolade = 0;
	int nbCrochet = 0;
	bool inJson = false;
	bool inArray = false;
	bool ligne1 =true;
	string line = l1;

	do {
		if (ligne1) {ligne1=false;}
		else {getline(fichier,line);ligneLues++;}
		for (char& c : line) {//On lit char par char la ligne

			if (c == '{') {
				nbAccolade++;
				inJson= true;
			}else if (c == '[') {
					nbCrochet++;
					inArray=true;
				}
			//On ecrit dans le fichier des JSON sinon dans autre ligne
			sortie << c;
			//On recher les } si c'était la dernière du JSON on "sort" du JSON
			if (c == '}') {
				nbAccolade--;
				if (nbAccolade <= 0) {
					nbAccolade = 0;
					inJson = false;
				}
			}else if(c == ']') {
					nbCrochet--;
					if (nbCrochet <= 0) {
						nbCrochet = 0;
						inArray =false;
						//onUtile = -1; //On reset onUtile
					}
				}

		}//fin pour char
	} while (inJson || inArray);
	//fin pour ligne
	return true;
}
bool jumpLine(string line) {

	int const tailleList = 5;
	string listJump[tailleList];
	listJump[0] = (string)"[Unity";
	listJump[1] = (string)"[Store";
	listJump[2] = (string)"[Discord";
	listJump[3] = (string)"[Account";
	listJump[4] = (string)"[Social";
	//listJump[5] = (string)"[Client";

	for (string l: listJump) {
		size_t foundJump = line.find(l);
		if (foundJump != string::npos) {
			return true;
		}
	}

	return false;
}
bool getLogInformations(string fileName, bool* continuerProgramme){

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
	sorties[1].open("Parsed_Files/log_JSON.json",ofstream::out);
	if (!sorties[1]) {
		cout << "Le fichier 'log_JSON.json' n'a pas pus être créé.\n";
		return false;
		}
	sorties[0].open("Parsed_Files/discard.txt", ofstream::out);
	if (!sorties[0]) {
		cout << "Le fichier 'discard.txt' n'a pas pus être créé.\n";
		return false;
	}
		/*
	ofstream jsonFile("Parsed_Files/log_JSONtest.json", ofstream::out);
	if (!jsonFile) {
		cout << "Le fichier 'log_JSON.json' n'a pas pus être créé.\n";
		return false;
	}
	ofstream discardFile("Parsed_Files/discard.txt", ofstream::out);
	if (!discardFile) {
		cout << "Le fichier 'discard.txt' n'a pas pus être créé.\n";
		return false;
	}
*/
	sorties[2].open("Parsed_Files/deck.json",ofstream::out);
	if (!sorties[2]) {
		cout << "Le fichier 'deck.json' n'a pas pus être créé.\n";
		return false;
		}
	sorties[3].open("Parsed_Files/carte.json",ofstream::out);
	if (!sorties[3]) {
		cout << "Le fichier 'carte.json' n'a pas pus être créé.\n";
		return false;
		}	
	sorties[4].open("Parsed_Files/match.json",ofstream::out);
	if (!sorties[4]) {
		cout << "Le fichier 'match.json' n'a pas pus être créé.\n";
		return false;
		}
	/*
	ofstream deckFile("Parsed_Files/deck.json", ofstream::out);
	if (!deckFile) {
		cout << "Le fichier 'deck.json' n'a pas pus être créé.\n";
		return false;
	}

	ofstream carteFile("Parsed_Files/carte.json", ofstream::out);
	if (!carteFile) {
		cout << "Le fichier 'carte.json' n'a pas pus être créé.\n";
		return false;
	}

	ofstream matchFile("Parsed_Files/match.json", ofstream::out);
	if (!matchFile) {
		cout << "Le fichier 'match.json' n'a pas pus être créé.\n";
		return false;
	}
	*/
	/*jsonFile*/sorties[1] << "{\n \"jsonList\" : [\n";
	sorties[2] << "{\n \"jsonList\" : [\n";
	sorties[3] << "{\n \"jsonList\" : [\n";
	sorties[4] << "{\n \"jsonList\" : [\n";

	cout << "Parsing launched ! \n";

	int nbJson =0;
	int ligneLues = 0;
	int sortie = 1;

	for (string line; getline(file, line) ;) {
		ligneLues++;
		//if (checkLigneSkip(line)){getline(file,line);n++;}
		if (jumpLine(line)) {sorties[0]<<line;getline(file,line);ligneLues++;}

		sortie = choixSortie(line);
		if (sortie != 1){sorties[0]<<line;getline(file,line);ligneLues++;}
		if (line.find("Event.MatchCreated") != string::npos){sorties[0]<<line; sortie = 4; line= "{";}

		if (line[0] == '{' || (line[0]=='[')){//(line.find('{')!=string::npos){
			nbJson++;
			getJSON(line,file,sorties[sortie],ligneLues);
			sorties[sortie]<<",\n";
		}else {sorties[0]<<line;}
		sortie = 1;
		
	}//fin pour ligne
	ligneLues++; //ca compte 1 en moins !?
	cout<<ligneLues<<"lignes lues !\n";
	cout << "Fin de lecture du début !\n Passage en écoute. \n";

	clock_t start = clock();
	clock_t lastTick = start;
	while (*continuerProgramme) { //AJOUTER LECTURE LIGNE PAR LIGNE APRES PREMIRE LECTURE ! ... Je sais pas comment faire pour le moment
		if (((clock() - lastTick) / (double)CLOCKS_PER_SEC) == 5) {
			lastTick = clock();
			cout << "POKE ! \n";
			ifstream file2;
			file2.open(fileName);
			if (!file2) {
			cout << "Le fichier est introuvable dans le dossier du programe ou n'a pas pus etre ouvert.\n";
			return false;
			}
			if (skipLine(ligneLues,file2)) {
				cout<<"seconde passe \n";
				secondePasse(file2,sorties,ligneLues);
				cout<<ligneLues<<"lignes lu au total";
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
	
	
	/*jsonFile.close();
	discardFile.close();*/
	/*
	deckFile.close();
	carteFile.close();
	matchFile.close();
	*/
	exit(0); // On fermme comme il faut

	return true;

}

bool secondePasse(ifstream & entree, ofstream* sorties, int & lignesLues){

	for (string line; getline(entree, line);) {
		lignesLues++;
		//if (checkLigneSkip(line)){getline(file,line);n++;}
		if (jumpLine(line)) {sorties[0]<<line;getline(entree,line);lignesLues++;}
		if (line[0] == '{' || (line[0]=='[')){//(line.find('{')!=string::npos){
			//nbJson++;
			getJSON(line,entree,sorties[1],lignesLues);
			sorties[1]<<",\n";
		}else {sorties[0]<<line;}

	}//fin pour ligne
	return true;

}

int choixSortie(string line){
	int const tailleList = 3;
	string utilityList[tailleList];// A OPTIMISER
	utilityList[0] = (string)"<== Deck.GetDeckLists(";
	utilityList[1] = (string)"<== PlayerInventory.GetPlayerCardsV3(";
    utilityList[2] = (string)"[Client GRE]";

	int i = 0;
	for (string l : utilityList) {
		size_t foundUtile = line.find(l);
		if (foundUtile != string::npos) {
			return i+2;
		}
		i++;
	}

	return 1;//pas a trier
}