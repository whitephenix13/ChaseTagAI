#pragma once
#include <utility>

struct hash_int_pair {
	size_t operator()(const std::pair<int, int>& p) const {
		auto hash1 = std::hash<int>{}(p.first);
		auto hash2 = std::hash<int>{}(p.second);
		return hash1 ^ hash2;
	}
};