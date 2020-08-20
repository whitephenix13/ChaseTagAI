#include "../Pch/pch.h"
#include "BoardPositionConverter.h"
#include <math.h>
std::pair<int, int> BoardPositionConverter::cellPosToIndex(std::pair<float, float> pos, std::pair<int, int> boardSize, std::pair<float, float> offset) {
	return  std::make_pair((pos.first - offset.first) + boardSize.first / 2 , (pos.second - offset.second) + boardSize.second / 2);
}

std::pair<float, float> BoardPositionConverter::cellIndexToPos(std::pair<int, int> index, std::pair<int, int> boardSize, std::pair<float, float> offset){
	return  std::make_pair((index.first + offset.first) - boardSize.first / 2, (index.second + offset.second) - boardSize.second / 2);
}

CELL_TYPE BoardPositionConverter::cellTypeAt(CELL_TYPE* board, int line, int column, std::pair<int, int> boardSize) {
	return board[line * boardSize.second + column];
}