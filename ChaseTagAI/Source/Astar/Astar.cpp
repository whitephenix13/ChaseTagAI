#include "../Pch/pch.h"
#include "Astar.h"
#include <functional>
#include "../Board/BoardPositionConverter.h"
#include <limits>

//TODO: unordered_map does not works with pair, either use a class or create custom hash https://www.tutorialspoint.com/how-to-create-an-unordered-map-of-pairs-in-cplusplus
//TODO: se renseigner sur https://www.codeproject.com/Articles/1083210/An-Efficient-Cplusplus-Fixed-Block-Memory-Allocato
//TODO: need to find a class (create one?) that is a priority queue (for finding min value efficiently) but also that can 1) check existence of element and update its g/f score in the open set
//TODO: reference for implementation
//TODO: try with precomputation of distance to avoid cache miss
//TODO: try to avoid using classs when not necessary 
/*
Need algorithm that converts double index to single index 

*/
//Anonymous namespace to hide local helper functions
namespace {
    float distance(std::pair<int, int> current, std::pair<int, int> neighbor) {
        return sqrt(pow(current.first - neighbor.first, 2) + pow(current.second - neighbor.second, 2));
    }
    bool isCellValid(CELL_TYPE* board, std::pair<int, int> xy, std::pair<int, int> boardSize) {
        bool indexInBound = xy.first >= 0 && xy.first < boardSize.first&& xy.second >= 0 && xy.second < boardSize.second;
        return  indexInBound && BoardPositionConverter::cellTypeAt(board, xy.first, xy.second, boardSize) == CELL_TYPE::EMPTY;
    }

    std::vector<std::pair<int, int>*> getNeighbors(std::pair<int, int> cellIndex, CELL_TYPE* board, std::pair<int, int> boardSize) {
        std::vector<std::pair<int, int>*> res;

        //  +(-1,-1)   +(0,-1)  +(1,-1)
        //  +(-1,0)  cellIndex  +(1,0)
        //  +(-1,1)    +(0,1)   +(1,1)
        //

        //Handle direct neighbor cells: cells that are left,right,bottom and top of the current cell
        bool* cellValidArray = new bool[4];
        //direct neighbors are +(-1,0) +(0,-1) +(1,0) + (0,1)
        std::pair<int, int> directNeighborCells[4] = { std::make_pair(-1 + cellIndex.first,0 + cellIndex.second),std::make_pair(0+cellIndex.first,-1 + cellIndex.second),
            std::make_pair(1 + cellIndex.first,0+cellIndex.second),std::make_pair(0 + cellIndex.first,1 + cellIndex.second) };
        for (int i = 0; i < 4; ++i) {
            bool cellValidity = isCellValid(board, directNeighborCells[i], boardSize);
            cellValidArray[i]= cellValidity;
            if (cellValidity)
                res.push_back(new std::pair<int, int>((directNeighborCells[i])));
        }
        //handle indirect cells: cells that are diagonal to the current cell
        //indirect neighbors are +(-1,-1) +(1,-1) +(1,1) + (-1,1)
        std::pair<int, int> indirectNeighborCells[4] = { std::make_pair(-1 + cellIndex.first,-1 + cellIndex.second),std::make_pair(1 + cellIndex.first,-1 + cellIndex.second)
            ,std::make_pair(1 + cellIndex.first,1 + cellIndex.second),std::make_pair(-1 + cellIndex.first,1 + cellIndex.second) };
        for (int i = 0; i < 4; ++i) {
            bool cellValidity = isCellValid(board, indirectNeighborCells[i], boardSize);

            //diagonal cell is valid if and only if the direct cells surrounding it are valid
            if (cellValidity && cellValidArray[i] && cellValidArray[(i + 1) % 4])
                res.push_back(new std::pair<int, int>(indirectNeighborCells[i]));
        }
        delete[] cellValidArray;
        return res;
    }
     void reconstructPath(std::unordered_map<std::pair<int, int>, std::pair<int, int>, hash_pair_int> cameFrom, std::pair<int, int> endNode, std::vector<std::pair<int, int>>* outPath) {
        //Add the end node and create a new value so that it can be returned from the function
         outPath->push_back(std::pair<int, int>(endNode));

        std::pair<int, int> currentNode = endNode;
        std::unordered_map<std::pair<int, int>, std::pair<int, int>, hash_pair_int>::iterator itrNextNode = cameFrom.find(currentNode);
        while (itrNextNode != cameFrom.end()) {
            currentNode = itrNextNode->second;
            //Add each node from the path and create a new value so that it can be returned from the function
            outPath->push_back(std::pair<int, int>(currentNode));
            itrNextNode = cameFrom.find(currentNode);
        }
        
        //The output path is as followed [endNode, endNode-1, ..., startNode]. As we access element with pop_back, the order is correct and we don't need to reverse it
    }
}

bool Astar::findPath(std::pair<int,int> startCellIndex, std::pair<int, int> targetCellIndex, std::function<float(const std::pair<int, int>&, const std::pair<int, int>&)> h, CELL_TYPE* board, std::pair<int, int> boardSize, std::vector<std::pair<int, int>>* outPath) {
    const int boardFlatSize = boardSize.first * boardSize.second;
    AstarNode startCell = AstarNode(startCellIndex);
    //Queue containing the node to expand by priority. The highest priority node can be retrieved easily
    std::priority_queue<AstarNode, std::vector<AstarNode>, AstarNodeComparator> openQueue;
    //Add the starting point to the list of node to expand
    openQueue.push(startCell);
    //Set containing the node that we already visited once (those node should be ignored if found in the open set)
    std::unordered_set<std::pair<int, int>, hash_pair_int> closedSet;
    closedSet.reserve(boardFlatSize);
    //cameFrom[Node] represents the node immediately preceding it on the cheapest path from start
    // to node currently known.
    std::unordered_map<std::pair<int, int>, std::pair<int, int>,hash_pair_int> cameFrom;
    cameFrom.reserve(boardFlatSize);
    // For node n, gScore[n] is the cost of the cheapest path from start to n currently known.
    std::unordered_map<std::pair<int, int>, float, hash_pair_int> gScore;
    gScore.reserve(boardFlatSize);
    // gScore : = map with default value of Infinity
    gScore.insert({ {startCellIndex, 0} });
    // For node n, fScore[n] := gScore[n] + h(n). fScore[n] represents our current best guess as to
    // how short a path from start to finish can be if it goes through n.
    // fScore: = map with default value of Infinity
    std::unordered_map<std::pair<int, int>, float, hash_pair_int> fScore;
    fScore.reserve(boardFlatSize);
    fScore.insert({ {startCellIndex, h(startCell.cellIndex,targetCellIndex)} });
    while ((openQueue.size()>0)) {
        // current : = the node in openSet having the lowest fScore[] value
        AstarNode current = openQueue.top();
        openQueue.pop();//Actually removes the priority element
        //Only iterate on elements that are not in the closed set
        if (closedSet.find(current.cellIndex) != closedSet.end())
            continue;
        //Finally add the current node to the closed set as we don't want to explore it once again
        closedSet.insert(current.cellIndex);

        std::pair<int,int> currentCellIndex =current.cellIndex;
        if ((currentCellIndex.first == targetCellIndex.first) && (currentCellIndex.second == targetCellIndex.second)) {
            reconstructPath(cameFrom, current.cellIndex,outPath);
            return true;
        }
        std::vector<std::pair<int, int>*> neighbors = getNeighbors(currentCellIndex, board, boardSize);
        for (std::pair<int, int>* neighbor : neighbors) {
            // distance(current,neighbor) is the weight of the edge from current to neighbor

            // tentative_gScore is the distance from start to the neighbor through current
            float tentative_gScore = gScore.find(current.cellIndex)->second + distance(current.cellIndex, *neighbor);
            float neighbor_gScore = std::numeric_limits<float>::max();
            std::unordered_map<std::pair<int, int>, float,hash_pair_int>::iterator neighbor_gScoreItr = gScore.find(*neighbor);
            if (neighbor_gScoreItr != gScore.end())
                neighbor_gScore = neighbor_gScoreItr->second;
            if (tentative_gScore < neighbor_gScore) {
                // This path to neighbor is better than any previous one. Record it!
                AstarNode neighborCopy = AstarNode(*neighbor);//make a copy since pointer will be deleted 

                //cameFrom[neighbor] : = current
                std::pair<std::unordered_map<std::pair<int, int>, std::pair<int, int>,hash_pair_int>::iterator,bool> insertCameFromInfo= cameFrom.insert(std::pair<std::pair<int, int>, std::pair<int, int>>(neighborCopy.cellIndex, current.cellIndex));
                if (!insertCameFromInfo.second)
                    insertCameFromInfo.first->second = current.cellIndex;

                // gScore[neighbor] : = tentative_gScore
                std::pair<std::unordered_map<std::pair<int, int>, float,hash_pair_int>::iterator, bool> insertGScoreInfo = gScore.insert(std::pair<std::pair<int, int>, float>(neighborCopy.cellIndex, tentative_gScore));
                if (!insertGScoreInfo.second)
                    insertGScoreInfo.first->second = tentative_gScore;

                //fScore[neighbor] : = gScore[neighbor] + h(neighbor)
                float fScoreNeighbor = tentative_gScore + h(neighborCopy.cellIndex,targetCellIndex);
                std::pair<std::unordered_map<std::pair<int, int>, float,hash_pair_int>::iterator, bool> insertFScoreInfo = fScore.insert(std::pair<std::pair<int, int>, float>(neighborCopy.cellIndex, fScoreNeighbor));
                if (!insertFScoreInfo.second)
                    insertFScoreInfo.first->second = fScoreNeighbor;

                //We add the element to the open set regardless of whether it already exists.
                //This also means that when poping an element from the open set, we have to check if it is not in the closed set 
                neighborCopy.totalCost = fScoreNeighbor;
                openQueue.push(neighborCopy);
            }
        }

        for (std::pair<int, int>* neighbor : neighbors) {
            delete neighbor;
        }
    }
    // Open set is empty but goal was never reached
    return false;
}

