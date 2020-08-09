#pragma once
#include <utility>
#include <array>
#define AIDLL_API __declspec(dllexport) 

enum CELL_TYPE {
	EMPTY, WALL
};

extern "C" {
	AIDLL_API void MouseAIAction(CELL_TYPE* board,int xBoardSize,int zBoardSize, float xMousePos, float zMousePos,float xCatPos,float zCatPos, int actionsSize, float* outActions);
}

