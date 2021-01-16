#include "Pch/pch.h"
#include "AiDLL.h"
#include "Astar/Astar.h"
#include "Actions/ActionHandler.h"
#include <tgmath.h>
#include "Strategy/PathFinding/FleePathFinding.h"
#include <utility>
#include <algorithm>

//TODO: should not be there. This is "unity specific"
float getCellPosition(float worldPos, int boardSize, float cellSize) {
    float offset = (boardSize + 1) % 2 / 2.0f;
    return offset + round((worldPos - offset)/cellSize) * cellSize;
}
//extern "C" is necessary to export the reference from our functions to the DLL
extern "C" {
	void MouseAIAction(CELL_TYPE* board, int* boardSize, float cellSize, float* cellOffset, float* mousePos, float* catPos, int actionsSize, float* outActions) {
        catPos[0] < 0 ? ActionHandler::moveRight(outActions) : ActionHandler::moveLeft(outActions);
        catPos[1] < 0 ? ActionHandler::moveUp(outActions) : ActionHandler::moveDown(outActions);
		
        if (outActions[0] * mousePos[0] > ((boardSize[0]-1) / 2))
            outActions[0] = 0;
        if (outActions[1] * mousePos[1] > ((boardSize[1]-1) / 2))
            outActions[1] = 0;
        //TODO: do not forget to free memory (especially from Astar)
	}

    void FindPath(CELL_TYPE* board, int* boardSize, float* mousePos, float* catPos, float fearStrength, float fearArea, float sameCellTypePenalty
        , CELL_TYPE* travelCostKeys, float* travelCostValues, int travelCostSize, int* outFlattenResultPath, int* outFlattenResultPathSize) {

        std::pair<int, int> boardSizePair(boardSize[0], boardSize[1]);
        std::pair<int, int> mousePosPair(mousePos[0], mousePos[1]);
        std::pair<int, int> catPosPair(catPos[0], catPos[1]);

        std::map<CELL_TYPE, float> travelCost;
        for(int i=0; i<travelCostSize;++i)
            travelCost.insert(std::pair<CELL_TYPE,int>(travelCostKeys[i], travelCostValues[i]));

        std::vector<std::pair<int, int>>* outPath = new std::vector<std::pair<int, int>>();
        FleePathFinding::execute(board, boardSizePair, travelCost,mousePosPair, catPosPair, fearStrength, fearArea, sameCellTypePenalty, outPath);

        if (outPath != nullptr) {
            //The number of element returned for the path has to be lower are equal to the container size (which is the default value of outFlattenResultPathSize)
            //Hence, we have to take at most outFlattenResultPathSize number of elements
            //Also we are flattening the result meaning that a path of size 10 will use 20 spaces in the result flattened array.
            *outFlattenResultPathSize = std::min(2*static_cast<int>(outPath->size()),*outFlattenResultPathSize);
            std::pair<int, int> pathElement;
            for (int i = 0; i < (*outFlattenResultPathSize) / 2; ++i) {
                pathElement = outPath->at(i);
                outFlattenResultPath[2 * i] = pathElement.first;
                outFlattenResultPath[2 * i + 1] = pathElement.second;
            }

        }
        else {
            *outFlattenResultPathSize = 0;
        }
    }

}

