#include <thread>

#include "Components/IntegerComponent.hpp"
#include "ComponentKeeper.hpp"
#include "EntityCreator.hpp"
#include "Systems/GameSystem.hpp"
#include "Transmitter.hpp"
#include "MtGALogParser.hpp"

int main() {

	EntityCreator creator;
	ComponentKeeper keeper;

	Transmitter infoTransmitter;

	std::thread parserThread{[&infoTransmitter]() { startParsing(infoTransmitter); }};

	GameSystem gameSystem{ keeper, creator, infoTransmitter };
	gameSystem.playGame();

	Information stop;
	stop.type = InformationType::StopListen;
	infoTransmitter.addInfoForListener(stop);

	parserThread.join();

	return 0;
}