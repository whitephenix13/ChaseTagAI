#include "../Pch/pch.h"
#include "AstarNodeDataOriented.h"


AstarNodeDataOriented::AstarNodeDataOriented() {
	totalCost = 0;
	cellIndex = -1;
}
AstarNodeDataOriented::AstarNodeDataOriented(const AstarNodeDataOriented& copy) {
	this->cellIndex = copy.cellIndex;
	this->totalCost = copy.totalCost;
}
AstarNodeDataOriented::AstarNodeDataOriented(int cellIndex) {
	this->cellIndex = cellIndex;
	totalCost = 0;
}

// the top of the priority queue is the greatest element by default,
// but we want the smallest, so flip the sign
bool AstarNodeDataOriented::operator<(const AstarNodeDataOriented& n2)
{
	return this->totalCost > n2.totalCost;
}

AstarNodeDataOriented::~AstarNodeDataOriented() {

}