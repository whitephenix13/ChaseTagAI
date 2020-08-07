#pragma once
#define AIDLL_API __declspec(dllexport) 

extern "C" {
	AIDLL_API bool Negate(bool val);
}