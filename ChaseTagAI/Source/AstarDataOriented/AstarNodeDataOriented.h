#pragma once
#include <utility>
class AstarNodeDataOriented {
public:
    int cellIndex;
    float totalCost;
    AstarNodeDataOriented();
    AstarNodeDataOriented(const AstarNodeDataOriented& copy);
    AstarNodeDataOriented(int cellIndex);
    // the top of the priority queue is the greatest element by default,
// but we want the smallest, so flip the sign
    bool operator < (const AstarNodeDataOriented&);
    ~AstarNodeDataOriented();

};
