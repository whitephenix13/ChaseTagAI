#include "../Pch/pch.h"
#include "AstarDataOriented.h"
#include <unordered_map>
#include <vector>
#include "AstarNodeDataOriented.h"
#include <queue>
#include <unordered_set>
#include "AstarNodeComparatorDataOriented.h"
#include <iostream>
#include <bitset>


//Represents the index of a cell of the board. Starts at 0 for (0,0) and then increase by moving line by line (1 for(0,1) , boardsize.second for (1,0), ...)
typedef int CellIndex;
//Represents the index of a pair of cell of the board 0 for (0,0),(0,0), 1 for (0,1),(0,0) , ...
typedef int CellPairIndex;

namespace {
    inline const int CellToIndex(const std::pair<int, int>& cell, const std::pair<int, int>& boardSize) {
        //We want 0 -> (0,0), 1->(0,1) ... , boardsize.second -> (1,0)
        return cell.second + cell.first * boardSize.second;
    }
    inline const int CellPairToIndex(const CellIndex& cell1, const CellIndex& cell2, const std::pair<int, int>& boardSize) {
        //We want 0 -> (0,0),(0,0), 1 -> (0,1),(0,0) , boardsize.second -> (1,0),(0,0) boardsize.second*boardsize.first -> (0,0),(0,1)
        return  cell1 + cell2 * boardSize.second * boardSize.first;
    }
    inline const int CellPairToIndex(const std::pair<int, int>& cell1, const std::pair<int, int>& cell2, const std::pair<int, int>& boardSize) {
        //We want 0 -> (0,0),(0,0), 1 -> (0,1),(0,0) , boardsize.second -> (1,0),(0,0) boardsize.second*boardsize.first -> (0,0),(0,1)
        return CellPairToIndex(CellToIndex(cell1, boardSize), CellToIndex(cell2, boardSize),boardSize);
    }

    const std::pair<int,int> IndexToCell(const CellIndex& cellIndex, const std::pair<int, int>& boardSize) {
        return std::make_pair<int, int>(cellIndex / boardSize.second, cellIndex % boardSize.second);
    }
    const std::pair<std::pair<int, int>, std::pair<int, int>> IndexToCellPair(const CellPairIndex& cellPairIndex, const std::pair<int, int>& boardSize) {
        int cell1Index = cellPairIndex % (boardSize.first * boardSize.second);
        int cell2Index = cellPairIndex / (boardSize.first * boardSize.second);
        return std::pair<std::pair<int, int>, std::pair<int, int>>(IndexToCell(cell1Index, boardSize), IndexToCell(cell2Index, boardSize));
    }
    inline const float distance(std::pair<int, int>& node1, std::pair<int, int>& node2) {
        return sqrt(pow(node1.first - node2.first, 2) + pow(node1.second - node2.second, 2));
    }

    void getNeighborsAsArray(const CellIndex& cellIndex, const CELL_TYPE* board, const std::pair<int, int>& boardSize, CellIndex* outDirectNeighbors, CellIndex* outDiagonalNeighbors) {
        //Handle direct neighbor cells: cells that are left,right,bottom and top of the current cell
        bool cellValidArray[8];
        //left, top, right, bottom, top left, top right, bottom right, bottom left
        CellIndex neighborCells[8] = { cellIndex - 1, cellIndex - boardSize.second, cellIndex + 1, cellIndex + boardSize.second,
            cellIndex - boardSize.second - 1,cellIndex - boardSize.second + 1,cellIndex + boardSize.second + 1,cellIndex + boardSize.second - 1 };
        //left cell
        cellValidArray[0] = (cellIndex % boardSize.second) == 0 ? false : board[neighborCells[0]] == CELL_TYPE::EMPTY;
        //top cell
        cellValidArray[1] = (cellIndex / boardSize.second) == 0 ? false : board[neighborCells[1]] == CELL_TYPE::EMPTY;
        //right cell
        cellValidArray[2] = (cellIndex % boardSize.second) == (boardSize.second - 1) ? false : board[neighborCells[2]] == CELL_TYPE::EMPTY;
        //bottom cell
        cellValidArray[3] = (cellIndex / boardSize.second) == (boardSize.second - 1) ? false : board[neighborCells[3]] == CELL_TYPE::EMPTY;

        //top left
        cellValidArray[4] = (!cellValidArray[0] || !cellValidArray[1]) ? false : board[neighborCells[4]] == CELL_TYPE::EMPTY;
        //top right
        cellValidArray[5] = (!cellValidArray[1] || !cellValidArray[2]) ? false : board[neighborCells[5]] == CELL_TYPE::EMPTY;
        //bottom right
        cellValidArray[6] = (!cellValidArray[2] || !cellValidArray[3]) ? false : board[neighborCells[6]] == CELL_TYPE::EMPTY;
        //bottom left
        cellValidArray[7] = (!cellValidArray[3] || !cellValidArray[0]) ? false : board[neighborCells[7]] == CELL_TYPE::EMPTY;

        for (int i = 0; i < 4; ++i) {
            outDirectNeighbors[i] = cellValidArray[i]? neighborCells[i] : -1;
        }
        for (int i = 4; i < 8; ++i) {
            outDiagonalNeighbors[i-4] = cellValidArray[i] ? neighborCells[i] : -1;
        }
    }
  
    void reconstructPath(const CellIndex* cameFrom, const CellIndex& endNode, const std::pair<int, int>& boardSize, std::vector<std::pair<int, int>>* outPath) {
        //Add the end node and create a new value so that it can be returned from the function
        outPath->push_back(IndexToCell(endNode,boardSize));

        CellIndex currentNode = endNode;
        CellIndex nextNode = cameFrom[currentNode];
        while (nextNode!= -1) {
            currentNode = nextNode;
            //Add each node from the path and create a new value so that it can be returned from the function
            outPath->push_back(IndexToCell(currentNode, boardSize));
            nextNode = cameFrom[currentNode];
        }

        //The output path is as followed [endNode, endNode-1, ..., startNode]. As we access element with pop_back, the order is correct and we don't need to reverse it
    }

    void releaseMemory(int MAX_CELL_INDEX,CellIndex* neighbors,float* heuristicArray, float* gScoreArray, float* fScoreArray, bool* closedSetArray, CellIndex* cameFromArray
        , CellIndex** directNeighborsArray, CellIndex** diagonalNeighborsArray) {
        delete neighbors;
        neighbors = nullptr;
        delete[] heuristicArray;
        heuristicArray = nullptr;
        delete[] gScoreArray;
        gScoreArray = nullptr;
        delete[] fScoreArray;
        fScoreArray = nullptr;
        delete[] closedSetArray;
        closedSetArray = nullptr;
        delete[] cameFromArray;
        cameFromArray = nullptr;


        for (int ind = 0; ind < MAX_CELL_INDEX; ++ind) {
            delete[] directNeighborsArray[ind];
            directNeighborsArray[ind] = nullptr;
        }
        delete[] directNeighborsArray;
        directNeighborsArray = nullptr;

        for (int ind = 0; ind < MAX_CELL_INDEX; ++ind) {
            //TODO: error using delete with ind close to 1150 ...
            delete[] diagonalNeighborsArray[ind];
            diagonalNeighborsArray[ind] = nullptr;
        }
        delete[] diagonalNeighborsArray;
        diagonalNeighborsArray = nullptr;
    }
}

bool AstarDataOriented::findPath(const std::pair<int, int>& startCell, const std::pair<int, int>& targetCell, const std::function<float(const std::pair<int, int>&, const std::pair<int, int>&)> h, const CELL_TYPE* board, const std::pair<int, int>& boardSize, std::vector<std::pair<int, int>>* outPath) {
	

    const int MAX_CELL_INDEX = boardSize.first * boardSize.second;
    float* heuristicArray= new float[MAX_CELL_INDEX];
    CellIndex** directNeighborsArray = new CellIndex* [MAX_CELL_INDEX];
    for (int ind = 0; ind < MAX_CELL_INDEX; ++ind) {
        directNeighborsArray[ind] = new CellIndex[4]();
    }
    CellIndex** diagonalNeighborsArray = new CellIndex* [MAX_CELL_INDEX];
    for (int ind = 0; ind < MAX_CELL_INDEX; ++ind) {
        diagonalNeighborsArray[ind] = new CellIndex[4]();
    }
    //precompute distances
    //distance between two neighbors: [0] for direct neighbors, [1] for diagonal
    double distanceArray[2]{ 1,sqrt(2) };

    //Precompute heuristics
    for (int ind = 0; ind < MAX_CELL_INDEX; ++ind) {
        heuristicArray[ind] = h(IndexToCell(ind, boardSize), targetCell);
    }

    //Precompute neighboors

    for (int ind = 0; ind < MAX_CELL_INDEX; ++ind) {
        getNeighborsAsArray(ind, board, boardSize, directNeighborsArray[ind], diagonalNeighborsArray[ind]);
    }

    int startCellIndex = CellToIndex(startCell, boardSize);
    int targetCellIndex = CellToIndex(targetCell, boardSize);
    AstarNodeDataOriented startCellNode(startCellIndex);
    //Queue containing the node to expand by priority. The highest priority node can be retrieved easily
    std::priority_queue<AstarNodeDataOriented, std::vector<AstarNodeDataOriented>, AstarNodeComparatorDataOriented> openQueue;
    //Add the starting point to the list of node to expand
    openQueue.push(startCellNode);

    //Set containing the node that we already visited once (those node should be ignored if found in the open set)
    bool* closedSetArray= new bool[MAX_CELL_INDEX];//This could be optimized using a dynamic bitset but readability is worst
    for (int ind = 0; ind < MAX_CELL_INDEX; ++ind) 
        closedSetArray[ind] = false;
    //cameFrom[Node] represents the node immediately preceding it on the cheapest path from start
    // to node currently known.
    CellIndex* cameFromArray= new CellIndex[MAX_CELL_INDEX];
    for (int ind = 0; ind < MAX_CELL_INDEX; ++ind)
        cameFromArray[ind] = -1;
    // For node n, gScore[n] is the cost of the cheapest path from start to n currently known.
    float* gScoreArray = new float[MAX_CELL_INDEX];
    for (int ind = 0; ind < MAX_CELL_INDEX; ++ind)
        gScoreArray[ind] = std::numeric_limits<float>::infinity();
   
    // gScore : = map with default value of Infinity
    gScoreArray[startCellIndex] = 0;

    // For node n, fScore[n] := gScore[n] + h(n). fScore[n] represents our current best guess as to
    // how short a path from start to finish can be if it goes through n.
    // fScore: = map with default value of Infinity
    float* fScoreArray = new float[MAX_CELL_INDEX];
    for (int ind = 0; ind < MAX_CELL_INDEX; ++ind)
        fScoreArray[ind] = std::numeric_limits<float>::infinity();
    fScoreArray[startCellIndex] = heuristicArray[startCellIndex];

    AstarNodeDataOriented current;
    CellIndex* neighbors =nullptr;

    float tentative_gScore;
    float neighbor_gScore;
    while ((openQueue.size() > 0)) {
        // current : = the node in openSet having the lowest fScore[] value
        current = openQueue.top();
        openQueue.pop();//Actually removes the priority element

        //Only iterate on elements that are not in the closed set
        if (closedSetArray[current.cellIndex])
            continue;
        //Finally add the current node to the closed set as we don't want to explore it once again
        closedSetArray[current.cellIndex]=true;

        current.cellIndex = current.cellIndex;
        if (current.cellIndex == targetCellIndex) {
            reconstructPath(cameFromArray, current.cellIndex,boardSize, outPath);
            releaseMemory(MAX_CELL_INDEX, neighbors, heuristicArray, gScoreArray, fScoreArray, closedSetArray, cameFromArray, directNeighborsArray, diagonalNeighborsArray);
            return true;
        }

        for (int neighborTypeIndex = 0; neighborTypeIndex < 2; ++neighborTypeIndex) {
            neighbors = (neighborTypeIndex ==0? directNeighborsArray : diagonalNeighborsArray)[current.cellIndex];
            for (int i = 0; i < 4;++i) {
                CellIndex neighbor = neighbors[i];
                //only keep valid neighbors
                if (neighbor < 0)
                    continue;
                // distance(current,neighbor) is the weight of the edge from current to neighbor

                // tentative_gScore is the distance from start to the neighbor through current
                tentative_gScore = gScoreArray[current.cellIndex] + distanceArray[neighborTypeIndex];
                neighbor_gScore = gScoreArray[neighbor];
                if (tentative_gScore < neighbor_gScore) {
                    // This path to neighbor is better than any previous one. Record it!

                    //cameFrom[neighbor] : = current
                    cameFromArray[neighbor] = current.cellIndex;

                    // gScore[neighbor] : = tentative_gScore
                    gScoreArray[neighbor] = tentative_gScore;

                    //fScore[neighbor] : = gScore[neighbor] + h(neighbor)
                    fScoreArray[neighbor] = tentative_gScore + heuristicArray[neighbor];

                    //We add the element to the open set regardless of whether it already exists.
                    //This also means that when poping an element from the open set, we have to check if it is not in the closed set 

                    AstarNodeDataOriented neighborNode(neighbor);
                    neighborNode.totalCost = fScoreArray[neighbor];
                    openQueue.push(neighborNode);
                }
            }
        }
    }

    releaseMemory(MAX_CELL_INDEX, neighbors, heuristicArray, gScoreArray, fScoreArray, closedSetArray, cameFromArray, directNeighborsArray, diagonalNeighborsArray);
    
    // Open set is empty but goal was never reached
    return false;

}

