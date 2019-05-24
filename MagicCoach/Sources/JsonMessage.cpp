#include "JsonMessage.hpp"

/*Class utiliser pour parser les message des logs*/

using namespace std;

JsonMessage::JsonMessage(string json, bool deb) {
	message = json;
	parsingSuccess = reader.parse(json, root);
	debug = deb;
}

bool JsonMessage::getAction(Transmitter & trans) {

	Information info = Information();

	bool parsingSuccessful = reader.parse(message, root);
	if (!parsingSuccessful) {
		cout << "Parser: Error parsing the string" << endl;
	} else {

		info.player = getActivePlayer();
		/*if (getGameStageStart) {
			cout << "Stage start !\n";
		}*/
		string phase = getPhase();
		string step = getStep();
		if (phase != "NOTHING") {
			info.type = InformationType::CurrentStep;
			info.values.push_back(stepToInt(step, phase));
			if (debug) {
				cout << "Phase: " << phase << " | ";
				cout << "Step : " << step << "\n";
			}
		} else if (getGameOver()) {
			info.type = InformationType::GameOver;
			if (debug) {
				cout << "Game over \n";
			}
		} else if (getMatchOver()) {
			info.type = InformationType::MatchOver;
			if (debug) {
				cout << "Match over \n";
			}
		} else if (root.isMember("gameStateMessage") && root["gameStateMessage"].isMember("gameObjects")) {
			array<vector<vector<int>>, 2> objs = getGameObjects(); /* [0] : SynchroID | [1] : Attaquant | Qui contient une liste de chaque carte qui contient elle même une liste des info de dedant : type info -> carte -> info carte */
			if (objs[0].size() > 0) {
				for (vector<int> obj : objs[0]) { //Synchro des id
					Information infoObj = Information();
					infoObj.player = info.player;
					infoObj.values.push_back(obj[0]); // Instanceid
					infoObj.values.push_back(obj[1]); // grpid
					infoObj.values.push_back(obj[2]); // Owner
					infoObj.values.push_back(obj[3]); // controller
					infoObj.type = InformationType::SynchroID;
					if (debug) {
						cout << "Parser: SynchroID, InstanceId : " << obj[0] << " | grpid : " << obj[1] << "\n";
					}
					trans.addInfoForCoach(infoObj);
				}
				if (objs[1].size() > 0) {
					for (vector<int> obj : objs[1]) { //Attaquant
						Information infoObj = Information();
						infoObj.player = info.player;
						infoObj.values.push_back(obj[0]); // InstanceID
						infoObj.values.push_back(obj[1]); // Bool bloquer ou non
						if (debug) {
							cout << "Parser: Attacking InstanceId : " << obj[0];
						}
						if (obj[1] == 1) { // si la carte est bloquer
							if (debug) {
								cout << " | blocker : ";
							}
							for (int j = 2; j < obj.size(); j++) {
								if (debug) {
									cout << obj[j] << " | ";
								}
								infoObj.values.push_back(obj[j]); // on met tous les bloquer dans l'ordre a la suite
							}
						}
						if (debug) {
							cout << "\n";
						}
						infoObj.type = InformationType::AttackingAttackers;
						trans.addInfoForCoach(infoObj);
					}
				}
				//on quit ici car pas plus pour ce message
				return true;
			}
		} /*else if (root.isMember("gameStateMessage") && root["gameStateMessage"].isMember("actions")) {
			vector<int> objs = getCast();
			if (objs.size() > 0) {
				for (int obj : objs) {
					Information infoObj = Information();
					infoObj.player = info.player;
					infoObj.values.push_back(obj);
					infoObj.type = InformationType::PlayCard;
					if (debug) {
						cout << "Parser: Play card, InstanceID " << obj << "\n";
					}
					trans.addInfoForCoach(infoObj);
				}
				return true;
			}
		} */else if (root.isMember("declareAttackersReq") && root["declareAttackersReq"].isMember("attackers")) {//Declare attack
			vector<int> objs = getDeclareAttack();
			if (objs.size() > 0) {
				Information infoObj = Information();
				infoObj.player = info.player;
				infoObj.type = InformationType::DeclaringAttackers;
				if (debug) {
					cout << "Parser: Declare attack, InstanceID : ";
				}
				for (int obj : objs) {
					infoObj.values.push_back(obj);
					if (debug) { cout << obj << " | "; }
				}
				if (debug) { cout << "\n"; }
				trans.addInfoForCoach(infoObj);
				return true;
			}
		} else {
			return false; //rien trouver
		}
	}

	trans.addInfoForCoach(info);

	return true;
}

string JsonMessage::getStep() {
	if (root.isMember("gameStateMessage")) {
		if (root["gameStateMessage"].isMember("turnInfo")) {
			if (root["gameStateMessage"]["turnInfo"].isMember("step")) {
				return root["gameStateMessage"]["turnInfo"]["step"].asString();
			}
		}
	}
	return "NOTHING";
}

string JsonMessage::getPhase() {
	if (root.isMember("gameStateMessage")) {
		if (root["gameStateMessage"].isMember("turnInfo")) {
			if (root["gameStateMessage"]["turnInfo"].isMember("phase")) {
				return root["gameStateMessage"]["turnInfo"]["phase"].asString();
			}
		}
	}
	return "NOTHING";
}

bool JsonMessage::getGameOver() {
	if (root.isMember("gameStateMessage")) {
		if (root["gameStateMessage"].isMember("gameInfo")) {
			if (root["gameStateMessage"]["gameInfo"].isMember("stage")) {
				string end = root["gameStateMessage"]["gameInfo"]["stage"].asString();
				if (end == "GameStage_GameOver") {
					return true;
				}
			}
		}
	}
	return false;
}

bool JsonMessage::getMatchOver() {
	if (root.isMember("gameStateMessage")) {
		if (root["gameStateMessage"].isMember("gameInfo")) {
			if (root["gameStateMessage"]["gameInfo"].isMember("matchState")) {
				string end = root["gameStateMessage"]["gameInfo"]["matchState"].asString();
				if (end == "MatchState_GameComplete") {
					return true;
				}
			}
		}
	}
	return false;
}

bool JsonMessage::getGameStageStart() {
	if (root.isMember("gameStateMessage")) {
		if (root["gameStateMessage"].isMember("gameInfo")) {
			if (root["gameStateMessage"]["gameInfo"].isMember("stage")) {
				string end = root["gameStateMessage"]["gameInfo"]["stage"].asString();
				if (end == "GameStage_Start") {
					return true;
				}
			}
		}
	}
	return false;
}

unsigned int JsonMessage::getActivePlayer() {
	if (root.isMember("gameStateMessage") && root["gameStateMessage"].isMember("turnInfo") && root["gameStateMessage"]["turnInfo"].isMember("activePlayer")) {
		return (unsigned int)root["gameStateMessage"]["turnInfo"]["activePlayer"].asInt();
	}
	return 0;
}

array<vector<vector<int>>, 2> JsonMessage::getGameObjects() {
	array<vector<vector<int>>, 2> valeurs = array<vector<vector<int>>, 2>();
	if (root.isMember("gameStateMessage") && root["gameStateMessage"].isMember("gameObjects")) {
		if (debug) {
			cout << "Parser: gameObjects found \n";
		}
		Json::Value objs = root["gameStateMessage"]["gameObjects"];
		for (int i = 0; i < objs.size(); i++) {
			if (objs[i].isMember("instanceId") && objs[i].isMember("grpId") && objs[i].isMember("ownerSeatId") && objs[i].isMember("controllerSeatId") ) {
				vector<int> valObj = {
					objs[i]["instanceId"].asInt(),
					objs[i]["grpId"].asInt(),
					objs[i]["ownerSeatId"].asInt(),
					objs[i]["controllerSeatId"].asInt()
				};
				valeurs[0].push_back(valObj);
				//gestion des attaquant
				if (objs[i].isMember("attackState") && objs[i]["attackState"].asString() == "AttackState_Attacking") { // si la carte attaque AttackState_Declared
					vector<int> valObjDeux = vector<int>();
					valObjDeux.push_back(objs[i]["instanceId"].asInt());
					if (objs[i].isMember("blockState") && objs[i]["blockState"].asString() == "BlockState_Blocked" && objs[i].isMember("attackInfo") && objs[i]["attackInfo"].isMember("orderedBlockers")) { // si la carte est bloquer
						valObjDeux.push_back(1);
						for (Json::Value idBlock : objs[i]["attackInfo"]["orderedBlockers"]) {
							valObjDeux.push_back(idBlock["instanceId"].asInt());
						}
					} else {
						valObjDeux.push_back(0);
					}
					valeurs[1].push_back(valObj);//on ajoute dans la liste des attaquant
				}
			}
		}
	}
	return valeurs;
}

vector<int> JsonMessage::getCast() {
	vector<int> valeurs = vector<int>();
	if (root.isMember("gameStateMessage") && root["gameStateMessage"].isMember("actions")) {
		if (debug) {
			cout << "Parser: action found\n";
		}
		Json::Value objs = root["gameStateMessage"]["actions"];
		for (int i = 0; i < objs.size(); i++) {
			if (objs[i].isMember("action") && objs[i]["action"].isMember("instanceId")) {
				if (objs[i]["action"].isMember("actionType") && objs[i]["action"]["actionType"].asString() == "ActionType_Cast") {
					valeurs.push_back(objs[i]["action"]["instanceId"].asInt());
				}
			}
		}
	}
	return valeurs;
}

vector<int> JsonMessage::getDeclareAttack() {
	vector<int> valeurs = vector<int>();
	if (root.isMember("declareAttackersReq") && root["declareAttackersReq"].isMember("attackers")) {
		if (debug) {
			cout << "Parser: action found\n";
		}
		Json::Value objs = root["declareAttackersReq"]["attackers"];
		for (int i = 0; i < objs.size(); i++) {
			if (objs[i].isMember("attackerInstanceId")) {
				valeurs.push_back(objs[i]["attackerInstanceId"].asInt());
			}
		}
	}
	return valeurs;
}

int JsonMessage::stepToInt(string step, string phase) {
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

JsonMessage::~JsonMessage() {
}
