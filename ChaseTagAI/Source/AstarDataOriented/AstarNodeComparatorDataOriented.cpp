#include "../Pch/pch.h"
#include "AstarNodeComparatorDataOriented.h"

/// <summary>
/// Returns the highest priority node which is the node with the lowest cost 
/// </summary>
/// <param name="node1"></param>
/// <param name="node2"></param>
/// <returns></returns>
bool AstarNodeComparatorDataOriented::operator()(AstarNodeDataOriented& node1, AstarNodeDataOriented& node2) {
	return node1.totalCost > node2.totalCost;
}