#include "pch.h"
#include "AiDLL.h"
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
	}
}
//A* pseudo code
/*
function reconstruct_path(cameFrom, current)
    total_path := {current}
    while current in cameFrom.Keys:
        current := cameFrom[current]
        total_path.prepend(current)
    return total_path

// A* finds a path from start to goal.
// h is the heuristic function. h(n) estimates the cost to reach goal from node n.
function A_Star(start, goal, h)
    // The set of discovered nodes that may need to be (re-)expanded.
    // Initially, only the start node is known.
    // This is usually implemented as a min-heap or priority queue rather than a hash-set.
    openSet := {start}

    // For node n, cameFrom[n] is the node immediately preceding it on the cheapest path from start
    // to n currently known.
    cameFrom := an empty map

    // For node n, gScore[n] is the cost of the cheapest path from start to n currently known.
    gScore := map with default value of Infinity
    gScore[start] := 0

    // For node n, fScore[n] := gScore[n] + h(n). fScore[n] represents our current best guess as to
    // how short a path from start to finish can be if it goes through n.
    fScore := map with default value of Infinity
    fScore[start] := h(start)

    while openSet is not empty
        // This operation can occur in O(1) time if openSet is a min-heap or a priority queue
        current := the node in openSet having the lowest fScore[] value
        if current = goal
            return reconstruct_path(cameFrom, current)

        openSet.Remove(current)
        for each neighbor of current
            // d(current,neighbor) is the weight of the edge from current to neighbor
            // tentative_gScore is the distance from start to the neighbor through current
            tentative_gScore := gScore[current] + d(current, neighbor)
            if tentative_gScore < gScore[neighbor]
                // This path to neighbor is better than any previous one. Record it!
                cameFrom[neighbor] := current
                gScore[neighbor] := tentative_gScore
                fScore[neighbor] := gScore[neighbor] + h(neighbor)
                if neighbor not in openSet
                    openSet.add(neighbor)

    // Open set is empty but goal was never reached
    return failure
*/
