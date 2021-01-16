#pragma once
#include <utility>
namespace ActionHandler {
	const void moveTowards(const std::pair<int, int>& startCell, const std::pair<int, int>& targetCell, float* outAction);

	void moveLeft(float* outActions);
	void moveRight(float* outActions);
	void waitOnHorizontalAxis(float* outActions);
	void moveUp(float* outActions);
	void moveDown(float* outActions);
	void waitOnVerticalAxis(float* outActions);

	const bool isMoveLeft(const float* outActions);
	const bool isMoveRight(const float* outActions);
	const bool isWaitOnHorizontalAxis(const float* outActions);
	const bool isMoveUp(const float* outActions);
	const bool isMoveDown(const float* outActions);
	const bool isWaitOnVerticalAxis(const float* outActions);
}