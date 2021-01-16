#pragma once
#include "../../Board/CellType.h"
#include <map>

namespace FleePathFinding {
	void execute(CELL_TYPE* board, const std::pair<int, int>& boardSize, std::map<CELL_TYPE, float> travelCostMap, const std::pair<int, int>& mouseCell
		, const std::pair<int, int>& catCell, float fearStrength, float fearArea, float sameCellTypePenalty
		, std::vector<std::pair<int, int>>* outPath);

}