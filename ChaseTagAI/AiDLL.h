#pragma once
#include <utility>
#include <array>
#define AIDLL_API __declspec(dllexport) 

enum CELL_TYPE {
	EMPTY, WALL
};

extern "C" {
	AIDLL_API void MouseAIAction(CELL_TYPE* board,int* boardSize, float cellSize,float* cellOffset, float* mousePos, float* catPos, int actionsSize, float* outActions);
}
float getCellPosition();


