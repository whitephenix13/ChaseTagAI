#pragma once
#include <utility>
#include <array>
#include "Board/CellType.h"
#define AIDLL_API __declspec(dllexport) 



extern "C" {
	AIDLL_API void MouseAIAction(CELL_TYPE* board,int* boardSize, float cellSize,float* cellOffset, float* mousePos, float* catPos, int actionsSize, float* outActions);
}
float getCellPosition(float worldPos, int boardSize, float cellSize);


