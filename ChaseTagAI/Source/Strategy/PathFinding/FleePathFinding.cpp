#include "../../Pch/pch.h"
#include "../../AstarDataOriented/AstarDataOriented.h"
#include "../../Actions/ActionHandler.h"
#include "FleePathFinding.h"
#include <cmath>
namespace {
	float sqrt2 = sqrt(2);

	float defaultHeuristic(const std::pair<int, int>& currentPos, const std::pair<int, int>& targetPos) {
		return sqrt(pow(currentPos.first - targetPos.first, 2) + pow(currentPos.second - targetPos.second, 2));
	}
	float getPathTotalDistance(const std::vector<std::pair<int, int>>& path) {
		//Assume that cell in the path or either adjacent (distance of one) or diagonal (distance of sqrt(2))
		float distance = 0;
		for (unsigned int i = 1; i < path.size(); ++i) {
			distance += ((path.at(i - 1).first == path.at(i).first) || (path.at(i - 1).second == path.at(i).second)) ? 1 : sqrt2;
		}
		return distance;
	}
}
void FleePathFinding::execute(CELL_TYPE* board, const std::pair<int, int>& boardSize, std::map<CELL_TYPE, float> travelCostMap, const std::pair<int, int>& mouseCell
	, const std::pair<int, int>& catCell,float fearStrength, float fearArea, float sameCellTypePenalty, std::vector<std::pair<int, int>>* outPath) {
	const int NB_TARGET = boardSize.first * boardSize.second;

	std::vector<std::pair<int, int>>* outPathsCatToPos = new std::vector<std::pair<int, int>>[NB_TARGET];
	for (int i = 0; i < NB_TARGET; ++i)
		outPathsCatToPos[i] = std::vector<std::pair<int, int>>();
	bool* outSuccess = new bool[NB_TARGET];
	for (int i = 0; i < NB_TARGET; ++i)
		outSuccess[i] = false;

	std::function<float(std::pair<int, int>, std::pair<int, int>)> h = defaultHeuristic;

	std::pair<int, int>* targetIndices = new std::pair<int, int>[NB_TARGET];
	//i is the line index, j is the column index. We iterate over the column first (line fixed)
	for (int i = 0; i < boardSize.first; ++i) {
		for (int j = 0; j < boardSize.second; ++j) {
			targetIndices[j + i * boardSize.second] = std::pair<int, int>(i, j);
		}
	}
	//TODO: change findPaths to also return the outDistance so that it is easy to get the furstest distance ? 
	//Compute the shortest path for the cat to each cells of the board to find the cell that is the furthest away from the cat 
	AstarDataOriented::findPaths(catCell, targetIndices, NB_TARGET, h, board, boardSize, travelCostMap, outPathsCatToPos, outSuccess);

	float longestCellDistance = 0;
	std::pair<int, int> furthestCell = mouseCell;
	for (int i = 0; i < NB_TARGET; ++i)
		if (outSuccess[i]) {
			float cellDistance = getPathTotalDistance(outPathsCatToPos[i]);
			if (cellDistance > longestCellDistance) {
				longestCellDistance = cellDistance;
				furthestCell = targetIndices[i];
			}
		}
	
	std::pair<int, int>* targetIndicesMouse = new std::pair<int,int>(furthestCell);
	//outPath = new std::vector<std::pair<int, int>>();
	bool* outSuccessMouse = new bool(false);
	//Now move the mouse to the fursthest cell
	AstarDataOriented::findPaths(mouseCell, targetIndicesMouse, 1, h, fearStrength, fearArea, catCell, sameCellTypePenalty, board, boardSize, travelCostMap, outPath, outSuccessMouse);

	//Delete cells and set nullptr if no path is found 
	if (!outSuccessMouse[0]) {
		delete outPath;
		outPath = nullptr;
	}

	delete[] outPathsCatToPos;
	delete[] outSuccess;
	delete[] targetIndices;

	outPathsCatToPos = nullptr;
	outSuccess = nullptr;
	targetIndices = nullptr;

	delete outSuccessMouse;
	delete targetIndicesMouse;

	outSuccessMouse = nullptr;
	targetIndicesMouse = nullptr;
}