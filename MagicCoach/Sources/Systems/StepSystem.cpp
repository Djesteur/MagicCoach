#include "Systems/StepSystem.hpp"

StepSystem::StepSystem(ComponentKeeper &keeper, const Entity currentEntity): 
	System{keeper},
	m_gameEntity{currentEntity} {

	m_keeper.addEntity(m_gameEntity);

	std::shared_ptr<Component> currentTurn{newComponent(ComponentType::Integer)};
	std::static_pointer_cast<IntegerComponent>(currentTurn)->data() = 1;
	m_keeper.addComponent(currentEntity, "NumberOfTurn", currentTurn);


	std::shared_ptr<Component> currentPhase{newComponent(ComponentType::Integer)};
	std::static_pointer_cast<IntegerComponent>(currentPhase)->data() = CurrentPhase::MainPhase1;
	m_keeper.addComponent(currentEntity, "CurrentPhase", currentPhase);



	std::shared_ptr<Component> currentStep{newComponent(ComponentType::Integer)};
	std::static_pointer_cast<IntegerComponent>(currentStep)->data() = CurrentStep::MainPhaseStep1;
	m_keeper.addComponent(currentEntity, "CurrentStep", currentStep);


	std::shared_ptr<Component> currentPlayer{newComponent(ComponentType::Integer)};
	std::static_pointer_cast<IntegerComponent>(currentPlayer)->data() = 0;
	m_keeper.addComponent(currentEntity, "ActivePlayer", currentPlayer);
}

int StepSystem::getCurrentTurn() const {

	std::shared_ptr<Component> currentTurn{m_keeper.getComponent(m_gameEntity, "NumberOfTurn")};
	std::shared_ptr<IntegerComponent> currentTurnCast{std::static_pointer_cast<IntegerComponent>(currentTurn)};
	return currentTurnCast->data();
}

int StepSystem::getCurrentStep() const {

	std::shared_ptr<Component> currentStep{m_keeper.getComponent(m_gameEntity, "CurrentStep")};
	std::shared_ptr<IntegerComponent> currentStepCast{std::static_pointer_cast<IntegerComponent>(currentStep)};
	return currentStepCast->data();
}

int StepSystem::getCurrentPhase() const {

	std::shared_ptr<Component> currentPhase{m_keeper.getComponent(m_gameEntity, "CurrentPhase")};
	std::shared_ptr<IntegerComponent> currentPhaseCast{std::static_pointer_cast<IntegerComponent>(currentPhase)};
	return currentPhaseCast->data();
}

int StepSystem::getActivePlayer() const {

	std::shared_ptr<Component> currentPlayer{m_keeper.getComponent(m_gameEntity, "ActivePlayer")};
	std::shared_ptr<IntegerComponent> currentPlayerCast{std::static_pointer_cast<IntegerComponent>(currentPlayer)};
	return currentPlayerCast->data();
}

void StepSystem::nextStep() {

	std::shared_ptr<Component> currentStep{m_keeper.getComponent(m_gameEntity, "CurrentStep")};
	std::shared_ptr<IntegerComponent> currentStepCast{std::static_pointer_cast<IntegerComponent>(currentStep)};

	std::shared_ptr<Component> currentPhase{m_keeper.getComponent(m_gameEntity, "CurrentPhase")};
	std::shared_ptr<IntegerComponent> currentPhaseCast{std::static_pointer_cast<IntegerComponent>(currentPhase)};

	int &thisStep{currentStepCast->data()}, &thisPhase{currentPhaseCast->data()};
	thisStep++;

	if(thisStep > CurrentStep::CleanupStep) { 

		//New turn

		thisStep = CurrentStep::UntapStep;

		std::shared_ptr<Component> currentTurn{m_keeper.getComponent(m_gameEntity, "NumberOfTurn")};
		std::shared_ptr<IntegerComponent> currentTurnCast{std::static_pointer_cast<IntegerComponent>(currentTurn)};
		currentTurnCast->data()++;

		std::shared_ptr<Component> currentPlayer{m_keeper.getComponent(m_gameEntity, "ActivePlayer")};
		std::shared_ptr<IntegerComponent> currentPlayerCast{std::static_pointer_cast<IntegerComponent>(currentPlayer)};
		if(currentPlayerCast->data() == 0) { currentPlayerCast->data() = 1; }
		else { currentPlayerCast->data() = 0; }
	}

	if(thisStep <= CurrentStep::DrawStep) { thisPhase = CurrentPhase::BeginningPhase; }
	else if(thisStep == CurrentStep::MainPhaseStep1) { thisPhase = CurrentPhase::MainPhase1; }
	if(thisStep >= CurrentStep::BeginCombat && thisStep <= CurrentStep::EndCombatStep) { thisPhase = CurrentPhase::CombatPhase; }
	if(thisStep == CurrentStep::MainPhaseStep2) { thisPhase = CurrentPhase::MainPhase2; }
	if(thisStep >= CurrentStep::EndStep) { thisPhase = CurrentPhase::EndPhase; }
}