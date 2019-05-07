#include "Transmitter.hpp"

Transmitter::Transmitter() {}

Transmitter::~Transmitter() {

	m_toCoachMutex.lock();
	while (!m_infoToCoach.empty()) { m_infoToCoach.pop(); }
	m_toCoachMutex.unlock();

	m_toListenerMutex.lock();
	while (!m_infoToListener.empty()) { m_infoToListener.pop(); }
	m_toListenerMutex.unlock();

}

bool Transmitter::waitingInfoForCoach() { return !m_infoToCoach.empty(); }

void Transmitter::addInfoForCoach(Information info) {

	m_toCoachMutex.lock();
	m_infoToCoach.emplace(info);
	m_toCoachMutex.unlock();
}

Information Transmitter::getCoachInfo() {

	m_toCoachMutex.lock();
	Information newInfo{ m_infoToCoach.front() };
	m_infoToCoach.pop();
	m_toCoachMutex.unlock();
	return newInfo;
}

bool Transmitter::waitingInfoForListener() { return !m_infoToListener.empty(); }

void Transmitter::addInfoForListener(Information info) {

	m_toListenerMutex.lock();
	m_infoToListener.emplace(info);
	m_toListenerMutex.unlock();
}

Information Transmitter::getListenerInfo() {

	m_toListenerMutex.lock();
	Information newInfo{ m_infoToListener.front() };
	m_infoToListener.pop();
	m_toListenerMutex.unlock();
	return newInfo;
}