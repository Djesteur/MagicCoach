#ifndef TRANSMITTER_HPP
#define TRANSMITTER_HPP

#include <vector>
#include <mutex>
#include <queue>

enum class InformationType {

	PlayCard,
	Draw,
	CurrentStep,
	DeclaringAttackers,
	DeclaringBlockers,
	StopListen,
	GameOver,
	SynchroID,//instanceid, grpid(mtgaid), owner, controller
	MatchOver
};

struct Information {

	unsigned int player;
	std::vector<int> values;
	InformationType type;
};

class Transmitter {

	public:

		Transmitter();
		~Transmitter();

		Transmitter(const Transmitter&) = delete;
		Transmitter &operator=(const Transmitter&) = delete;

		bool waitingInfoForCoach();
		void addInfoForCoach(Information info);
		Information getCoachInfo();

		bool waitingInfoForListener();
		void addInfoForListener(Information info);
		Information getListenerInfo();

	private:

		std::queue<Information> m_infoToCoach, m_infoToListener;
		std::mutex m_toCoachMutex, m_toListenerMutex;
};

#endif