#pragma once
#include "AstarNodeDataOriented.h"
class AstarNodeComparatorDataOriented {
public:
    bool operator() (AstarNodeDataOriented&, AstarNodeDataOriented&);
};