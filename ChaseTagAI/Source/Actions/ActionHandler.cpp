#include "../Pch/pch.h"
#include "ActionHandler.h"

const void ActionHandler::moveTowards(const std::pair<int, int>& startCell, const std::pair<int, int>& targetCell, float* outAction) {
	//Inspect columns values 
	startCell.second < targetCell.second ? ActionHandler::moveRight(outAction) :
		(startCell.second == targetCell.second ? ActionHandler::waitOnHorizontalAxis(outAction) : ActionHandler::moveLeft(outAction));
	//Inspect lines values
	startCell.first < targetCell.first ? ActionHandler::moveDown(outAction) :
		(startCell.first == targetCell.first ? ActionHandler::waitOnVerticalAxis(outAction) : ActionHandler::moveUp(outAction));
}

void ActionHandler::moveLeft(float* outActions) {
	outActions[0] = -1;
}
void ActionHandler::moveRight(float* outActions) {
	outActions[0] = 1;
}
void ActionHandler::waitOnHorizontalAxis(float* outActions){
	outActions[0] = 0;
}
void ActionHandler::moveUp(float* outActions) {
	outActions[1] = 1;
}
void ActionHandler::moveDown(float* outActions) {
	outActions[1] = -1;
}
void ActionHandler::waitOnVerticalAxis(float* outActions) {
	outActions[1] = 0;
}

const bool ActionHandler::isMoveLeft(const float* outActions) {
	return outActions[0] ==-1;
}
const bool ActionHandler::isMoveRight(const float* outActions) {
	return outActions[0] == 1;
}
const bool ActionHandler::isWaitOnHorizontalAxis(const float* outActions) {
	return outActions[0] == 0;
}
const bool ActionHandler::isMoveUp(const float* outActions) {
	return outActions[1] == 1;
}
const bool ActionHandler::isMoveDown(const float* outActions) {
	return outActions[1] == -1;
}
const bool ActionHandler::isWaitOnVerticalAxis(const float* outActions) {
	return outActions[1] == 0;
}
