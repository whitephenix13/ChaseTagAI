#pragma once
#include "../Board/CellType.h"
#include <utility>
#include <functional>
#include <vector>
#include <map>
namespace AstarDataOriented {
	void findPaths(const std::pair<int, int>& startCell, const std::pair<int, int>* targetCells,const int nbTargetCells, const std::function<float(const std::pair<int, int>&
		, const std::pair<int, int>&)> h, const CELL_TYPE* board, const std::pair<int, int>& boardSize, std::map<CELL_TYPE, float> travelCostMap, std::vector<std::pair<int, int>>* outPaths, bool* outSuccess);
	void findPaths(const std::pair<int, int>& startCell, const std::pair<int, int>* targetCells, const int nbTargetCells, const std::function<float(const std::pair<int, int>&
		, const std::pair<int, int>&)> h, float fearStrength, float fearArea, std::pair<int, int> cellToFlee, float sameCellTypePenalty, const CELL_TYPE* board, const std::pair<int, int>& boardSize,
		std::map<CELL_TYPE, float> travelCostMap, std::vector<std::pair<int, int>>* outPaths, bool* outSuccess);
};