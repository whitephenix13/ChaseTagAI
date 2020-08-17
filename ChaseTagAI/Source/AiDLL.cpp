#include "Pch/pch.h"
#include "AiDLL.h"
#include "Astar/Astar.h"
#include <tgmath.h>
float getCellPosition(float worldPos, int boardSize, float cellSize) {
    float offset = (boardSize + 1) % 2 / 2.0f;
    return offset + round((worldPos - offset)/cellSize) * cellSize;
}
//extern "C" is necessary to export the reference from our functions to the DLL
extern "C" {
	void MouseAIAction(CELL_TYPE* board, int* boardSize, float cellSize, float* cellOffset, float* mousePos, float* catPos, int actionsSize, float* outActions) {
		outActions[0] = catPos[0] <0 ? 1:-1;
		outActions[1] = catPos[1] < 0 ? 1 : -1;
		
        if (outActions[0] * mousePos[0] > ((boardSize[0]-1) / 2))
            outActions[0] = 0;
        if (outActions[1] * mousePos[1] > ((boardSize[1]-1) / 2))
            outActions[1] = 0;
        //TODO: do not forget to free memory (especially from Astar)
	}
}

