#pragma once
#include "../Board/CellType.h"
#include "AstarNode.h"
#include "AstarNodeComparator.h"
#include <utility>
#include <functional>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include "HashIntPair.h"


namespace Astar {
	bool findPath(std::pair<int, int> startCellIndex, std::pair<int, int> targetCellIndex, std::function<float(const std::pair<int, int>&, const std::pair<int, int>&)> h, CELL_TYPE* board, std::pair<int, int> boardSize, std::vector<std::pair<int, int>>* outPath);
};


