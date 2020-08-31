#include "../Pch/pch.h"
#include "AstarDataOriented.h"
#include <unordered_map>
#include <vector>
#include "AstarNodeDataOriented.h"
#include <queue>
#include <unordered_set>
#include "AstarNodeComparatorDataOriented.h"
#include <iostream>


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

    const std::pair<int,int> IndexToCell(const CellIndex cellIndex, const std::pair<int, int>& boardSize) {
        return std::make_pair<int, int>(cellIndex / boardSize.second, cellIndex % boardSize.second);
    }
    const std::pair<std::pair<int, int>, std::pair<int, int>> IndexToCellPair(const CellPairIndex cellPairIndex, const std::pair<int, int>& boardSize) {
        int cell1Index = cellPairIndex % (boardSize.first * boardSize.second);
        int cell2Index = cellPairIndex / (boardSize.first * boardSize.second);
        return std::pair<std::pair<int, int>, std::pair<int, int>>(IndexToCell(cell1Index, boardSize), IndexToCell(cell2Index, boardSize));
    }
    inline const float distance(std::pair<int, int> node1, std::pair<int, int> node2) {
        return sqrt(pow(node1.first - node2.first, 2) + pow(node1.second - node2.second, 2));
    }

    void getNeighborsAsArray(CellIndex cellIndex, CELL_TYPE* board, std::pair<int, int> boardSize, CellIndex* outDirectNeighbors, CellIndex* outDiagonalNeighbors) {
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
  
    void reconstructPath(std::unordered_map<CellIndex, CellIndex> cameFrom, CellIndex endNode, std::pair<int, int> boardSize, std::vector<std::pair<int, int>>* outPath) {
        //Add the end node and create a new value so that it can be returned from the function
        outPath->push_back(IndexToCell(endNode,boardSize));

        CellIndex currentNode = endNode;
        std::unordered_map<CellIndex, CellIndex>::iterator itrNextNode = cameFrom.find(currentNode);
        while (itrNextNode != cameFrom.end()) {
            currentNode = itrNextNode->second;
            //Add each node from the path and create a new value so that it can be returned from the function
            outPath->push_back(IndexToCell(currentNode, boardSize));
            itrNextNode = cameFrom.find(currentNode);
        }

        //The output path is as followed [endNode, endNode-1, ..., startNode]. As we access element with pop_back, the order is correct and we don't need to reverse it
    }
}

bool AstarDataOriented::findPath(std::pair<int, int> startCell, std::pair<int, int> targetCell, std::function<float(const std::pair<int, int>&, const std::pair<int, int>&)> h, CELL_TYPE* board, std::pair<int, int> boardSize, std::vector<std::pair<int, int>>* outPath) {
	

    const int MAX_CELL_INDEX = boardSize.first * boardSize.second;
    
    std::unordered_map<CellIndex, float> heuristicMap;

    std::unordered_map<CellIndex, CellIndex*> directNeighborsMap;
    std::unordered_map<CellIndex, CellIndex*> diagonalNeighborsMap;
    //Precompute heuristics
    for (int ind = 0; ind < MAX_CELL_INDEX; ++ind) {
        heuristicMap.insert(std::make_pair(ind, h(IndexToCell(ind,boardSize), targetCell)));
    }
    //precompute distances
    float distanceArray[2]{ 1,sqrt(2) };//distance between adjacent cells and diagonal cells

    //Precompute neighboors
    for (int ind = 0; ind < MAX_CELL_INDEX; ++ind) {
        CellIndex* directNeighbors= new CellIndex[4]();
        CellIndex* diagonalNeighbors = new CellIndex[4]();
        getNeighborsAsArray(ind, board, boardSize, directNeighbors, diagonalNeighbors);
        directNeighborsMap.insert(std::make_pair(ind, directNeighbors));
        diagonalNeighborsMap.insert(std::make_pair(ind, diagonalNeighbors));
    }

    int startCellIndex = CellToIndex(startCell, boardSize);
    int targetCellIndex = CellToIndex(targetCell, boardSize);
    AstarNodeDataOriented startCellNode(startCellIndex);
    //Queue containing the node to expand by priority. The highest priority node can be retrieved easily
    std::priority_queue<AstarNodeDataOriented, std::vector<AstarNodeDataOriented>, AstarNodeComparatorDataOriented> openQueue;
    //Add the starting point to the list of node to expand
    openQueue.push(startCellNode);
    //Set containing the node that we already visited once (those node should be ignored if found in the open set)
    std::unordered_set<CellIndex> closedSet;
    //cameFrom[Node] represents the node immediately preceding it on the cheapest path from start
    // to node currently known.
    std::unordered_map<CellIndex, CellIndex> cameFrom;
    // For node n, gScore[n] is the cost of the cheapest path from start to n currently known.
    std::unordered_map<CellIndex, float> gScore;
    // gScore : = map with default value of Infinity
    gScore.insert({ {startCellIndex, 0} });
    // For node n, fScore[n] := gScore[n] + h(n). fScore[n] represents our current best guess as to
    // how short a path from start to finish can be if it goes through n.
    // fScore: = map with default value of Infinity
    std::unordered_map<CellIndex, float> fScore;
    fScore.insert({ {startCellIndex, heuristicMap.find(startCellIndex)->second} });

    AstarNodeDataOriented current;
    AstarNodeDataOriented neighborCopy;
    //std::vector<CellIndex>* neighbors = new std::vector<CellIndex>();
    CellIndex* neighbors;

    float tentative_gScore;
    float neighbor_gScore;
    float fScoreNeighbor;
    std::unordered_map<CellIndex, float>::iterator neighbor_gScoreItr;
    std::pair<std::unordered_map<CellIndex, CellIndex>::iterator, bool> insertCameFromInfo;
    std::pair<std::unordered_map<CellIndex, float>::iterator, bool> insertGScoreInfo;
    std::pair<std::unordered_map<CellIndex, float>::iterator, bool> insertFScoreInfo;
    while ((openQueue.size() > 0)) {
        // current : = the node in openSet having the lowest fScore[] value
        current = openQueue.top();
        openQueue.pop();//Actually removes the priority element
        //Only iterate on elements that are not in the closed set
        if (closedSet.find(current.cellIndex) != closedSet.end())
            continue;
        //Finally add the current node to the closed set as we don't want to explore it once again
        closedSet.insert(current.cellIndex);

        current.cellIndex = current.cellIndex;
        if (current.cellIndex == targetCellIndex) {
            reconstructPath(cameFrom, current.cellIndex,boardSize, outPath);
            return true;
        }

        for (int neighborTypeIndex = 0; neighborTypeIndex < 2; ++neighborTypeIndex) {
            neighbors = (neighborTypeIndex ==0? directNeighborsMap : diagonalNeighborsMap).find(current.cellIndex)->second;
            for (int i = 0; i < 4;++i) {
                CellIndex neighbor = neighbors[i];
                //only keep valid neighbors
                if (neighbor < 0)
                    continue;
                // distance(current,neighbor) is the weight of the edge from current to neighbor

                // tentative_gScore is the distance from start to the neighbor through current
                tentative_gScore = gScore.find(current.cellIndex)->second + distanceArray[neighborTypeIndex];
                neighbor_gScore = std::numeric_limits<float>::max();
                neighbor_gScoreItr = gScore.find(neighbor);
                if (neighbor_gScoreItr != gScore.end())
                    neighbor_gScore = neighbor_gScoreItr->second;
                if (tentative_gScore < neighbor_gScore) {
                    // This path to neighbor is better than any previous one. Record it!

                    //cameFrom[neighbor] : = current
                    insertCameFromInfo = cameFrom.insert(std::pair<int, int>(neighbor, current.cellIndex));
                    if (!insertCameFromInfo.second)
                        insertCameFromInfo.first->second = current.cellIndex;

                    // gScore[neighbor] : = tentative_gScore
                    insertGScoreInfo = gScore.insert(std::pair<CellIndex, float>(neighbor, tentative_gScore));
                    if (!insertGScoreInfo.second)
                        insertGScoreInfo.first->second = tentative_gScore;

                    //fScore[neighbor] : = gScore[neighbor] + h(neighbor)
                    fScoreNeighbor = tentative_gScore + heuristicMap.find(neighbor)->second;
                    insertFScoreInfo = fScore.insert(std::pair<CellIndex, float>(neighbor, fScoreNeighbor));
                    if (!insertFScoreInfo.second)
                        insertFScoreInfo.first->second = fScoreNeighbor;

                    //We add the element to the open set regardless of whether it already exists.
                    //This also means that when poping an element from the open set, we have to check if it is not in the closed set 
                    neighborCopy.totalCost = fScoreNeighbor;
                    openQueue.push(AstarNodeDataOriented(neighbor));
                }
            }
        }
    }
    for (int ind = 0; ind < MAX_CELL_INDEX; ++ind) {
        delete[] directNeighborsMap.find(ind)->second;
        delete[] diagonalNeighborsMap.find(ind)->second;
    }
    // Open set is empty but goal was never reached
    return false;

}
