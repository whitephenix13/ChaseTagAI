#pragma once
#include "AstarNode.h"
class AstarNodeComparator {
public:
    bool operator() (AstarNode&, AstarNode&);
};