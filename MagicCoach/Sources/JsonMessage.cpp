#include "JsonMessage.hpp"


using namespace std;

JsonMessage::JsonMessage(string json) {
	message = json;
	parsingSuccess = reader.parse(json, root);
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
			cout << "Phase: " << phase << " | ";
			cout << "Step : " << step << "\n";
		} else if (getGameOver()) {
			info.type = InformationType::GameOver;
			cout << "Game over \n";
		} else if (getMatchOver()) {
			info.type = InformationType::MatchOver;
			cout << "Match over \n";
		} else if (root.isMember("gameStateMessage") && root["gameStateMessage"].isMember("gameObjects")) {
			for (array<int, 4> obj : getGameObjects()) {
				Information infoObj = Information();
				infoObj.player = info.player;
				infoObj.values.push_back(obj[0]);
				infoObj.values.push_back(obj[1]);
				infoObj.values.push_back(obj[2]);
				infoObj.values.push_back(obj[3]);
				infoObj.type = InformationType::SynchroID;
				trans.addInfoForCoach(infoObj);
			}
			//on quit ici car pas plus pour ce message
			return true;
		} else if (root.isMember("gameStateMessage") && root["gameStateMessage"].isMember("action")) {
			for (int obj : getCast()) {
				Information infoObj = Information();
				infoObj.player = info.player;
				infoObj.values.push_back(obj);
				infoObj.type = InformationType::PlayCard;
				trans.addInfoForCoach(infoObj);
			}
			return true;
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

vector<array<int, 4>> JsonMessage::getGameObjects() {
	vector<array<int, 4>> objs = vector<array<int, 4>>();
	if (root.isMember("gameStateMessage") && root["gameStateMessage"].isMember("gameObjects")) {
		for (Json::Value obj; root["gameStateMessage"]["gameObjects"];) {
			if (obj.isMember("instanceId") && obj.isMember("grpId") && obj.isMember("ownerSeatId") && obj.isMember("controllerSeatId") ) {
				array<int, 4> valObj = {
					obj["instanceId"].asInt(),
					obj["grpId"].asInt(),
					obj["ownerSeatId"].asInt(),
					obj["controllerSeatId"].asInt()
				};
				objs.push_back(valObj);
			}
		}
	}
	return objs;
}

vector<int> JsonMessage::getCast() {
	vector<int> objs = vector<int>();
	if (root.isMember("gameStateMessage") && root["gameStateMessage"].isMember("action")) {
		for (Json::Value obj; root["gameStateMessage"]["actions"];) {
			if (obj.isMember("seatId") && obj.isMember("action") && obj["action"].isMember("instanceId")) {
				if (obj["action"].isMember("actionType") && root["action"]["actionType"].asString() == "ActionType_Cast") {
					objs.push_back(obj["action"]["instanceId"].asInt());
				}
			}
		}
	}
	return objs;
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
