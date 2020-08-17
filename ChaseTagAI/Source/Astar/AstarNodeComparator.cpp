#include "../Pch/pch.h"
#include "AstarNodeComparator.h"

/// <summary>
/// Returns the highest priority node which is the node with the lowest cost 
/// </summary>
/// <param name="node1"></param>
/// <param name="node2"></param>
/// <returns></returns>
bool AstarNodeComparator::operator()(AstarNode& node1,AstarNode& node2) {
	return node1.totalCost> node2.totalCost;
}