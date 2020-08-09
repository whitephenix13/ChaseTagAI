#include "pch.h"
#include "AiDLL.h"


//extern "C" is necessary to export the reference from our functions to the DLL
extern "C" {
	void MouseAIAction(CELL_TYPE* board, int xBoardSize, int zBoardSize, float xMousePos, float zMousePos, float xCatPos, float zCatPos, int actionsSize, float* outActions ) {
		outActions[0] = xCatPos <0 ? 1:-1;
		outActions[1] = zCatPos < 0 ? 1 : -1;
		//TODO: do not force movement against the wall 
	}
}

