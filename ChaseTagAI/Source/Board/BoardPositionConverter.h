#pragma once
#include "CellType.h"
#include <utility>
namespace BoardPositionConverter {
	/// <summary>
	/// DONT FORGET TO DELETE POINTER
	/// </summary>
	/// <param name="pos"></param>
	/// <param name="boardSize"></param>
	/// <param name="offset"></param>
	/// <returns></returns>
	std::pair<int, int> cellPosToIndex(std::pair<float, float> pos, std::pair<int, int> boardSize, std::pair<float, float> offset);
	/// <summary>
	/// DONT FORGET TO DELETE POINTER
	/// </summary>
	/// <param name="index"></param>
	/// <param name="boardSize"></param>
	/// <param name="offset"></param>
	/// <returns></returns>
	std::pair<float, float> cellIndexToPos(std::pair<int, int> index, std::pair<int, int> boardSize, std::pair<float, float> offset);
	CELL_TYPE cellTypeAt(CELL_TYPE* board, int x, int y, std::pair<int, int> boardSize);
};