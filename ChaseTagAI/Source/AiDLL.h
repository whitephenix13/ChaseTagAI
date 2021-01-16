#pragma once
#include <utility>
#include <array>
#include "Board/CellType.h"
#include <map>
#define AIDLL_API __declspec(dllexport) 



extern "C" {
	AIDLL_API void MouseAIAction(CELL_TYPE* board,int* boardSize, float cellSize,float* cellOffset, float* mousePos, float* catPos, int actionsSize, float* outActions);
	//CELL_TYPE* board, const std::pair<int, int>& boardSize, const std::pair<int, int>& mouseCell
	//, const std::pair<int, int>& catCell, float fearStrength, float fearArea, std::vector<std::pair<int, int>>* outPath
	AIDLL_API void FindPath(CELL_TYPE* board, int* boardSize, float* mousePos, float* catPos, float fearStrength, float fearArea, float sameCellTypePenalty
		, CELL_TYPE* travelCostKeys, float* travelCostValues, int travelCostSize, int* outFlattenResultPath, int* outFlattenResultPathSize);
}
float getCellPosition(float worldPos, int boardSize, float cellSize);


