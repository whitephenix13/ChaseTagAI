#pragma once
#include <utility>
struct hash_pair_of_pair_int {
	size_t operator()(const std::pair<std::pair<int, int>,std::pair<int, int>>& p) const {
		auto hash1 = std::hash<int>{}(p.first.first);
		auto hash2 = std::hash<int>{}(p.first.second);
		auto hash3 = std::hash<int>{}(p.second.first);
		auto hash4 = std::hash<int>{}(p.second.second);
		return (hash1 ^ hash2) ^(hash3 ^ hash4);
	}
};