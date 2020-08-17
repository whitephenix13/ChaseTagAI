#include "../Pch/pch.h"
#include "AstarNode.h"


AstarNode::AstarNode() {
	totalCost = 0;
}
AstarNode::AstarNode(const AstarNode& copy) {
	this->cellIndex = copy.cellIndex;
	this->totalCost = copy.totalCost;
}
AstarNode::AstarNode(std::pair<int, int> cellIndex) {
	this->cellIndex = cellIndex;
	totalCost = 0;
}

// the top of the priority queue is the greatest element by default,
// but we want the smallest, so flip the sign
bool AstarNode::operator<(const AstarNode&n2)
{
		return this->totalCost > n2.totalCost;
}

AstarNode::~AstarNode() {
	
}