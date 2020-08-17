#pragma once
#include <utility>
class AstarNode {
    public:
        std::pair<int, int> cellIndex;
        float totalCost;
        AstarNode();
        AstarNode(const AstarNode& copy);
        AstarNode(std::pair<int,int> cellIndex);
        // the top of the priority queue is the greatest element by default,
// but we want the smallest, so flip the sign
        bool operator < (const AstarNode&);
        ~AstarNode();

};
