#pragma once
#include "../Board/CellType.h"
#include <utility>
#include <functional>
#include <vector>
namespace AstarDataOriented {
	bool findPath(const std::pair<int, int>& startCell, const std::pair<int, int>& targetCell, const std::function<float(const std::pair<int, int>&, const std::pair<int, int>&)> h, const CELL_TYPE* board, const std::pair<int, int>& boardSize, std::vector<std::pair<int, int>>* outPath);

};